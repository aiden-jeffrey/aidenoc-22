
Range = tuple[int, int]
Pair = tuple[Range, Range]

def day_4(part: int, filename: str):
	with open(filename, "r") as fp:
		lines = fp.readlines()
		pairs = list(map(make_pair, lines))

	return part_1(pairs) if part == 0 else part_2(pairs)

def make_pair(line: str) -> tuple[Pair]:
	return tuple(map(
		lambda s: tuple(map(int, s.split("-"))),
		line.split(",")))

def part_1(pairs: list[Pair]) -> bool:
	cnt = 0
	for pair in pairs:
		delta_min = pair[0][0] - pair[1][0]
		delta_max = pair[0][1] - pair[1][1]
		if (delta_min <= 0 and delta_max >= 0) or (delta_min >= 0 and delta_max <= 0):
			cnt += 1
	print(f"fully contained pair count is {cnt}")
	return True


def part_2(pairs: list[Pair]) -> bool:
	cnt = 0
	for pair in pairs:
		delta_0 = pair[0][1] - pair[1][0]
		delta_1 = pair[1][1] - pair[0][0]
		if delta_0 >= 0 and delta_1 >= 0:
			cnt += 1
	print(f"overlapping pair count is {cnt}")
	return True
