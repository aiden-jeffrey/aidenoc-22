use std::fmt::Display;

use std::env;
use std::fs::File;
use std::io::{self, prelude::*, BufReader};

enum Operation {
  MUL,
  ADD,
}

#[derive(Debug, PartialEq, Eq)]
struct Line {

}

fn main() -> io::Result<()> {
  let args: Vec<String> = env::args().collect();
  let file_name = &args[1];

  let file = File::open(file_name)?;
  let reader = BufReader::new(file);
  let lines: Vec<_> = reader.lines().collect::<Result<_, _>>()?;

  for line in lines {
    let line = line.replace(":", "");
    let tokens: Vec<_> = line.split(" ")
      .collect::<Vec<_>>().iter()
      .map(|c| c.parse::<u32>().unwrap()).collect();

    let answer = tokens[0];
    // build operations
    for i in 2..tokens.len() {

    }

  }

  Ok(())
}
