#include "semantica.h"

#include <string.h>
#include <stdlib.h>

#define MAX_VARIABLES 500
#define MAX_AMBITOS   100
#define MAX_ERRORES   20

typedef struct {
    char nombre[256];
    int tiene_valor;
} Variable;

/*
 * Pila de ambitos.
 * Todas las variables visibles van en un solo arreglo (la "tabla").
 * Cada ambito recuerda en que posicion de la tabla empezo (marcas[]).
 * Al cerrar un ambito basta con regresar `cantidad` a esa marca, y las
 * variables de ese bloque dejan de existir.
 */
typedef struct {
    Variable variables[MAX_VARIABLES];
    int cantidad;               /* cuantas variables hay en la tabla */

    int marcas[MAX_AMBITOS];    /* marcas[i] = inicio del ambito i en la tabla */
    int profundidad;            /* 0 = ambito global */
    int exceso;                 /* bloques anidados de mas (si pasan de MAX_AMBITOS) */

    unsigned errores;
    const char *ruta;
    FILE *diagnosticos;
} Contexto;

/* entra a un ambito nuevo (se llama al abrir un bloque) */
static void push_scope(Contexto *c)
{
    if (c->profundidad + 1 >= MAX_AMBITOS) {
        c->exceso++;            /* demasiado anidado: no abre ambito nuevo */
        return;
    }
    c->profundidad++;
    c->marcas[c->profundidad] = c->cantidad;
}

/* sale del ambito actual y destruye sus variables (se llama al cerrar un bloque) */
static void pop_scope(Contexto *c)
{
    if (c->exceso > 0) {
        c->exceso--;
        return;
    }
    c->cantidad = c->marcas[c->profundidad];
    c->profundidad--;
}

/* busca en TODOS los ambitos visibles, de adentro hacia afuera; -1 si no existe */
static int buscar(const Contexto *c, const char *nombre)
{
    for (int i = c->cantidad - 1; i >= 0; i--) {
        if (strcmp(c->variables[i].nombre, nombre) == 0) {
            return i;
        }
    }
    return -1;
}

/* busca SOLO en el ambito actual (para detectar E02); -1 si no esta */
static int buscar_en_ambito_actual(const Contexto *c, const char *nombre)
{
    for (int i = c->cantidad - 1; i >= c->marcas[c->profundidad]; i--) {
        if (strcmp(c->variables[i].nombre, nombre) == 0) {
            return i;
        }
    }
    return -1;
}

/* imprime:  archivo:linea:columna: error CODIGO: prefijo 'nombre'sufijo */
static void reportar(Contexto *c, const Token *t, const char *codigo,
                     const char *prefijo, const char *sufijo)
{
    if (c->errores < MAX_ERRORES) {
        fprintf(c->diagnosticos, "%s:%d:%d: error %s: %s '%s'%s\n",
                c->ruta, t->line, t->col, codigo, prefijo, t->lexeme, sufijo);
    }
    c->errores++;
}

/* revisa una expresion: cada variable que se lee debe existir y tener valor */
static void revisar_expresion(Contexto *c, const AstNodo *nodo)
{
    if (nodo == NULL) {
        return;
    }

    /* E04: division entre una constante 0, detectada en compilacion */
    if (nodo->tipo == AST_BINARIA && nodo->token.type == TOKEN_DIV) {
        const AstNodo *der = nodo->hijo != NULL ? nodo->hijo->siguiente : NULL;
        if (der != NULL && der->tipo == AST_NUMERO &&
            strtol(der->token.lexeme, NULL, 10) == 0) {
            reportar(c, &nodo->token, "E04", "división entre cero constante", "");
        }
    }

    if (nodo->tipo == AST_VARIABLE) {
        int i = buscar(c, nodo->token.lexeme);
        if (i < 0) {
            reportar(c, &nodo->token, "E01", "variable no declarada", "");
        } else if (!c->variables[i].tiene_valor) {
            reportar(c, &nodo->token, "E01", "variable sin valor asignado", "");
        }
        return;    }

    for (const AstNodo *h = nodo->hijo; h != NULL; h = h->siguiente) {
        revisar_expresion(c, h);
    }
}

static void revisar_sentencia(Contexto *c, const AstNodo *nodo)
{
    switch (nodo->tipo) {
    case AST_DECLARACION:
        /* primero la expresion inicial (puede leer una variable de afuera) */
        revisar_expresion(c, nodo->hijo);

        if (buscar_en_ambito_actual(c, nodo->token.lexeme) >= 0) {
            /* ya existe en este mismo ambito: E02 y no se vuelve a agregar */
            reportar(c, &nodo->token, "E02", "redeclaración de",
                     " en el mismo ámbito");
        } else if (c->cantidad < MAX_VARIABLES) {
            Variable *v = &c->variables[c->cantidad++];
            strcpy(v->nombre, nodo->token.lexeme);
            v->tiene_valor = (nodo->hijo != NULL);
        }
        break;

    case AST_ASIGNACION: {
        revisar_expresion(c, nodo->hijo);
        int i = buscar(c, nodo->token.lexeme);
        if (i < 0) {
            reportar(c, &nodo->token, "E01", "variable no declarada", "");
        } else {
            c->variables[i].tiene_valor = 1;
        }
        break;
    }

    case AST_IF:
    case AST_WHILE:
        /* primer hijo = condicion, los demas son bloques */
        revisar_expresion(c, nodo->hijo);
        for (const AstNodo *h = nodo->hijo->siguiente; h != NULL; h = h->siguiente) {
            revisar_sentencia(c, h);
        }
        break;

    case AST_PRINT:
        revisar_expresion(c, nodo->hijo);
        break;

    case AST_BLOQUE:
        /* cada bloque { } abre un ambito nuevo y lo cierra al terminar */
        push_scope(c);
        for (const AstNodo *h = nodo->hijo; h != NULL; h = h->siguiente) {
            revisar_sentencia(c, h);
        }
        pop_scope(c);
        break;

    case AST_PROGRAMA:
        /* el programa es el ambito global (profundidad 0), no abre otro */
        for (const AstNodo *h = nodo->hijo; h != NULL; h = h->siguiente) {
            revisar_sentencia(c, h);
        }
        break;

    default:
        break;
    }
}

unsigned semantica_revisar(const AstNodo *raiz, const char *ruta,
                           FILE *diagnosticos)
{
    static Contexto c;  /* static: es grande y asi no va en la pila */
    memset(&c, 0, sizeof c);
    c.ruta = ruta;
    c.diagnosticos = diagnosticos;

    if (raiz != NULL) {
        revisar_sentencia(&c, raiz);
    }
    return c.errores;
}