use std::slice::Iter;

use std::env;
use std::fs::File;
use std::io::{self, prelude::*, BufReader};

enum Operation {
  Mul = 0,
  Add = 1,
  Cnt = 2,
}

// impl Operation {
//   pub fn all() -> Iter<'static, Operation> {
//       static OPERS: [Operation; 2] = [Operation::Mul, Operation::Add];
//       OPERS.iter()
//   }
// }


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
    let values: Vec<_> = line.split(" ")
      .collect::<Vec<_>>().iter()
      .map(|c| c.parse::<u32>().unwrap()).collect();

    let mut operations: Vec<Operation> = Vec::new();
    let answer = values[0];
    // build operations list
    let oper_cnt = Operation::Cnt as u32;
    let num_opers = (values.len() - 1) as u32;
    let max = u32::pow(oper_cnt, num_opers);

    println!("{} - {}", max, line);

    for n in 0..max {
      let mut n = n;
      for _ in 0..num_opers {
        n /= oper_cnt;
        print!("{}", n % oper_cnt);
      }
      println!();
    }

  }

  Ok(())
}
