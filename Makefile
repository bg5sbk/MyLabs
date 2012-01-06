main: act.h act.c main.c
	cc -pthread -o main act.c main.c
