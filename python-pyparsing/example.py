#!/usr/bin/env python3
import pyparsing as pp

pTree = pp.Forward()
pForest = pTree[...].leave_whitespace()
pTree <<= (pp.Group(("(" + pForest + ")").\
                    set_parse_action(lambda s, l, t: t[1:-1]))
           | pp.Word(" \n")
           | pp.Combine(
               pp.OneOrMore(
                   pp.CharsNotIn(" \n()\\")
                   | ("\\" + pp.Char(" \n()\\")).\
                   set_parse_action(lambda s, l, t: t[1:]))))\
    .leave_whitespace()

print(pForest.parse_string(input()))
