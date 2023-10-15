all: lab2-signal lab2-sigact1 lab2-sigact2 lab2-pipefull lab2-pipeerr lab2-pipesig lab2-mask

lab2-signal: lab2-signal.c
	gcc $^ -o $@ -Wall

lab2-sigact1: lab2-sigact1.c
	gcc $^ -o $@ -Wall

lab2-sigact2: lab2-sigact2.c
	gcc $^ -o $@ -Wall

lab2-pipefull: lab2-pipefull.c
	gcc $^ -o $@ -Wall

lab2-pipeerr: lab2-pipeerr.c
	gcc $^ -o $@ -Wall

lab2-pipesig: lab2-pipesig.c
	gcc $^ -o $@ -Wall

lab2-mask: lab2-mask.c
	gcc $^ -o $@ -Wall
  
clean:
	rm -f lab2-signal lab2-sigact1 lab2-sigact2 lab2-pipefull lab2-pipeerr lab2-pipesig lab2-mask
