#!/usr/bin/env ts-node

import { readFileSync } from "fs";
import { basename } from "path";

class Valve
{
	children: Valve[] = [];
	name: string;
	flow_rate: number;

	constructor(name: string, flow_rate: number)
	{
		this.name = name;
		this.flow_rate = flow_rate;
	}

	parseChildren(line: string, cache: Map<string, Valve>)
	{
		const tokens = line.split("; ")[1]
			.split(" ")
			.slice(4)
			.map(ss => ss.replace(",", ""));
		for (const name of tokens)
		{
			const child = cache.get(name);
			if (!child)
			{
				console.error(`missing child ${name}`);
				return;
			}
			this.children.push(child);
		}
	}

	static fromLine(line: string): Valve
	{
		const tokens = line.split(";")[0].split(" ");
		const name = tokens[1];
		const rate = Number.parseInt(tokens[4].split("=")[1]);

		return new Valve(name, rate);
	}
}

interface Open
{
	type: "open";
	valve: Valve;
}

interface MoveTo
{
	type: "move-to";
	valve: Valve;
}

type Operation = Open | MoveTo;

function opToStr(oper: Operation)
{
	if (oper.type == "open")
		return `o${oper.valve.name}`;
	else if (oper.type == "move-to")
		return `m${oper.valve.name}`;
}

// function strToOp(str: string)
// {
// 	return
// }

class Solution
{
	path: Operation[] = [];
	opened: Set<Valve> = new Set();
	capacity: number;
	pressure: number = 0;
	cnt = 0;

	constructor(capacity: number)
	{
		this.capacity = capacity;
	}

	add(operation: Operation): boolean
	{
		this.path.push(operation);

		if (operation.type == "open")
		{
			const v = operation.valve;
			if (this.opened.has(v)) return false;

			this.opened.add(v);
			return true
		}

		return true;
	}

	pop()
	{
		const oper = this.path.pop();
		if (oper && oper.type == "open" && this.opened.has(oper.valve))
		{
			this.opened.delete(oper.valve);
			this.pressure -= oper.valve.flow_rate;
		}
	}

	tick()
	{
		this.cnt++;
		let pressure = 0;
		for (const v of this.opened) pressure += v.flow_rate
		this.pressure += pressure;
	}

	peek(to: number)
	{
		let pressure = this.pressure;
		for (let i = this.cnt; i < to; i++)
		{
			for (const v of this.opened) pressure += v.flow_rate
		}
		return pressure;
	}

	current(): Valve | undefined
	{
		return this.path[this.path.length - 1]?.valve;
	}

	clone(): Solution
	{
		const p = new Solution(this.capacity);
		p.path = [...this.path];
		p.opened = new Set(this.opened);
		p.pressure = this.pressure;
		p.cnt = this.cnt;
		return p
	}

	hash(withEnd: boolean): string
	{
		const list: string[] = [];
		for (const open of this.opened) list.push(open.name);
		// if (!withEnd)
		// {
		// 	const last = this.path[this.path.length - 1];
		// 	if (last.type == "open") list.pop();
		// 	// list.pop();
		// }
		return list.sort().join("-") + ` (${this.current()?.name})`;
	}

	toString(): string
	{
		return this.path.map(o => opToStr(o)).join(" ");
	}

	print(): void
	{
		console.log(this.toString());
		console.log([...this.opened.values()].map(v => v.name + v.flow_rate));
		console.log(this.pressure);
		console.log(this.cnt);
		console.log("")
	}

	// static fromString(str: string): Solution
	// {

	// }
}

// memoise on cnt and opened mask
//
//

function maxPath(
	solution: Solution,
	t:        number,
	tEnd:     number,
	memo:     Map<string, Solution>,
	bestMemo: Map<string, Solution>): Solution {
	if (t >= tEnd) return solution;

	const current = solution.current();
	if (!current)
	{
		console.error("blank solution")
		return solution;
	}

	const solved = memo.get(solution.hash(true) + t.toString());
	if (solved)
	{
		// console.log(`solved ${memo.size}`)
		return solved;
	}

	solution.tick();

	const key = `${t}-${current.name}`;
	const best = bestMemo.get(key);
	if (!best || best.pressure < solution.pressure)
	{
		bestMemo.set(key, solution);
	}
	else if (best)
	{
		solution = best;
		// solution.pop();
	}

	const operations: Operation[] = solution.opened.has(current) || current.flow_rate == 0
		? []
		: [{ type: "open", valve: current }];

	for (const neighbour of current.children)
		operations.push({ type: "move-to", valve: neighbour });

	let final = solution;
	for (const child of operations)
	{
		let subSolution = solution.clone();

		subSolution.add(child);
		subSolution = maxPath(subSolution, t + 1, tEnd, memo, bestMemo);

		if (subSolution.pressure > final.pressure) {
			final = subSolution;
		}
	}

	// console.log(`${final.cnt}`)

	// const maxAccumulator = memo.get(`${t + 1}`);
	// if (!maxAccumulator || final.pressure > maxAccumulator.pressure)
	// {
	// 	memo.set(`${t + 1}`, final.clone());
	// }

	memo.set(solution.hash(true) + (t).toString(), final);

	return final;
}

function day_16(filename: string, part: number): boolean
{
	let data: string;
	try {
		data = readFileSync(filename, "utf8");
	} catch (error) {
		console.error(`failure to read file ${filename}`);
		return false;
	}

	const cache = new Map<string, Valve>();
	const lines = data.split("\n");
	for (const line of lines) {
		const valve = Valve.fromLine(line);
		cache.set(valve.name, valve);
	}

	for (const line of lines) {
		const temp = Valve.fromLine(line);
		const valve = cache.get(temp.name);
		if (!valve) {
			console.error(`missing valve ${temp.name}`);
		}
		valve?.parseChildren(line, cache);
	}

	const start = cache.get("AA");
	if (!start)
	{
		console.error("missing start");
		return false;
	}

	// const test: Operation[] = [
	// 	{ type: "move-to", valve: cache.get("AA") || start },
	// 	{ type: "move-to", valve: cache.get("DD") || start },
	// 	{ type: "open", valve: cache.get("DD") || start },
	// 	{ type: "move-to", valve: cache.get("CC") || start },
	// 	{ type: "move-to", valve: cache.get("BB") || start },
	// 	{ type: "open", valve: cache.get("BB") || start },
	// 	{ type: "move-to", valve: cache.get("AA") || start },
	// ]

	const test: Operation[] = [
		{ type: "move-to", valve: cache.get("AA") || start },
		{ type: "move-to", valve: cache.get("DD") || start },
		{ type: "open", valve: cache.get("DD") || start },
		{ type: "move-to", valve: cache.get("CC") || start },
		{ type: "move-to", valve: cache.get("BB") || start },
		{ type: "open", valve: cache.get("BB") || start },
		{ type: "move-to", valve: cache.get("AA") || start },
		{ type: "move-to", valve: cache.get("II") || start },
		{ type: "move-to", valve: cache.get("JJ") || start },
		{ type: "open", valve: cache.get("JJ") || start },
		{ type: "move-to", valve: cache.get("II") || start },
		{ type: "move-to", valve: cache.get("AA") || start },
		{ type: "move-to", valve: cache.get("DD") || start },
		{ type: "move-to", valve: cache.get("EE") || start },
		{ type: "move-to", valve: cache.get("FF") || start },
		{ type: "move-to", valve: cache.get("GG") || start },
		{ type: "move-to", valve: cache.get("HH") || start },
		{ type: "open", valve: cache.get("HH") || start },
		{ type: "move-to", valve: cache.get("GG") || start },

		{ type: "move-to", valve: cache.get("FF") || start },
		{ type: "move-to", valve: cache.get("EE") || start },
		{ type: "open", valve: cache.get("EE") || start },
		{ type: "move-to", valve: cache.get("DD") || start },
		{ type: "move-to", valve: cache.get("CC") || start },
		{ type: "open", valve: cache.get("CC") || start },

	]

	const sol = new Solution(10);

	const goTo = 30;
	for (let i = 0; sol.cnt <= goTo; i++)
	{
		sol.tick();
		if (i < test.length) sol.add(test[i]);
	}
	// sol.tick();

	console.log("test:\n")
	sol.print();


	const i = goTo;
	// for (let i = 1; i < goTo; i++) {
		const memo = new Map<string, Solution>();
		const bestMemo = new Map<string, Solution>();

		const init = new Solution(lines.length);
		init.add({ type: "move-to", valve: start });

		const solution = maxPath(init, 0, i, memo, bestMemo);
		console.log(`solution ${i}:\n`);
		solution.print();
	// }
	// solution.tick();

	// for (const [i, best] of bestMemo)
	// {
	// 	console.log(i, best.cnt, best.peek(goTo));
	// }

	return false;
}

function main()
{
	// life's too short to work out how to use the node-js C ABI,
	// let's just use a system call
	process.exitCode = 1;
	if (process.argv.length < 3)
		console.log(`Usage: ${basename(__filename)} -- give me a filename and a part`);
	else if (day_16(process.argv[2], Number.parseInt(process.argv[3])))
		process.exitCode = 0;
}

main()
