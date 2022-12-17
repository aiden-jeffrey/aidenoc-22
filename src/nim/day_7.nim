import std/algorithm

import os
import strformat
import strutils

# util
proc find_if[T](s: seq[T], pred: proc(x: T): bool): T =
  result = nil
  for i, x in s:
    if pred(x):
      result = x
      break

# node tree
type
  FileType = enum
    File
    Dir

  Node = ref object
    name:     string
    kind:     FileType
    size:     int
    children: seq[Node]
    parent:   Node

proc find_node(node: Node, pred: proc(n: Node): bool, results: var seq[Node]) =
  if pred(node):
    results.add(node)

  for child in node.children:
    find_node(child, pred, results)

proc print_node(node: Node, indent: int = 0): void =
  let padding = " ".repeat(indent)
  let size = if node.size > 0: fmt", size={node.size}" else: ""

  echo(fmt"{padding}- {node.name} ({node.kind}{size})")

  for child in node.children:
    print_node(child, indent + 2)

proc calc_sizes(node: Node): int =
  for child in node.children:
    node.size += calc_sizes(child)
  return node.size

# diractory operations
proc make_dir(name: string, parent: Node): Node =
  return Node(
        name: name,
        kind: Dir,
        children: @[],
        size: 0,
        parent: parent)

proc make_file(name: string, size: int, parent: Node): Node =
  return Node(
        name: name,
        kind: File,
        children: @[],
        size: size,
        parent: parent)

proc change_dir(cwd: Node, tokens: seq[string]): Node =
  if tokens.len < 3:
      echo("error: missing tokens in cd line")
      return nil

  let target = tokens[2]

  if target == "..":
    if cwd.isNil or cwd.parent.isNil:
      echo("error: invalid ..")
      return nil
    return cwd.parent
  else:
    if cwd.isNil:
      return make_dir(target, nil)
    else:
      let fp = cwd.children.find_if(proc (n: Node): bool = n.name == target)
      if fp.kind != Dir:
        echo(fmt"error: can't cd into file {target}")
        return nil
      return fp

proc parse_file_line(parent: Node, tokens: seq[string]): Node =
  if (tokens[0] == "dir"):
    return make_dir(tokens[1], parent)
  else:
    return make_file(tokens[1], tokens[0].parseInt(), parent)

proc parse_input(lines: seq[string]): Node =
  var cwd: Node = nil
  var lsing = false

  for i, line in lines:
    if line.len == 0:
      echo(fmt"error: blank line found at line {i}")
      return nil

    let tokens = line.splitWhitespace()
    if tokens[0] == "$":
      lsing = false
      let cmd = tokens[1]
      if cmd == "cd":
        cwd = change_dir(cwd, tokens)
        if cwd.isNil:
          echo("error: nil cd")
          return nil

      elif cmd == "ls":
        lsing = true
    elif lsing:
      let fp = parse_file_line(cwd, tokens)
      cwd.children.add(fp)

  # get root
  result = cwd
  while not result.parent.isNil:
    result = result.parent

proc part_1(root: Node): bool =
  var results: seq[Node] = @[]
  find_node(
    root,
    proc (n: Node): bool = n.kind == Dir and n.size < 100000,
    results)

  var sum = 0
  for result in results:
    sum += result.size
  echo("total size: ", sum)

proc part_2(root: Node): bool =
  const capacity = 70000000
  const required = 30000000

  let used    = root.size
  let to_free = required - (capacity - used)

  var directories: seq[Node] = @[]
  find_node(
    root,
    proc (n: Node): bool = n.kind == Dir and n.size >= to_free,
    directories)

  directories.sort(proc (x, y: Node): int = cmp(x.size, y.size))

  let solution = directories[0]
  echo(fmt"best option: {solution.name} ({solution.size})")

  return true

proc day_7(filename: string, part: int): bool =
  var contents = readFile(filename).strip()
  let lines = contents.splitLines()

  # parse lines into tree
  var root = parse_input(lines)
  if root.isNil:
    echo("error: reading file")
    return false

  # generate sizes
  let root_size = calc_sizes(root)
  echo("root size: ", root_size);

  # debug
  print_node(root)

  if part == 0:
    return part_1(root)
  else:
    return part_2(root)

when isMainModule:
  if paramCount() < 2:
    echo "Usage: give me a text file and a part"
  elif day_7(paramStr(1), paramStr(2).parseInt()):
    echo "\nall good"

