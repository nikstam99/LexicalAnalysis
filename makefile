OBJS	= c_src/lexan.o c_src/list.o c_src/general.o
OBJS2	= c_src/builders.o c_src/list.o c_src/general.o 
OBJS3 	= c_src/splitters.o c_src/list.o c_src/general.o 
SOURCE	= c_src/lexan.c c_src/list.c c_src/general.c 
HEADER	= include/list.h include/general.h
CC		= gcc
FLAGS	= -g -c -I include 

all: $(OBJS) $(OBJS2) $(OBJS3)
	$(CC) -g $(OBJS2) -o builders
	$(CC) -g $(OBJS3) -o splitters
	$(CC) -g $(OBJS) -o lexan

lexan.o: c_src/lexan.c
	$(CC) $(FLAGS) c_src/lexan.c

list.o: c_src/list.c
	$(CC) $(FLAGS) c_src/list.c

general.o: c_src/general.c
	$(CC) $(FLAGS) c_src/general.c

builders.o: c_src/builders.c
	$(CC) $(FLAGS) c_src/builders.c	

splitters.o: c_src/splitters.c
	$(CC) $(FLAGS) c_src/splitters.c	

valgrind: $(OBJS) $(OBJS2) $(OBJS3)
	$(CC) -g $(OBJS2) -o builders
	$(CC) -g $(OBJS3) -o splitters
	$(CC) -g $(OBJS) -o lexan
	valgrind ./lexan -i io_files/TextFile.txt -l 5 -m 5 -t 20 -e io_files/ExclusionList.txt -o io_files/Outputfile.txt

clean:
	rm -f $(OBJS) lexan
	rm -f $(OBJS2) builders
	rm -f $(OBJS3) splitters

run: 
	./lexan -i io_files/TextFile.txt -l 5 -m 5 -t 20 -e io_files/ExclusionList.txt -o io_files/Outputfile.txt

