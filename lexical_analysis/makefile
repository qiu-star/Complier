lextest: lexical_analysis.o lex.yy.o errormsg.o util.o
	cc -g -o lextest lexical_analysis.o lex.yy.o errormsg.o util.o

lexical_analysis.o: lexical_analysis.c tokens.h errormsg.h util.h
	cc -g -c lexical_analysis.c

errormsg.o: errormsg.c errormsg.h util.h
	cc -g -c errormsg.c

lex.yy.o: lex.yy.c tokens.h errormsg.h util.h
	cc -g -c lex.yy.c

lex.yy.c: c.lex
	lex c.lex

util.o: util.c util.h
	cc -g -c util.c

clean: 
	rm -f a.out util.o lexical_analysis.o lex.yy.o lex.yy.c errormsg.o lextest
