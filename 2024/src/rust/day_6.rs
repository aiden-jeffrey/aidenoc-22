use std::fmt::Display;

use std::env;
use std::fs::File;
use std::io::{self, prelude::*, BufReader};

#[derive(Debug, PartialEq, Eq)]
struct Map {
  map:     Vec<Vec<char>>,
  guard:   Option<(i32, i32, char)>,
  visited: u32,
  in_loop: bool,
  width:   i32,
  height:  i32,
}

impl Display for Map {
  fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
    let mut str: String = String::new();

    for y in 0..self.map.len() {
      let line = &self.map[y];
      for x in 0..line.len() {
        let ch = {
          if let Some(guard) = self.guard {
            if (x as i32) == guard.0 && (y as i32) == guard.1 {
              guard.2
            } else {
              line[x]
            }
          } else {
            line[x]
          }
        };
        str.push(ch);
      }
      str.push('\n');
    }
    write!(f, "{}", str)
  }
}

fn look_up(dir: char) -> Result<(i32, i32, char, char), ()> {
  match dir {
    '^' => { Ok(( 0, -1, '>', '|')) },
    '>' => { Ok(( 1,  0, 'v', '-')) },
    'v' => { Ok(( 0,  1, '<', '|')) },
    '<' => { Ok((-1,  0, '^', '-')) },
    _ => Err(())
  }
}

impl Map {
  fn new(lines: Vec<String>) -> Self {
    let width = lines[0].len();
    let height = lines.len();

    let mut map = vec![vec!['\0'; width]; height];
    let mut guard: Option<(i32, i32, char)> = None;

    for (y, line) in lines.into_iter().enumerate() {

      for (x, letter) in line.chars().enumerate() {
        if letter == '^' {
          guard = Some((x as i32, y as i32, letter));
          map[y][x] = '.';
        } else {
          map[y][x] = letter;
        }
      }
    }

    Self {
      map,
      guard,
      visited: 0,
      in_loop: false,
      height: height.try_into().unwrap(),
      width: width.try_into().unwrap(),
    }
  }

  fn tick(&mut self) -> Result<bool, ()> {
    let mut turned = false;
    loop {
      if let Some(guard) = self.guard {
        let (dx, dy, turn, path) = look_up(guard.2)?;
        let x = guard.0 + dx;
        let y = guard.1 + dy;
        if x < 0 || x >= self.width || y < 0 || y >= self.height {
          // left board
          return Ok(true);
        }

        let ch = self.map[y as usize][x as usize];
        if "#O^>v<".contains(ch) {
          // wall
          self.guard = Some((guard.0, guard.1, turn));
          if ch == guard.2 {
            self.in_loop = true;
          }

          self.map[y as usize][x as usize] = guard.2;
          turned = true;
        } else {
          if self.map[guard.1 as usize][guard.0 as usize] == '.' {
            self.map[guard.1 as usize][guard.0 as usize] = if turned { '+' } else { path };
            self.visited += 1;
          }

          self.guard = Some((x, y, guard.2));
          break;
        }
      }
    }
    Ok(false)
  }

  fn add_obstacle(&mut self, other: &Self) {
    if let Some(guard) = other.guard {
      self.map[guard.1 as usize][guard.0 as usize] = 'O';
    }
  }

  fn mark(&mut self, ch: char) -> bool {
    if let Some(guard) = self.guard {
      if self.map[guard.1 as usize][guard.0 as usize] == ch {
        return false;
      } else {
        self.map[guard.1 as usize][guard.0 as usize] = ch;
        return true;
      }
    }
    false
  }
}

fn main() -> io::Result<()> {
  let args: Vec<String> = env::args().collect();
  let file_name = &args[1];

  let file = File::open(file_name)?;
  let reader = BufReader::new(file);
  let lines: Vec<_> = reader.lines().collect::<Result<_, _>>()?;

  part_1(&lines);
  part_2(&lines);

  Ok(())
}

fn part_1(lines: &[String]) {
  let mut map = Map::new(lines.to_vec());

  println!("{}", map);

  while !map.tick().unwrap() {
    // thread::sleep(dt);
  }
  println!("{}", map);

  println!("part 1 - visited: {}", map.visited + 1);
}

fn part_2(lines: &[String]) {
  let mut base = Map::new(lines.to_vec());

  let mut num_loops = 0;
  base.tick().unwrap();
  while !base.tick().unwrap() {
    // put obstacle at current position and run sim to check for loop
    let mut sim = Map::new(lines.to_vec());
    sim.add_obstacle(&base);

    // println!("{}", sim);
    let mut in_loop = false;
    while !sim.tick().unwrap() {
      if sim.in_loop {
        in_loop = true;

        break;
      }
    }

    if in_loop && base.mark('@') {
      num_loops += 1;
    }
  }

  println!("part 2 - num loops: {}", num_loops);
}
