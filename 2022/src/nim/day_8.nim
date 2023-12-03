import os
import strformat
import strutils

type
  Direction = enum
    North
    South
    East
    West

  Matrix[T] = object
    width:  int
    height: int
    data:   seq[T]

  Tree = ref object
    height: int
    color:  set[Direction]
    score:  int

func make_matrix[T](width: int, height: int): Matrix[T] =
  return Matrix[T](
    width:  width,
    height: height,
    data:   newSeq[T](width * height))

func make_tree(ch: char): Tree =
  return Tree(height: ord(ch) - ord('0'), color: {}, score: 1)

proc parse_lines(lines: seq[string]): Matrix[Tree] =
  result = make_matrix[Tree](lines[0].len, lines.len)
  for y, line in lines:
    for x, ch in line:
      result.data[x * result.height + y] = make_tree(ch)

proc shade_trees(trees: Matrix[Tree], direction: Direction): void =
  var max_height = 0
  case direction
  of North:
    for x in countup(0, trees.width - 1):
      max_height = -1
      for y in countup(0, trees.height - 1):
        var tree = trees.data[x * trees.height + y]
        if tree.height > max_height:
          max_height = tree.height
          tree.color.incl(North)
  of South:
    for x in countup(0, trees.width - 1):
      max_height = -1
      for y in countdown(trees.height - 1, 0):
        var tree = trees.data[x * trees.height + y]
        if tree.height > max_height:
          max_height = tree.height
          tree.color.incl(South)
  of East:
    for y in countup(0, trees.height - 1):
      max_height = -1
      for x in countup(0, trees.width - 1):
        var tree = trees.data[x * trees.height + y]
        if tree.height > max_height:
          max_height = tree.height
          tree.color.incl(East)
  of West:
    for y in countup(0, trees.height - 1):
      max_height = -1
      for x in countdown(trees.width - 1, 0):
        var tree = trees.data[x * trees.height + y]
        if tree.height > max_height:
          max_height = tree.height
          tree.color.incl(West)

proc score_tree(trees: Matrix[Tree], x: int, y: int): void =
  let tree = trees.data[x * trees.height + y]

  for direction in North..West:
    var dx: int
    var dy: int
    case direction
    of North:
      dx = 0
      dy = -1
    of South:
      dx = 0
      dy = 1
    of East:
      dx = -1
      dy = 0
    of West:
      dx = 1
      dy = 0

    var distance = 0
    var X = x + dx
    var Y = y + dy

    while (X >= 0) and (X < trees.width) and (Y >= 0) and (Y < trees.height):
      let other = trees.data[X * trees.height + Y]

      distance += 1

      if other.height >= tree.height:
        break

      X += dx
      Y += dy

    tree.score *= distance

proc part_1(trees: Matrix[Tree]): bool =
  for direction in North..West:
    trees.shade_trees(direction)

  # print colors
  var str = ""
  for y in countup(0, trees.height - 1):
    for x in countup(0, trees.width - 1):
      let tree = trees.data[x * trees.height + y]
      str.add(fmt"{card(tree.color)}")
    str.add("\n")
  echo(str)

  var cnt = 0
  for tree in trees.data:
    if (card(tree.color) != 0):
      cnt += 1

  echo(fmt"there are {cnt} visible trees")

proc part_2(trees: Matrix[Tree]): bool =
  for y in countup(0, trees.height - 1):
    for x in countup(0, trees.width - 1):
      trees.score_tree(x, y)

  # print scores
  var str = ""
  for y in countup(0, trees.height - 1):
    for x in countup(0, trees.width - 1):
      let tree = trees.data[x * trees.height + y]
      str.add(fmt"{tree.score}")
    str.add("\n")
  echo(str)

  var max = 0
  for tree in trees.data:
    if tree.score > max:
      max =tree.score

  echo(fmt"max score is {max}")

  return true


proc day_8(filename: string, part: int): bool =
  var contents = readFile(filename).strip()
  let lines = contents.splitLines()

  var trees = parse_lines(lines)

  return if part == 0: part_1(trees) else: part_2(trees)

when isMainModule:
  if paramCount() < 2:
    echo "Usage: give me a text file and a part"
  elif day_8(paramStr(1), paramStr(2).parseInt()):
    echo "\nall good"

