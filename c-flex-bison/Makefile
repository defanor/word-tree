all: example

lexer.h lex.yy.c: lexer.l
	flex $^

parser.h parser.tab.c: parser.y
	bison $^

example: lex.yy.c parser.tab.c parser.h lexer.h tree.h example.c
	cc -g -o $@ lex.yy.c parser.tab.c example.c

test: example
	./example '(foo (bar) baz () ((q  u x)) \(\ \\\) )'
	./example '()'
	./example '(erroneous input'
	./example '(a  (b(c)d (e)))'
	./example 'foo (bar) baz'

clean:
	rm example lexer.h lex.yy.c parser.h parser.tab.c
