use std::collections::{HashMap, HashSet};
use std::env;
use std::fs::File;
use std::io::{self, prelude::*, BufReader};

fn main() -> io::Result<()> {
  let args: Vec<String> = env::args().collect();
  let file_name = &args[1];

  let file = File::open(file_name)?;
  let reader = BufReader::new(file);

  let mut rules: HashMap<u32, HashSet<u32>> = HashMap::new();
  let mut updates: Vec<Vec<u32>> = Vec::new();

  let mut parsing_rules = true;
  for line in reader.lines().map_while(Result::ok) {
    if line.is_empty() {
      parsing_rules = false
    } else if parsing_rules {
      if let Some((a, b)) = line.split_once("|") {
        let before = a.parse::<u32>().unwrap();
        let after = b.parse::<u32>().unwrap();

        if let Some(existing) = rules.get_mut(&after) {
          existing.insert(before);
        } else {
          let befores: HashSet<u32> = HashSet::from([before]);
          rules.insert(after, befores);
        }
      }
    }
    else {
      let pages: Vec<u32> = line.split(",")
        .collect::<Vec<_>>()
        .iter()
        .map(|s| s.parse::<u32>().unwrap())
        .collect();

      updates.push(pages);

    }

  }

  part_1(&updates, &rules);
  part_2(&updates, &rules);

  Ok(())
}

fn part_1(updates: &Vec<Vec<u32>>, rules: &HashMap<u32, HashSet<u32>>) {
  let mut count = 0;

  for pages in updates {
    let mut good = true;
    for (i, &head) in pages.iter().enumerate() {
      let tail = &pages[(i + 1)..];

      for before in tail {
        if let Some(after) = rules.get(&head) {
          if after.contains(before) {
            good = false;
            break;
          }
        }
      }

      if !good {
        break;
      }
    }

    if good {
      let m = pages.len() / 2;
      count += pages[m];
    }
  }
  println!("part 1: {}", count);
}

fn part_2(updates: &Vec<Vec<u32>>, rules: &HashMap<u32, HashSet<u32>>) {
  let mut updates: Vec<Vec<u32>> = updates.to_owned();

  let mut count = 0;
  for pages in &mut updates {
    let mut was_bad = false;

    loop {
      let mut swapped = false;
      for hi in 0..pages.len() {
        let head = pages[hi];
        for ti in (hi + 1)..pages.len() {
          let before = pages[ti];
          if let Some(after) = rules.get(&head) {
            if after.contains(&before) {
              pages.swap(hi, ti);
              swapped = true;
              was_bad = true;

              break;
            }
          }
        }

        if swapped {
          break;
        }
      }

      if !swapped {
        break;
      }
    }

    if was_bad {
      let m = pages.len() / 2;
      count += pages[m];
    }
  }

  println!("part 2: {}", count);
}
