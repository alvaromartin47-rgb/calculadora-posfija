/* *****************************************************************
 *                                                                 *
 *                              DC                                 *
 *                                                                 *
 *                                                                 *
 * *****************************************************************/

/*------------------------------------------------------------------*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "mod/pila/pila.h"
#include "mod/strutil/strutil.h"

/*------------------------------------------------------------------*/

/* *****************************************************************
 *                      FUNCIONES DE PROGRAMA
 * *****************************************************************/

/*
Recibe un numero de tipo long y lo aloja en el heap devolviendo un puntero
a long. La liberacion de memoria queda a cargo del usuario de la función.
Post: _num_ vive en el heap.
*/
long *long_crear(long num) {
    long *n_num = malloc(sizeof(long));
    if (!n_num) return NULL;
    *n_num = num;
    return n_num;
}

/*
Recibe una pila y le aplica la raiz cuadrada al primer elemento (tope),
devuelve true si se puedo apilar correctamente y false si la pila está
vacia o el tope es menor a cero.
Pre: la pila fue creada.
Post: si devolvió true el tope inicial no existe y el nuevo tope es su
raiz cuadrada.
*/
bool aplicar_raiz(pila_t *pila) {
    long *e1 = pila_desapilar(pila);

    if (!e1 || *e1 < 0) {
        free(e1);
        return false;
    }

    long *res = long_crear(sqrt(*e1));
    free(e1);
    return pila_apilar(pila, res);
}

/*
Recibe una pila, le aplica el operador ternario a los primeros 3 elementos,
y apila el resultado.
Devuelve true si se puedo apilar correctamente, false si no se pudieron desapilar
los tres elementos u ocurrio un error al apilar.
Pre: la pila fue creada.
Post: si devolvió true el tope inicial ya no existe y el nuevo tope es el resultado
la opcion correcta de acuerdo a la condicion.
*/
bool aplicar_ternario(pila_t *pila) {
    long *e3 = pila_desapilar(pila);
	long *e2 = pila_desapilar(pila);
	long *e1 = pila_desapilar(pila);
    
    if (!e1 || !e2 || !e3){
        if (e1) free(e1);
        if (e2) free(e2);
        if (e3) free(e3);
        return false;
    }

	if (*e1 == 0) {
		long *falso = e3;
		e3 = e2;
		e2 = falso;
	}

	free(e1);
	free(e3);

	if (pila_apilar(pila, e2)) return true;
	free(e2);
    return false;
}

/*
Recibe una pila y un puntero a un string.
Compara el string recibido con los strings correspondientes a las
operaciones posibles a realizar, apila el resultado y devuelve true.
Si ocurre que la pila se encuentra vacia o el string no corresponde a
ninguna operación devuelve false.
Pre: la pila fue creada.
Post: devolvio el booleano correspondiente.
*/
bool operar_bi(pila_t *pila, char *tok) {
    long *e1 = pila_desapilar(pila);
    long *e2 = pila_desapilar(pila);
    
    if (!e1 || !e2) {
        if (e1) free(e1);
        if (e2) free(e2);
        return false;
    }
    long *calc = NULL;
    
    if (strcmp(tok, "+") == 0) calc = long_crear(*e2 + *e1);
    else if (strcmp(tok, "-") == 0) calc = long_crear(*e2 - *e1);
    else if (strcmp(tok, "*") == 0) calc = long_crear(*e2 * *e1);
    else if (strcmp(tok, "/") == 0 && *e1 != 0) calc = long_crear(*e2 / *e1);
    else if (strcmp(tok, "^")  == 0 && *e1 >= 0) calc = long_crear(pow(*e2, *e1));
    else if (strcmp(tok, "log") == 0 && *e2 > 0 && *e1 >= 2) calc = long_crear(log10(*e2) / log10(*e1));
    
    free(e1);
    free(e2);

    if (calc && pila_apilar(pila, calc)) return true;
    free(calc);
    return false;
}

/*
Recibe una pila y un vector de strings. Recorre cada string de _tokens_,
si es un operando lo apila, si es un operador se lo aplica a los elementos
de la pila, apila el resultado, y devuelve true. De lo contrario, si no es
un string válido u ocurrio algun error inesperado devuelve false. 
Pre: la pila fue creada.
Post: si devolvió true se apilo un nuevo resultado.
*/
bool dc(pila_t *pila, char **tokens) {

    for (size_t i=0; tokens[i]; i++) {
        if (strcmp(tokens[i], "") == 0) continue;

        // Si es un operando
        char *resto = NULL;
        long int num = strtol(tokens[i], &resto, 10);
        
        if (num != 0 || strcmp(tokens[i], "0") == 0) {
            if (*resto == '\0') {
                long *n_num = long_crear(num);
                if (!n_num || !pila_apilar(pila, n_num)) return false;
            }
            else if (tokens[i] != resto) return false;
        }

        // De lo contrario es un operador

        // Si es operacion unaria
        else if (strcmp(tokens[i], "sqrt") == 0) {
            if (!aplicar_raiz(pila)) return false;
        }

        // Si es operacion ternaria
        else if (strcmp(tokens[i], "?") == 0) {
            if (!aplicar_ternario(pila)) return false;
        }

        // Si es operacion binaria
        else {
            if (!operar_bi(pila, tokens[i])) return false;
        }
    }       

    return true;
}

/*
Recibe una pila vacia y lee la entrada standard, por cada linea del
archivo stdin realiza una operación que puede devolver true o false,
si devolvió true la opración se realizo correctamete y muestra el resultado
por salida standard, de lo contrario muestra un mensaje de error por salida
standard de errores.
Pre: la pila fue creada y esta vacia.
Post: se mostró un mensaje por alguna salida standard (el que corresponda).
*/
void procesar_entrada(pila_t *pila) {
    
    size_t tam_buffer = 1;
    char *buff_linea = malloc(sizeof(char) * tam_buffer);

    while (getline(&buff_linea, &tam_buffer, stdin) != EOF) {
        size_t largo = strlen(buff_linea);

        if (strcmp(&buff_linea[largo-1], "\n") == 0) buff_linea[largo-1] = '\0';

        char **tokens = split(buff_linea, ' ');

        bool es_valido = dc(pila, tokens);
        
        // Solo debe haber un elemento, el resultado.
        long *res = pila_desapilar(pila);

        if (!es_valido || !res || !pila_esta_vacia(pila)) {
            if (res) free(res);
            printf("ERROR\n");
            while (!pila_esta_vacia(pila)) free(pila_desapilar(pila));
        }
        
        else {
            printf("%ld\n", *res);
            free(res);
        }
        
        free_strv(tokens);
    }
    
    free(buff_linea);
}

/*------------------------------------------------------------------*/

/* *****************************************************************
 *                              MAIN
 * *****************************************************************/

int main() {
    
    pila_t *pila = pila_crear();
    if (!pila) return 1;
    
    procesar_entrada(pila);
    
    pila_destruir(pila);

    return 0;
}

                        /*FIN DE PROGRAMA*/                         
/*------------------------------------------------------------------*/