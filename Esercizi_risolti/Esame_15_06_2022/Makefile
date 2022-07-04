DEST=./
CC=

main:
	if [ -e $(DEST) ]; then \
		cd $(DEST); \
		gcc main.c -std=gnu90 -o coda; \
	else \
		echo "?ERROR" >&2; \
	fi;

cexp:
	gcc prove.c -std=gnu90 -o prove;

exp: cexp
	./prove

.SILENT:
