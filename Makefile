CC = g++ 
# CFLAGS = -Wall -pedantic 
CFLAGS = -w -g

INCLUDE = -iquote include  
INCLUDE += -iquote utils/logger/include  


bin/projectXYZ: bin/main.o bin/logger.o
	$(CC) $(CFLAGS) bin/main.o bin/logger.o -o $@

bin/main.o: src/main.cpp
	$(CC) -c $(CFLAGS) $(INCLUDE) src/main.cpp -o $@

bin/logger.o: utils/logger/src/logger.c
	$(CC) -c $(CFLAGS) $(INCLUDE) utils/logger/src/logger.c -o $@

.PHONY:run,clean


clean:
	rm -f ./bin/*
run:
	./bin/projectXYZ

mem:
	valgrind --leak-check=full --show-leak-kinds=all ./bin/projectXYZ