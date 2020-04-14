lextest: grammar.o lexical_analysis.o symbol.o midcode.o lex.yy.o util.o
	cc -g -o grammartest grammar.o lexical_analysis.o symbol.o midcode.o lex.yy.o util.o

symbol.o: symbol.c symbol.h util.h
	cc -g -c symbol.c

midcode.o: midcode.c midcode.h util.h
	cc -g -c midcode.c

lexical_analysis.o: lexical_analysis.c tokens.h util.h
	cc -g -c lexical_analysis.c

lex.yy.o: lex.yy.c tokens.h util.h
	cc -g -c lex.yy.c

lex.yy.c: c.lex
	lex c.lex

util.o: util.c util.h
	cc -g -c util.c

grammar.o: grammar.c grammar.h tokens.h util.h
	cc -g -c grammar.c

clean: 
	rm -f a.out util.o lexical_analysis.o lex.yy.o lex.yy.c grammar.o grammartest symbol.o midcode.o
