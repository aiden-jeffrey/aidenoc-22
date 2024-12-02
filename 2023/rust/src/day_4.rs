use std::collections::HashMap;
use std::env;
use std::fs::File;
use std::io::{self, prelude::*, BufReader};
use std::str::FromStr;

#[derive(Clone, Debug, PartialEq, Eq)]
struct Card {
  id:      u32,
  winning: Vec<u32>,
  have:    Vec<u32>,
}

#[derive(Debug, PartialEq, Eq)]
pub struct ParseCardError (String);

impl FromStr for Card {
  type Err = ParseCardError;

  fn from_str(s: &str) -> Result<Self, Self::Err> {
    let mut data = s.split(":");

    let mut game_data = data.next().unwrap().split_whitespace();
    let _ = game_data.next();
    let id = game_data.next().unwrap().parse::<u32>().unwrap();

    let lists_data = data.next().unwrap().split_once("|");

    let (winning_data, have_data) = lists_data.unwrap();

    let winning: Vec<u32> = winning_data.trim().split_whitespace().map(|n| n.parse::<u32>().unwrap()).collect();
    let have:    Vec<u32> = have_data.trim().split_whitespace().map(|n| n.parse::<u32>().unwrap()).collect();

    Ok(Card { id, winning, have })
  }
}

impl Card {
  fn score(self: &Self) -> u32 {
    let mut score = 0;

    for has in &self.have {
      if self.winning.contains(&has) {
        if score == 0 {
          score = 1;
        } else {
          score *= 2;
        }
      }
    }

    score
  }

  fn count_winners(self: &Self) -> u32 {
    let mut cnt = 0;

    for has in &self.have {
      if self.winning.contains(&has) {
        cnt += 1;
      }
    }

    cnt
  }
}

fn main() -> io::Result<()> {
    let args: Vec<String> = env::args().collect();
    let file_name = &args[1];

    let file = File::open(file_name)?;
    let reader = BufReader::new(file);

    let mut cards = Vec::new();

    for line_option in reader.lines() {
      if let Ok(line) = line_option {
        let card = Card::from_str(&line).unwrap();

        cards.push(card);
      }
    }

    println!("total 1: {}", part_1(&cards));
    println!("total 2: {}", part_2(&cards));

    Ok(())
}

fn part_1(cards: &Vec<Card>) -> u32 {
  let mut sum = 0;
  for card in cards {
    let score = card.score();
    println!("{} = {}", card.id, score);

    sum += score;
  }

  return sum;
}

fn part_2(cards: &Vec<Card>) -> u32 {
  let mut card_counts: HashMap<u32, u32> = HashMap::new();
  for card in cards {
    card_counts.insert(card.id, 1);
  }

  for card in cards {
    let cnt = card_counts.get(&card.id).unwrap();

    for _ in 0..*cnt {
      let score = card.count_winners();
      let start = card.id + 1;
      let end   = card.id + 1 + score;

      for other_id in start..end {
        let other_cnt = card_counts.get_mut(&other_id).unwrap();

        *other_cnt += 1;
      }
    }
  }

  let mut sum = 0;
  for card in cards {
    let cnt = *card_counts.get(&card.id).unwrap();

    println!("{}: {}", card.id, cnt);

    sum += cnt;
  }

  sum
}
