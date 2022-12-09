#!/usr/bin/env ts-node

import { readFileSync } from "fs";
import { basename } from "path";

type Pos = [number, number];

enum Dir
{
	R = "R",
	L = "L",
	U = "U",
	D = "D",
};

function rope_to_str(rope: Pos[], size: Pos): string
{
	let str = "";
	for (let y = size[1] - 1; y >= 0; y--)
	{
		for (let x = 0; x < size[0]; x++)
		{
			let found = false;
			for (let i = 0; i < rope.length; i++)
			{
				const knot   = rope[i];
				const symbol = i == 0
					? "H"
					: rope.length == 2
						? "T"
						: i.toString();
				if (x == knot[0] && y == knot[1])
				{
					str += symbol;
					found = true;
					break;
				}
			}
			if (!found)
				str += ".";
		}
		str += "\n";
	}

	str += "\n";

	return str;
}

function fix_length(head: Pos, tail: Pos): void
{
	const delta: Pos = [head[0] - tail[0], head[1] - tail[1]];
	const max = Math.max(...delta.map(e => Math.abs(e)));
	if (max >= 2)
	{
		for (let i = 0; i < 2; i++)
		{
			if (Math.abs(delta[i]) == 1)
				tail[i] = head[i];
			if (Math.abs(delta[i]) >= 2)
				tail[i] = head[i] - Math.sign(delta[i]);
		}
	}
}

function solve(
	lines:       string[],
	rope_length: number,
	init:        Pos = [0, 0],
	size:        Pos = [6, 5]): boolean
{
	const rope: Pos[] = [];
	for (let i = 0; i < rope_length; i++) rope.push([init[0], init[1]]);
	const visited: Set<string> = new Set();
	const head = rope[0];
	const tail = rope[rope.length - 1];

	for (let i = 0; i < lines.length; i++)
	{
		const line = lines[i].split(" ");

		const move = line[0];
		let times  = Number.parseInt(line[1]);

		while (times-- > 0)
		{
			switch (move)
			{
				case Dir.R:
					head[0] += 1
					break;

				case Dir.L:
					head[0] -= 1
					break;

				case Dir.U:
					head[1] += 1
					break;

				case Dir.D:
					head[1] -= 1
					break;

				default:
					break;
			}

			for (let j = 0; j < rope.length - 1; j++)
				fix_length(rope[j], rope[j + 1]);

			visited.add(tail.toString());
		}
		console.log(rope_to_str(rope, size));
	}

	console.log(`tail visited ${visited.size} unique spots`);

	return true;
}

function day_9(filename: string, part: number): boolean
{
	let data: string;
	try {
		data = readFileSync(filename, "utf8");
	} catch (error) {
		console.error(`failure to read file ${filename}`);
		return false;
	}

	const lines = data.split("\n");

	return part === 0 ? solve(lines, 2) : solve(lines, 10, [11, 15], [26, 20]);
}

function main()
{
	// life's too short to work out how to use the node-js C ABI,
	// let's just use a system call
	process.exitCode = 1;
	if (process.argv.length < 3)
		console.log(`Usage: ${basename(__filename)} -- give me a filename and a part`);
	else if (day_9(process.argv[2], Number.parseInt(process.argv[3])))
		process.exitCode = 0;
}

main()
