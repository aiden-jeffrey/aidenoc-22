package main

import (
	"fmt"
	"log"
	"os"
	"sort"
	"strconv"
	"strings"
)

type Inspect func(item *int)
type Throw func(item *int) uint

type Monkey struct {
	id       int
	items    []*int
	count    *int
	inspect  Inspect
	test_div int
	throw    Throw
}

func parse_operation(item *int, tokens []string) {
	operand := func(token string) *int {
		if token == "old" {
			return item
		} else {
			i, _ := strconv.Atoi(token)
			return &i
		}
	}

	a := operand(tokens[0])
	b := operand(tokens[2])

	if tokens[1] == "*" {
		*item = *a * *b
	} else if tokens[1] == "+" {
		*item = *a + *b
	} else if tokens[1] == "-" {
		*item = *a - *b
	} else if tokens[1] == "/" {
		*item = *a / *b
	} else {
		fmt.Println("error: invalid operation")
	}
}

func make_monkey(desc string) Monkey {
	lines := strings.Split(desc, "\n")

	// this is hateful, but i hate regex
	// id
	id_str := strings.Split(
		strings.Replace(lines[0], ":", "", -1), " ")[1]
	id, _ := strconv.Atoi(id_str)

	// items
	items_str := strings.Split(strings.Split(
		strings.Replace(lines[1], " ", "", -1), ":")[1], ",")
	items := make([]*int, len(items_str))

	for i, x := range items_str {
		val, _ := strconv.Atoi(x)
		items[i] = &val
	}

	cnt := 0
	pcnt := &cnt

	// operation
	oper_str := strings.Split(strings.Split(lines[2], "= ")[1], " ")
	operation := func(item *int) {
		*pcnt++
		parse_operation(item, oper_str)
	}

	// throw
	test_div, _ := strconv.Atoi(
		strings.Split(lines[3], "divisible by ")[1])
	if_true, _ := strconv.Atoi(
		strings.Split(lines[4], "monkey ")[1])
	if_false, _ := strconv.Atoi(
		strings.Split(lines[5], "monkey ")[1])
	throw := func(item *int) uint {
		if (*item % test_div) == 0 {
			return uint(if_true)
		} else {
			return uint(if_false)
		}
	}

	return Monkey{
		id,
		items,
		pcnt,
		operation,
		test_div,
		throw,
	}
}

func run_round(monkeys []Monkey, divide bool, prime_product int) {
	var item *int
	for from := range monkeys {
		num_items := len(monkeys[from].items)
		for i := 0; i < num_items; i++ {
			item, monkeys[from].items =
				monkeys[from].items[0], monkeys[from].items[1:]

			monkeys[from].inspect(item)
			if divide {
				*item = *item / 3
			} else {
				*item = *item % prime_product
			}

			to := monkeys[from].throw(item)
			monkeys[to].items = append(monkeys[to].items, item)
		}
	}
}

func print_round(monkeys []Monkey) {
	for _, monkey := range monkeys {
		fmt.Println(monkey.id, monkey.items)
	}
	fmt.Println("------")
}

func simulate(monkeys []Monkey, num_rounds int, divide bool, prime_product int) {

	for i := 0; i < num_rounds; i++ {
		run_round(monkeys, divide, prime_product)
	}

	print_round(monkeys)

	for _, monkey := range monkeys {
		fmt.Println("monkey", monkey.id, "inspected items", *monkey.count, "times.")
	}

	sort.Slice(monkeys, func(i, j int) bool {
		return *monkeys[i].count > *monkeys[j].count
	})

	fmt.Println("monkey business:", (*monkeys[0].count)*(*monkeys[1].count))
}

func part_1(monkeys []Monkey) {
	simulate(monkeys, 20, true, 0)
}

func part_2(monkeys []Monkey, prime_product int) {
	simulate(monkeys, 10000, false, prime_product)
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

	chunks := strings.Split(string(b), "\n\n")
	monkeys := make([]Monkey, 0)

	prime_product := 1

	for _, line := range chunks {
		monkey := make_monkey(line)
		prime_product *= monkey.test_div
		monkeys = append(monkeys, monkey)
	}

	if part == 0 {
		part_1(monkeys)
	} else {

		part_2(monkeys, prime_product)
	}
}
