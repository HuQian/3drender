out: objloader

objloader: parser.tab.c scanner.yy.c
	gcc -o $@ $^

scanner.yy.c: scanner.l
	flex -o $@ $^

parser.tab.c: parser.y
	bison -o $@ -vdty $^

