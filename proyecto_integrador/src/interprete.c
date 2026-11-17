#include "interprete.h"

#include <stdlib.h>
#include <string.h>

#define MAX_VARIABLES 500
#define MAX_AMBITOS   100

typedef struct {
    char nombre[256];
    int valor;
} Variable;

/*
 * Memoria del interprete.
 * Misma idea de ambitos que en semantica.c: todas las variables vivas
 * estan en un arreglo, y cada ambito recuerda en que posicion empezo.
 * Al cerrar un bloque se regresa `cantidad` a esa marca.
 */
typedef struct {
    Variable variables[MAX_VARIABLES];
    int cantidad;

    int marcas[MAX_AMBITOS];
    int profundidad;            /* 0 = ambito global */
    int exceso;                 /* bloques anidados de mas */

    FILE *salida;
} Memoria;

static void push_scope(Memoria *m)
{
    if (m->profundidad + 1 >= MAX_AMBITOS) {
        m->exceso++;
        return;
    }
    m->profundidad++;
    m->marcas[m->profundidad] = m->cantidad;
}

static void pop_scope(Memoria *m)
{
    if (m->exceso > 0) {
        m->exceso--;
        return;
    }
    m->cantidad = m->marcas[m->profundidad];
    m->profundidad--;
}

/* busca de adentro hacia afuera: la declaracion mas reciente gana (shadowing) */
static Variable *buscar(Memoria *m, const char *nombre)
{
    for (int i = m->cantidad - 1; i >= 0; i--) {
        if (strcmp(m->variables[i].nombre, nombre) == 0) {
            return &m->variables[i];
        }
    }
    return NULL;
}

static void declarar(Memoria *m, const char *nombre, int valor)
{
    if (m->cantidad >= MAX_VARIABLES) {
        return;
    }
    Variable *v = &m->variables[m->cantidad++];
    snprintf(v->nombre, sizeof v->nombre, "%s", nombre);
    v->valor = valor;
}

static void ejecutar(Memoria *m, const AstNodo *nodo);

/* Evalua una expresion y devuelve su valor entero.
   Las comparaciones devuelven 1 (verdadero) o 0 (falso). */
static int evaluar(Memoria *m, const AstNodo *nodo)
{
    if (nodo == NULL) {
        return 0;
    }

    switch (nodo->tipo) {
    case AST_NUMERO:
        return (int)strtol(nodo->token.lexeme, NULL, 10);

    case AST_VARIABLE: {
        Variable *v = buscar(m, nodo->token.lexeme);
        return v != NULL ? v->valor : 0;
    }

    case AST_UNARIA:
        /* el parser solo crea este nodo para el '-' */
        return -evaluar(m, nodo->hijo);

    case AST_BINARIA: {
        const AstNodo *izq = nodo->hijo;
        const AstNodo *der = izq != NULL ? izq->siguiente : NULL;
        if (izq == NULL || der == NULL) {
            return 0;
        }

        int a = evaluar(m, izq);
        int b = evaluar(m, der);

        switch (nodo->token.type) {
        case TOKEN_PLUS:  return a + b;
        case TOKEN_MINUS: return a - b;
        case TOKEN_MUL:   return a * b;
        case TOKEN_DIV:   return b != 0 ? a / b : 0;  /* el caso b==0 lo ataja E04 */
        case TOKEN_LT:    return a <  b;
        case TOKEN_LTE:   return a <= b;
        case TOKEN_GT:    return a >  b;
        case TOKEN_GTE:   return a >= b;
        case TOKEN_EQ:    return a == b;
        case TOKEN_NEQ:   return a != b;
        default:          return 0;
        }
    }

    default:
        return 0;
    }
}

static void ejecutar_lista(Memoria *m, const AstNodo *primero)
{
    for (const AstNodo *h = primero; h != NULL; h = h->siguiente) {
        ejecutar(m, h);
    }
}

static void ejecutar(Memoria *m, const AstNodo *nodo)
{
    if (nodo == NULL) {
        return;
    }

    switch (nodo->tipo) {
    case AST_DECLARACION:
        /* sin inicializador vale 0; semantica ya impidio que se lea asi */
        declarar(m, nodo->token.lexeme,
                 nodo->hijo != NULL ? evaluar(m, nodo->hijo) : 0);
        break;

    case AST_ASIGNACION: {
        int valor = evaluar(m, nodo->hijo);
        Variable *v = buscar(m, nodo->token.lexeme);
        if (v != NULL) {
            v->valor = valor;
        }
        break;
    }

    case AST_IF: {
        const AstNodo *condicion = nodo->hijo;
        const AstNodo *entonces = condicion != NULL ? condicion->siguiente : NULL;
        const AstNodo *si_no = entonces != NULL ? entonces->siguiente : NULL;

        if (evaluar(m, condicion)) {
            ejecutar(m, entonces);
        } else {
            ejecutar(m, si_no);
        }
        break;
    }

    case AST_WHILE: {
        const AstNodo *condicion = nodo->hijo;
        const AstNodo *cuerpo = condicion != NULL ? condicion->siguiente : NULL;

        while (evaluar(m, condicion)) {
            ejecutar(m, cuerpo);
        }
        break;
    }

    case AST_PRINT:
        fprintf(m->salida, "%d\n", evaluar(m, nodo->hijo));
        break;

    case AST_BLOQUE:
        /* cada bloque abre su propio ambito, igual que en semantica.c */
        push_scope(m);
        ejecutar_lista(m, nodo->hijo);
        pop_scope(m);
        break;

    case AST_PROGRAMA:
        /* el programa es el ambito global: no abre uno nuevo */
        ejecutar_lista(m, nodo->hijo);
        break;

    default:
        break;
    }
}

void interprete_ejecutar(const AstNodo *raiz, FILE *salida)
{
    static Memoria m;  /* static: es grande y asi no va en la pila */
    memset(&m, 0, sizeof m);
    m.salida = salida;

    ejecutar(&m, raiz);
}