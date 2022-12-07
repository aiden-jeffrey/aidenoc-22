
def day_3(part: int, filename: str):
	with open(filename, "r") as fp:
		lines = fp.readlines()

		if part == 0:
			part_1(lines)
		if part == 1:
			part_2(lines)

	return True

def part_1(lines: str):
	sum = 0

	for rucksack in lines:
		common = find_common(rucksack)
		sum += priority(common)

	print(f"sum is {sum}")

def part_2(lines: str):
	sum = 0

	for group in range(0, len(lines), 3):
		found = False
		for item_0 in lines[group]:
			for item_1 in lines[group + 1]:
				if item_0 == item_1:
					for item_2 in lines[group + 2]:
						if item_1 == item_2:
							found = True
							sum += priority(item_0)
							break
				if found:
					break
			if found:
					break

	print(f"sum is {sum}")

def priority(item: str):
	ch_ord = ord(item)
	if ch_ord >= ord("a") and ch_ord <= ord("z"):
		return ch_ord - ord("a") + 1
	elif ch_ord >= ord("A") and ch_ord <= ord("Z"):
		return ch_ord - ord("A") + 27
	else:
		print(f"Error: invalid item {item}")

def find_common(rucksack: str) -> str:
	mid = len(rucksack) // 2
	lhs, rhs = rucksack[:mid], rucksack[mid:]

	for l in lhs:
		for r in rhs:
			if l == r:
				return l
