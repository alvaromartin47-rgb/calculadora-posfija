# CREADO POR ALVARO MARTIN EN PYTHON :)

CFLAGS = -g -std=c99 -Wall -Werror -Wno-conversion -Wtype-limits -pedantic
VFLAGS = --leak-check=full --track-origins=yes --show-reachable=yes

# OBJETOS A BORRAR
REMOVE = dc.o mod/pila/pila.o mod/strutil/strutil.o


# COMPILACION DE OBJETOS

dc.o: dc.c
	@- $(CC) -c $(CFLAGS) $^ -o $@

mod/pila/pila.o: mod/pila/pila.c
	@- $(CC) -c $(CFLAGS) $^ -o $@

mod/strutil/strutil.o: mod/strutil/strutil.c
	@- $(CC) -c $(CFLAGS) $^ -o $@


# COMPILACION DE EJECUTABLES

dc: dc.o mod/pila/pila.o mod/strutil/strutil.o
	@- $(CC) $(CFLAGS) $^ -o $@ -lm


# LIMPIAR

clean:
	@- rm -r $(REMOVE)
