all:
	gcc -lpthread -lrt -o question1 question1.c
	gcc -lpthread -lrt -o question2 question2.c
	clang -pthread -o question3 question3.c
	clang -pthread -lrt -o question4 question4.c

clean:
	rm -f question1 question2 question3 question4