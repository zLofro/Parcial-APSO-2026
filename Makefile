Padre:
	cc -o Padre Padre.c
Hijo1:
	cc -o Hijo1 Hijo1.c
Hijo2:
	cc -o Hijo2 Hijo2.c
Hijo3:
	cc -o Hijo3 Hijo3.c
all:	Padre Hijo1 Hijo2 Hijo3
clean:
	rm Padre Hijo1 Hijo2 Hijo3