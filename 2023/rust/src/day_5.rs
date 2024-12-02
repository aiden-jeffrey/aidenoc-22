use std::collections::HashMap;
use std::env;
use std::fs::File;
use std::io::{self, prelude::*, BufReader};

#[derive(Clone, Debug, PartialEq, Eq)]
struct ThingMap {
  source: String,
  dest:   String,
  map:    HashMap<u32, u32>,
}

impl ThingMap {
  fn new(source: String, dest: String) -> Self {
    ThingMap{
      source,
      dest,
      map: HashMap::new(),
    }
  }

  fn add_line(self: &mut Self, line: &String) {
    let mut tokens = line.trim().split_whitespace();

    let dest_start   = tokens.next().unwrap().parse::<u32>().unwrap();
    let source_start = tokens.next().unwrap().parse::<u32>().unwrap();
    let range_len    = tokens.next().unwrap().parse::<u32>().unwrap();

    for i in 0..range_len {
      self.map.insert(source_start + i, dest_start + i);
    }
  }

  fn get(self: &Self, query: u32) -> u32 {
    if let Some(value) = self.map.get(&query) {
      *value
    } else {
      query
    }
  }
}

fn main() -> io::Result<()> {
    let args: Vec<String> = env::args().collect();
    let file_name = &args[1];

    let file = File::open(file_name)?;
    let reader = BufReader::new(file);

    let mut seeds = Vec::new();

    let mut maps: HashMap<(String, String), ThingMap> = HashMap::new();
    let mut current: Option<Box<ThingMap>> = None;

    for (i, line_option) in reader.lines().enumerate() {
      if let Ok(line) = line_option {
        if i == 0 {
          let seed_data: Vec<u32> = line.split_once(":").unwrap().1.trim()
            .split_whitespace()
            .map(|n| n.parse::<u32>()
            .unwrap())
            .collect();

          seeds = [seeds, seed_data].concat();
        } else if line == "" {
          current = None;
        } else {
          match current {
            Some(ref mut current_thing) => {
              current_thing.add_line(&line);
            }
            None => {
              let (source, dest) = line.split_once(" ").unwrap().0.split_once("-to-").unwrap();

              let src_string = source.to_string();
              let dest_string = dest.to_string();

              let thing = ThingMap::new(source.to_string().clone(), dest.to_string().clone());
              current = Some(Box::new(thing));

              let clone = thing.clone();
              maps.insert((src_string, dest_string), clone);
            }
          }
        }
      }
    }

    // if let Some(seed_to_soil) = maps.get(("seed", "soil")) {
    //   for seed in seeds {
    //     println!("{} -> {}", seed, seed_to_soil.get(seed));
    //   }
    // }

    Ok(())
}
