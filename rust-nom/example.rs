extern crate nom;
use std::env;
pub use nom::{
    IResult,
    multi::{many0, many1},
    branch::alt,
    sequence::{delimited, preceded},
    character::complete::{one_of, none_of, anychar, char}
};
use Node::{Tree, Literal};

type Forest = Vec<Node>;

#[derive(Debug)]
enum Node {
    Tree(Forest),
    Literal(String),
}

fn parse_literal(input: &str) -> IResult<&str, Node> {
    alt((many1(one_of(" \n")),
         many1(alt((preceded(char('\\'), one_of(" \n()\\")),
                    none_of(" \n()\\"))))))
        (input)
        .map(|(next_input, v)|
             (next_input, Literal(v.into_iter().collect())))
}

fn parse_tree(input: &str) -> IResult<&str, Node> {
    delimited(char('('), parse_forest, char(')'))(input)
        .map(|(ni, f)| (ni, Tree(f)))
}

fn parse_elem(input: &str) -> IResult<&str, Node> {
    alt((parse_tree, parse_literal))(input)
}

fn parse_forest(input: &str) -> IResult<&str, Forest> {
    many0(parse_elem)(input)
}

fn main() {
    match env::args().nth(1) {
        None => println!("Argumetns: <input>"),
        Some(input) => println!("Result: {:?}", parse_forest(input.as_ref()))
    }
}
