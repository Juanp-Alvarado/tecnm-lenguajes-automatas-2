#include "semantica.h"

#include <string.h>

#define MAX_VARIABLES 500
#define MAX_ERRORES 20

typedef struct {
    char nombre[256];
    int tiene_valor;
} Variable;

typedef struct {
    Variable variables[MAX_VARIABLES];
    int cantidad;
    unsigned errores;
    const char *ruta;
    FILE *diagnosticos;
} Contexto;

/* busca desde la ultima declarada hacia atras; -1 si no existe */
static int buscar(const Contexto *c, const char *nombre)
{
    for (int i = c->cantidad - 1; i >= 0; i--) {
        if (strcmp(c->variables[i].nombre, nombre) == 0) {
            return i;
        }
    }
    return -1;
}

static void reportar(Contexto *c, const Token *t, const char *texto)
{
    if (c->errores < MAX_ERRORES) {
        fprintf(c->diagnosticos, "%s:%d:%d: error E01: %s '%s'\n",
                c->ruta, t->line, t->col, texto, t->lexeme);
    }
    c->errores++;
}

/* revisa una expresion: cada variable que se lee debe existir y tener valor */
static void revisar_expresion(Contexto *c, const AstNodo *nodo)
{
    if (nodo == NULL) {
        return;
    }

    if (nodo->tipo == AST_VARIABLE) {
        int i = buscar(c, nodo->token.lexeme);
        if (i < 0) {
            reportar(c, &nodo->token, "variable no declarada");
        } else if (!c->variables[i].tiene_valor) {
            reportar(c, &nodo->token, "variable sin valor asignado");
        }
        return;
    }

    for (const AstNodo *h = nodo->hijo; h != NULL; h = h->siguiente) {
        revisar_expresion(c, h);
    }
}

static void revisar_sentencia(Contexto *c, const AstNodo *nodo)
{
    switch (nodo->tipo) {
    case AST_DECLARACION:
        /* primero la expresion inicial, despues se declara la variable */
        revisar_expresion(c, nodo->hijo);
        if (c->cantidad < MAX_VARIABLES) {
            Variable *v = &c->variables[c->cantidad++];
            strcpy(v->nombre, nodo->token.lexeme);
            v->tiene_valor = (nodo->hijo != NULL);
        }
        break;

    case AST_ASIGNACION: {
        revisar_expresion(c, nodo->hijo);
        int i = buscar(c, nodo->token.lexeme);
        if (i < 0) {
            reportar(c, &nodo->token, "variable no declarada");
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

    case AST_PROGRAMA:
    case AST_BLOQUE:
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