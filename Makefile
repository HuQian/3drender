out: objloader

objloader: main.cpp parser.tab.cpp scanner.yy.cpp
	g++ -o $@ $^

scanner.yy.cpp: scanner.l
	flex -v -o $@ $^

parser.tab.cpp: parser.y
	bison -o $@ $^ -d 

run:
	./objloader samples/ilegal_input_2.c

clean:
	-rm parser.tab*
	-rm parser.output
	-rm scanner.yy*
	-rm objloader