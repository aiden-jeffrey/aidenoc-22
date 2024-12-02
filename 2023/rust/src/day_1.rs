use std::collections::HashMap;
use std::env;
use std::fs::File;
use std::io::{self, prelude::*, BufReader};

fn main() -> io::Result<()> {
    let args: Vec<String> = env::args().collect();
    let file_name = &args[1];

    let file = File::open(file_name)?;
    let reader = BufReader::new(file);

    let mut total_1 = 0;
    let mut total_2 = 0;
    for line_option in reader.lines() {
        if let Ok(line) = line_option {

            let score_1 = eval_line_1(&line);
            let score_2 = eval_line_2(&line);

            total_1 += score_1;
            total_2 += score_2;
        }
    }

    println!("total_1: {}", total_1);
    println!("total_2: {}", total_2);

    Ok(())
}

fn eval_line_1(line: &String) -> u32 {
    let nums: Vec<u32> = line.chars()
        .filter(|c| c.is_digit(10))
        .filter_map(|c| c.to_digit(10))
        .collect();

    let len = nums.len();

    if len > 0 { nums[0] * 10 + nums[len - 1] } else { 0 }
}

fn eval_line_2(line: &String) -> u32 {
    let dict = HashMap::from([
        ("one", 1),
        ("two", 2),
        ("three", 3),
        ("four", 4),
        ("five", 5),
        ("six", 6),
        ("seven", 7),
        ("eight", 8),
        ("nine", 9),
        ("0", 0),
        ("1", 1),
        ("2", 2),
        ("3", 3),
        ("4", 4),
        ("5", 5),
        ("6", 6),
        ("7", 7),
        ("8", 8),
        ("9", 9),
    ]);

    let mut early: (Option<usize>, Option<&str>, Option<u32>) = (None, None, None);
    let mut late: (Option<usize>, Option<&str>, Option<u32>)  = (None, None, None);

    for word in dict.keys() {
        if let Some(index) = line.find(word) {
            if early.0.is_none() || index < early.0.unwrap() {
                early.0 = Some(index);
                early.1 = Some(word);
                early.2 = Some(dict[word]);
            }
        }

        if let Some(index) = line.rfind(word) {
            if late.0.is_none() || index > late.0.unwrap() {
                late.0 = Some(index);
                late.1 = Some(word);
                late.2 = Some(dict[word]);
            }
        }
    }

    if let Some(first) = early.2 {
        if let Some(last) = late.2 {
            return first * 10 + last;
        }
    }

    panic!("shouldn't get here...");
}

