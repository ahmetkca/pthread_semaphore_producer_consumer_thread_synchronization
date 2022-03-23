all:
	gcc -g -pthread -lrt -o question1 question1.c
	gcc -g -pthread -lrt -o question2 question2.c
	gcc -g -pthread -lrt -o question3 question3.c
	gcc -g -pthread -lrt -o question4 question4.c
	gcc -g -pthread -lm -lrt -o question5 question5.c

clean:
	rm -f question1 question2 question3 question4 question5
