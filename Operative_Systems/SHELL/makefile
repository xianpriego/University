
p3: p3.c head_list.h mem_list.h p3Code.h p2Code.h p1Code.h p0Code.h head_list.o mem_list.o process_list.o p3Code.o p2Code.o p1Code.o p0Code.o
	gcc -Wall -o p3 p3.c p0Code.o p1Code.o head_list.o mem_list.o p2Code.o process_list.o p3Code.o

head_list.o: head_list.h head_list.c 
	gcc -c head_list.o head_list.c 

mem_list.o: mem_list.h mem_list.c
	gcc -c mem_list.o mem_list.c

process_list.o: process_list.h process_list.c
	gcc -c process_list.o process_list.c

p3Code.o: p3Code.h p3Code.c
	gcc -c p3Code.o p3Code.c
	
p2Code.o: p2Code.h mem_list.h p2Code.c 
	gcc -c p2Code.o p2Code.c 

p1Code.o: p1Code.h p1Code.c 
	gcc -c p1Code.o p1Code.c

p0Code.o: p0Code.h p1Code.h p2Code.h head_list.h p0Code.c 
	gcc -c p0Code.o p0Code.c

clear:
	rm p3 p1Code.o p0Code.o p2Code.o p3Code.o head_list.o mem_list.o process_list.o
