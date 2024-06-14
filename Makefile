out: objloader

objloader: parser.tab.c scanner.yy.c
	gcc -o $@ $^

scanner.yy.c: scanner.l
	flex -v -o $@ $^

parser.tab.c: parser.y
	bison -o $@ $^ -d 

run:
	cat ./samples/ilegal_input_2.c | ./objloader