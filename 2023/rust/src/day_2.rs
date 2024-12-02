use std::collections::HashMap;
use std::env;
use std::fs::File;
use std::io::{self, prelude::*, BufReader};
use std::str::FromStr;

#[derive(Debug, PartialEq, Eq)]
struct Contents {
  blue:  u32,
  red:   u32,
  green: u32,
}

struct Game {
  id:           u32,
  turns:        Vec<Contents>,
  max_contents: Contents,
}

#[derive(Debug, PartialEq, Eq)]
pub struct ParseGameError (String);

impl FromStr for Game {
  type Err = ParseGameError;

  fn from_str(s: &str) -> Result<Self, Self::Err> {
    let mut data = s.split(":");

    let mut game_data = data.next().unwrap().split(" ");
    let _ = game_data.next();
    let id = game_data.next().unwrap().parse::<u32>().unwrap();

    let mut max_contents = Contents { red: 0, green: 0, blue: 0 };

    let mut turns = Vec::new();
    let turns_data = data.next().unwrap().split(";");
    for turn in turns_data {
      let mut red   = 0;
      let mut green = 0;
      let mut blue  = 0;

      let grabs = turn.split(",");

      for grab in grabs {
        let count_data = grab.trim().split_once(" ");
        if let Some((count, kind)) = count_data {
          let count = count.parse::<u32>().unwrap();

          match kind {
            "red" => {
              red += count;
            }
            "green" => {
              green += count;
            }
            "blue" => {
              blue += count;
            }
            _ => {
              panic!("unknown cube type {}", kind);
            }
          }
        }
      }

      turns.push(Contents {
        red,
        green,
        blue,
      });

      if red > max_contents.red {
        max_contents.red = red;
      }

      if blue > max_contents.blue {
        max_contents.blue = blue;
      }

      if green > max_contents.green {
        max_contents.green = green;
      }
    }

    let game = Game {
      id,
      turns,
      max_contents
    };

    Ok(game)
  }
}

impl Game {
  fn is_possible(self: &Self, contents: &Contents) -> bool {
    self.max_contents.red <= contents.red &&
      self.max_contents.blue <= contents.blue &&
      self.max_contents.green <= contents.green
  }

  fn power(self: &Self) -> u32 {
    self.max_contents.red * self.max_contents.blue * self.max_contents.green
  }
}

fn main() -> io::Result<()> {
    let args: Vec<String> = env::args().collect();
    let file_name = &args[1];

    let file = File::open(file_name)?;
    let reader = BufReader::new(file);

    let contents = Contents { red: 12, green: 13, blue: 14 };

    let mut possible_sum = 0;
    let mut power_sum = 0;

    let mut games = HashMap::new();

    for line_option in reader.lines() {
      if let Ok(line) = line_option {
        let game = Game::from_str(&line);
        if let Ok(game) = game {
          println!("game: {}", game.id);
          for turn in &game.turns {
            println!("    turn: {:?}", turn);
          }

          println!("max turn: {:?}", game.max_contents);
          println!("power: {:?}", game.power());

          power_sum += game.power();

          if game.is_possible(&contents) {
            println!("{} is possible", game.id);
            possible_sum += game.id;
          }

          games.insert(game.id, game);
        }
      }
    }

    println!("\n possible_sum = {}", possible_sum);
    println!("\n power_sum = {}", power_sum);

    Ok(())
}
