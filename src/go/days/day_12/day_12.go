package main

import (
	"fmt"
	"log"
	"math"
	"os"
	"strconv"
	"strings"
)

type Grid struct {
	squares [][]*Node
	width   int
	height  int
	start   *Node
	goal    *Node
}
type Set map[Position]*Node

type Position struct {
	x int
	y int
}

type Node struct {
	height    byte
	cost      int
	total_est int
	parent    *Node
	child     *Node
	position  Position
}

func make_node(position Position, height byte) *Node {
	node := Node{
		height,
		math.MaxInt,
		math.MaxInt,
		nil,
		nil,
		position,
	}
	return &node
}

func make_grid(str string, start_pos *Position) Grid {
	lines := strings.Split(string(str), "\n")

	width := len(lines[0])
	height := len(lines)

	squares := make([][]*Node, height)

	for y := range squares {
		squares[y] = make([]*Node, width)
	}

	var start *Node = nil
	var goal *Node = nil

	for y, line := range lines {
		bytes := []byte(line)
		for x, height := range bytes {
			node := make_node(Position{x, y}, height)

			if start_pos != nil && *start_pos == node.position {
				start = node
				node.height = 'a'
			} else if height == 'S' {
				start = node
				node.height = 'a'
			}

			if height == 'E' {
				goal = node
				node.height = 'z'
			}

			squares[y][x] = node
		}
	}

	if goal == nil || start == nil {
		fmt.Println(("error: undefined start / goal"))
	}

	start.cost = 0

	return Grid{squares, width, height, start, goal}
}

func print_grid(grid Grid) {
	fmt.Print("\n")
	for _, row := range grid.squares {
		for _, node := range row {
			if node.child != nil {
				dx := node.position.x - node.child.position.x
				dy := node.position.y - node.child.position.y
				if dx < 0 {
					fmt.Printf(">")
				} else if dx > 0 {
					fmt.Printf("<")
				} else if dy > 0 {
					fmt.Printf("^")
				} else if dy < 0 {
					fmt.Printf("v")
				} else {
					fmt.Printf("!")
				}
			} else {
				fmt.Printf("%c", node.height)
			}
		}
		fmt.Print("\n")
	}
	fmt.Print("\n")
}

func pop_lowest_estimated(set Set) *Node {
	lowest := math.MaxInt
	var out *Node = nil

	for _, node := range set {
		if out == nil || node.total_est < lowest {
			lowest = node.total_est
			out = node
		}
	}

	if out != nil {
		delete(set, out.position)
	}
	return out
}

func build_path(end *Node) []Position {
	var path []Position
	for node := end; node != nil; node = node.parent {
		if node.parent != nil {
			node.parent.child = node
		}
		path = append([]Position{node.position}, path...)
	}
	return path
}

func get_neighbours(from *Node, grid Grid) []*Node {
	var out []*Node
	steps := [][2]int{{-1, 0}, {0, -1}, {1, 0}, {0, 1}}
	for _, dxdy := range steps {
		x := from.position.x + dxdy[0]
		y := from.position.y + dxdy[1]
		if x >= 0 && x < grid.width && y >= 0 && y < grid.height {
			neighbour := grid.squares[y][x]
			if neighbour.height <= from.height+1 {
				out = append(out, neighbour)
			}
		}
	}
	return out
}

func a_star(grid Grid) ([]Position, bool) {
	start := grid.start
	goal := grid.goal
	open_set := Set{start.position: start}

	distance := func(from *Node, to *Node) int {
		return 1
	}

	estimate_to_goal := func(from *Node) int {
		return goal.position.x - from.position.x + goal.position.y - from.position.y
	}

	for len(open_set) > 0 {
		current := pop_lowest_estimated(open_set)
		if current.position == goal.position {
			return build_path(current), true
		}

		neighbours := get_neighbours(current, grid)

		for _, neighbour := range neighbours {
			temp_score := current.cost + distance(current, neighbour)
			if temp_score < neighbour.cost {
				neighbour.parent = current
				neighbour.cost = temp_score
				neighbour.total_est = temp_score + estimate_to_goal(neighbour)

				_, ok := open_set[neighbour.position]
				if !ok {
					open_set[neighbour.position] = neighbour
				}
			}
		}
	}

	return nil, false
}

func part_1(str string) bool {
	grid := make_grid(str, nil)
	print_grid(grid)
	path, ok := a_star(grid)

	if ok {
		fmt.Println("path reaches goal in ", len(path)-1, "steps")
		print_grid(grid)
		return true
	} else {
		fmt.Println("error finding path")
		return false
	}
}

func part_2(str string) bool {
	test_grid := make_grid(str, nil)

	var grids []Grid
	for _, row := range test_grid.squares {
		for _, node := range row {
			if node.height == 'a' {
				grids = append(grids, make_grid(str, &node.position))
			}
		}
	}

	shortest_dist := math.MaxInt
	for _, grid := range grids[1:] {
		path, ok := a_star(grid)
		if ok {
			dist := len(path) - 1
			fmt.Println("path reaches goal in ", dist, "steps")
			if dist < shortest_dist {
				shortest_dist = dist
			}
		} else {
			fmt.Println("error finding path")
		}
	}

	fmt.Println("\n shortest path length:", shortest_dist)
	return true
}

func main() {
	if len(os.Args) < 3 {
		fmt.Println("Usage", os.Args[0], "-- give me a filename and a part")
		return
	}

	b, err := os.ReadFile(os.Args[1])
	if err != nil {
		log.Fatal(err)
	}

	part, _ := strconv.Atoi(os.Args[2])
	str := string(b)

	if part == 0 {
		part_1(str)
	} else if part == 1 {
		part_2(str)
	}
}
