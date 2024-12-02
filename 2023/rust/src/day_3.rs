use std::cmp;
use std::env;
use std::fs::File;
use std::io::{self, prelude::*, BufReader};

fn in_range(n: usize, min: usize, max: usize) -> bool {
  n > min && n < max
}

#[derive(Debug, PartialEq, Eq)]
struct Elem {
  value: String,
  x:     usize,
  y:     usize,
  width: usize,
  is_sym: bool,
}

impl Elem {
  fn next_to(self: &Self, other: &Self) -> bool {
    let self_min = self.x_min();
    let self_max = self.x_max();
    let other_min = other.x_min();
    let other_max = other.x_max();

    let dx = if in_range(other_max, self_min, self_max) || in_range(other_min, self_min, self_max) {
      0
    } else {
      cmp::min(other_max.abs_diff(self_min), other_min.abs_diff(self_max))
    };

    let dy = self.y.abs_diff(other.y);

    dy <= 1 && dx <= 1
  }

  fn x_min(self: &Self) -> usize {
    self.x
  }

  fn x_max(self: &Self) -> usize {
    self.x + self.width - 1
  }

  fn as_num(self: &Self) -> u32 {
    self.value.parse::<u32>().unwrap()
  }
}

fn main() -> io::Result<()> {

  let args: Vec<String> = env::args().collect();
  let file_name = &args[1];

  let file = File::open(file_name)?;
  let reader = BufReader::new(file);

  let mut elems = Vec::new();

  for (y, line_option) in reader.lines().enumerate() {
    if let Ok(line) = line_option {
      let mut value = String::from("");
      let mut width = 0;

      for (x, ch) in line.chars().enumerate() {
        match ch {
          '.' => {
            if width > 0 {
              let elem = Elem { value, x: x - width, y, width, is_sym: false };
              elems.push(elem);

              value = String::from("");
              width = 0;
            }
          }
          '0'..='9' => {
            value.push(ch);
            width += 1;
          }
          _ => {
            if width > 0 {
              let elem = Elem { value, x: x - width, y, width, is_sym: false };
              elems.push(elem);

              value = String::from("");
              width = 0;
            }

            let elem = Elem { value: String::from(ch), x, y, width: 1, is_sym: true };
            elems.push(elem);
          }
        }
      }
      if width > 0 {
        let elem = Elem { value, x: line.len() - width, y, width, is_sym: false };
        elems.push(elem);
      }
    }
  }

  println!("part_1 sum: {}", part_1(&elems));
  println!("part_2 sum: {}", part_2(&elems));

  Ok(())
}

fn part_1(elems: &Vec<Elem>) -> u32 {
  let mut to_check: Vec<&Elem> = elems.iter().filter(|e| !e.is_sym).collect();

  to_check.retain(|elem| {
    let mut next_to = false;
    for other in elems {
      if !elem.is_sym && !other.is_sym && elem.next_to(&other) {
        next_to = true;
        break;
      }
    }

    next_to
  });

  let mut sum = 0;
  for part_num in to_check {
    sum += part_num.as_num();
  }

  sum
}

fn part_2(elems: &Vec<Elem>) -> u32 {
  let to_check: Vec<&Elem> = elems.iter().filter(|e| e.value == "*").collect();

  let mut sum = 0;

  for elem in to_check {
    let mut next_to = Vec::new();
    for other in elems {
      if !other.is_sym && elem.next_to(&other) {
        next_to.push(other);
      }
    }

    if next_to.len() == 2 {
      let ratio = next_to[0].as_num() * next_to[1].as_num();
      sum += ratio;
    }
  }

  sum
}
