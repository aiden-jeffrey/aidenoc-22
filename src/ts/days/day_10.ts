#!/usr/bin/env ts-node

import { readFileSync } from "fs";
import { basename } from "path";

abstract class Instruction
{
	abstract tick(x: number): [boolean, number];
}

class Noop implements Instruction
{
	tick(x: number): [boolean, number]
	{
		return [true, x];
	}
}

class Addx implements Instruction
{
	private timer = 2;
	private add: number;

	constructor(add: number)
	{
		this.add = add;
	}

	tick(x: number): [boolean, number]
	{
		return (--this.timer <= 0)
			? [true, x + this.add]
			: [false, x];
	}
}

function parse_line(line: string): Instruction
{
	const tokens = line.split(" ");
	return (line == "noop")
		? new Noop()
		: new Addx(Number.parseInt(tokens[1]));
}

function run_program(
	instructions: Instruction[],
	cb:           (cycle: number, x: number) => void): boolean
{
	let x = 1;
	let complete = false;
	let cycle = 0;

	for (
		let active: Instruction | undefined = instructions.pop();
		active;
		complete && (active = instructions.pop()))
	{
		cb(++cycle, x);
		[complete, x] = active.tick(x);
	}

	console.log(`at program completion, x = ${x}`);
	return true;
}

function part_1(instructions: Instruction[]): boolean
{
	let accum: number[] = [];
	const cb = (cycle: number, x: number) =>
	{
		if ((cycle - 20) % 40 == 0)
		{
			accum.push(x * cycle);
		}
	}
	const success = run_program(instructions, cb);
	if (success)
	{
		const sum = accum.reduce((prev, curr) => prev + curr, 0);
		console.log(`signal strength is ${sum}`);
	}

	return success;
}

function part_2(instructions: Instruction[]): boolean
{
	let lines: string[] = [];
	const cb = (cycle: number, x: number) =>
	{
		const pos   = (cycle - 1) % 40;
		const index = Math.floor((cycle - 1) / 40);
		if (lines[index] == undefined) lines[index] = "";

		lines[index] += (pos >= x - 1 && pos <= x + 1)
			? "#"
			: ".";
	}
	const success = run_program(instructions, cb);
	if (success)
	{
		for (const line of lines)
			console.log(line);
	}

	return success;
}

function day_10(filename: string, part: number): boolean
{
	let data: string;
	try {
		data = readFileSync(filename, "utf8");
	} catch (error) {
		console.error(`failure to read file ${filename}`);
		return false;
	}

	const instructions = data.split("\n").map(parse_line).reverse();

	return part === 0 ? part_1(instructions) : part_2(instructions);
}

function main()
{
	// life's too short to work out how to use the node-js C ABI,
	// let's just use a system call
	process.exitCode = 1;
	if (process.argv.length < 3)
		console.log(`Usage: ${basename(__filename)} -- give me a filename and a part`);
	else if (day_10(process.argv[2], Number.parseInt(process.argv[3])))
		process.exitCode = 0;
}

main()
