import os
import sequtils
import strformat
import strutils

type
  Position = array[2, int]

  Limits = object
    min: Position
    max: Position

  Sensor = object
    pos:      Position
    beacon:   Position
    distance: int

func equals(a: Position, b: Position): bool =
  return (b[0] == a[0]) and (b[1] == a[1])

func manh_dist(a: Position, b: Position): int =
  return abs(b[0] - a[0]) + abs(b[1] - a[1])

func parse_position(position: string): Position =
  let xy = position
    .replace("x=", "").replace("y=", "")
    .split(", ")
  return [xy[0].parseInt(), xy[1].parseInt()]

func parse_scan(line: string): Sensor =
  let parts = (line.split(":"))
  let positions = parts
    .map(func (str: string): Position =
      parse_position(str.split("at ")[1]))

  return Sensor(
    pos:      positions[0],
    beacon:   positions[1],
    distance: manh_dist(positions[0], positions[1]))

func contains(sensor: Sensor, pos: Position): (bool, int) =
  let dx = abs(pos[0] - sensor.pos[0])
  let dy = abs(pos[1] - sensor.pos[1])
  let dist = dx + dy
  let contained = dist <= sensor.distance

  var scan_to = pos[0]
  if contained:
    let add_x = abs(sensor.distance - dy)
    scan_to = sensor.pos[0] + add_x + 1

  return (contained, scan_to)

proc print_grid(sensors: seq[Sensor], limits: Limits): void =
  var str = ""
  for i in [0, 1]:
    for x in countup(limits.min[0], limits.max[0]):
      str.add(fmt"{x.intToStr():2}"[i])
    str.add("\n")

  for y in countup(limits.min[1], limits.max[1]):
    str.add(fmt"{y.intToStr():2} ")
    var x = limits.min[0]
    while x < limits.max[0]:
      var square = "."
      let pos = [x, y]
      var scanned = false

      for sensor in sensors:
        let (cont, scan_to) = sensor.contains(pos)
        if sensor.pos.equals(pos):
          square = "S"
          break
        elif sensor.beacon.equals(pos):
          square = "B"
          break
        elif cont:
          square = "#"
          if scan_to > x + 2:
            scanned = true
            str.add("#")
            while x < scan_to - 2:
              str.add("-")
              x += 1
            str.add("#")
      if not scanned:
        str.add(square)
      x += 1
    str.add("\n")
  echo(str)

proc part_1(sensors: seq[Sensor], limits: Limits, y: int): bool =
  var cnt = 0

  # print_grid(sensors[6..6], limits)

  for x in countup(limits.min[0], limits.max[0]):
    var contained = false
    let pos = [x, y]

    # var new_x = 0

    for sensor in sensors:
      let (cont, _) = sensor.contains(pos)
      if pos.equals(sensor.beacon):
        contained = false
        break
      elif cont:
        contained = true
        # new_x = scan_to

    if contained:
      cnt += 1
  echo(fmt"occupied positions: {cnt}")
  return true


proc part_2(sensors: seq[Sensor], max: int): bool =
  # print_grid(sensors, Limits(min: [0, 0], max: [max, max]))
  # var prev: Sensor = sensors[0]
  for y in countup(0, max):
    if (y mod 1000000) == 0:
      echo(fmt"y: {y}")
    var x = 0
    while x < max:
      let pos = [x, y]
      # let (_, cont) = prev.contains(pos)
      # if cont:
      #   continue
      var contained = false
      for sensor in sensors:
        let (cont, scan_to) = sensor.contains(pos)
        if cont:
          # prev = sensor
          contained = true
          # echo(fmt"{pos[0]},{pos[1]} - contained {dist} skipping from {x} to {nx}")
          x = scan_to
          break
      if not contained:
        echo(fmt"position: {pos} {pos[0] * 4000000 + pos[1]}")
        x = x + 1

  return true


proc day_15(filename: string, part: int, arg: int): bool =
  var contents = readFile(filename).strip()
  let lines = contents.splitLines()

  let sensors = lines.map(parse_scan)
  # var limits = Limits(min: [-4, -2], max: [25, 22])
  var limits = Limits(min: [high(int), high(int)], max: [low(int), low(int)])
  for sensor in sensors:
    for pos in [sensor.pos, sensor.beacon]:
      if pos[0] < limits.min[0]:
        limits.min[0] = pos[0] - 5000000
      if pos[1] < limits.min[1]:
        limits.min[1] = pos[1] - 5000000
      if pos[0] > limits.max[0]:
        limits.max[0] = pos[0] + 5000000
      if pos[1] > limits.max[1]:
        limits.max[1] = pos[1] + 5000000

  return
    if part == 0:
      part_1(sensors, limits, arg)
    else:
      part_2(sensors, arg)

when isMainModule:
  if paramCount() < 3:
    echo "Usage: give me a text file, a part and an int"
  elif day_15(paramStr(1), paramStr(2).parseInt(), paramStr(3).parseInt()):
    echo "\nall good"

