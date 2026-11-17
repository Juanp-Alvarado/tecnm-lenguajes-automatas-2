#include "tac.h"

#include <stdlib.h>
#include <string.h>

/*
 * Un operando del TAC: o es una constante conocida en compilacion, o es
 * el nombre de algo que existe en tiempo de ejecucion (una variable del
 * programa o un temporal _tN).
 *
 * Guardar el valor de las constantes es lo que permite el plegado: si
 * los dos lados de una operacion son constantes, se calcula aqui y no
 * se emite ninguna instruccion.
 */
typedef struct {
    char texto[288];
    int es_constante;
    int valor;
} Operando;

typedef struct {
    FILE *salida;
    unsigned temporales;   /* siguiente _tN */
    unsigned etiquetas;    /* ultima LN usada */
} Generador;

static Operando constante(int valor)
{
    Operando o;
    o.es_constante = 1;
    o.valor = valor;
    snprintf(o.texto, sizeof o.texto, "%d", valor);
    return o;
}

static Operando referencia(const char *nombre)
{
    Operando o;
    o.es_constante = 0;
    o.valor = 0;
    snprintf(o.texto, sizeof o.texto, "%s", nombre);
    return o;
}

/* cada temporal se usa una sola vez y nunca se reutiliza */
static Operando nuevo_temporal(Generador *g)
{
    char nombre[32];
    snprintf(nombre, sizeof nombre, "_t%u", g->temporales++);
    return referencia(nombre);
}

static unsigned nueva_etiqueta(Generador *g)
{
    return ++g->etiquetas;   /* L1, L2, ... */
}

static const char *texto_operador(TokenType tipo)
{
    switch (tipo) {
    case TOKEN_PLUS:  return "+";
    case TOKEN_MINUS: return "-";
    case TOKEN_MUL:   return "*";
    case TOKEN_DIV:   return "/";
    case TOKEN_LT:    return "<";
    case TOKEN_LTE:   return "<=";
    case TOKEN_GT:    return ">";
    case TOKEN_GTE:   return ">=";
    case TOKEN_EQ:    return "==";
    case TOKEN_NEQ:   return "!=";
    default:          return "?";
    }
}

/* calcula una operacion entre dos constantes; `valido` dice si se pudo */
static int plegar(TokenType operador, int a, int b, int *valido)
{
    *valido = 1;

    switch (operador) {
    case TOKEN_PLUS:  return a + b;
    case TOKEN_MINUS: return a - b;
    case TOKEN_MUL:   return a * b;
    case TOKEN_DIV:
        if (b == 0) {        /* lo ataja E04; aqui solo evitamos dividir */
            *valido = 0;
            return 0;
        }
        return a / b;
    case TOKEN_LT:    return a <  b;
    case TOKEN_LTE:   return a <= b;
    case TOKEN_GT:    return a >  b;
    case TOKEN_GTE:   return a >= b;
    case TOKEN_EQ:    return a == b;
    case TOKEN_NEQ:   return a != b;
    default:
        *valido = 0;
        return 0;
    }
}

static void generar_sentencia(Generador *g, const AstNodo *nodo);

/*
 * Genera el codigo de una expresion y devuelve donde quedo su valor.
 * Si el resultado se conoce en compilacion, devuelve la constante y no
 * emite nada: por eso no quedan temporales que nadie use.
 */
static Operando generar_expresion(Generador *g, const AstNodo *nodo)
{
    if (nodo == NULL) {
        return constante(0);
    }

    switch (nodo->tipo) {
    case AST_NUMERO:
        return constante((int)strtol(nodo->token.lexeme, NULL, 10));

    case AST_VARIABLE:
        return referencia(nodo->token.lexeme);

    case AST_UNARIA: {
        Operando valor = generar_expresion(g, nodo->hijo);

        /* -5 es simplemente la constante -5 */
        if (valor.es_constante) {
            return constante(-valor.valor);
        }

        Operando destino = nuevo_temporal(g);
        fprintf(g->salida, "%s = -%s\n", destino.texto, valor.texto);
        return destino;
    }

    case AST_BINARIA: {
        const AstNodo *izquierdo = nodo->hijo;
        const AstNodo *derecho = izquierdo != NULL ? izquierdo->siguiente : NULL;
        if (izquierdo == NULL || derecho == NULL) {
            return constante(0);
        }

        Operando a = generar_expresion(g, izquierdo);
        Operando b = generar_expresion(g, derecho);

        /* plegado: si los dos lados son constantes, se resuelve aqui */
        if (a.es_constante && b.es_constante) {
            int valido = 0;
            int resultado = plegar(nodo->token.type, a.valor, b.valor, &valido);
            if (valido) {
                return constante(resultado);
            }
        }

        Operando destino = nuevo_temporal(g);
        fprintf(g->salida, "%s = %s %s %s\n", destino.texto, a.texto,
                texto_operador(nodo->token.type), b.texto);
        return destino;
    }

    default:
        return constante(0);
    }
}

static void generar_lista(Generador *g, const AstNodo *primero)
{
    for (const AstNodo *h = primero; h != NULL; h = h->siguiente) {
        generar_sentencia(g, h);
    }
}

static void generar_if(Generador *g, const AstNodo *nodo)
{
    const AstNodo *condicion = nodo->hijo;
    const AstNodo *entonces = condicion != NULL ? condicion->siguiente : NULL;
    const AstNodo *si_no = entonces != NULL ? entonces->siguiente : NULL;

    Operando prueba = generar_expresion(g, condicion);
    unsigned salida = nueva_etiqueta(g);

    fprintf(g->salida, "ifFalso %s goto L%u\n", prueba.texto, salida);
    generar_sentencia(g, entonces);

    if (si_no == NULL) {
        fprintf(g->salida, "L%u:\n", salida);
        return;
    }

    /* con else hace falta una segunda etiqueta para brincarse esa rama */
    unsigned fin = nueva_etiqueta(g);
    fprintf(g->salida, "goto L%u\n", fin);
    fprintf(g->salida, "L%u:\n", salida);
    generar_sentencia(g, si_no);
    fprintf(g->salida, "L%u:\n", fin);
}

static void generar_while(Generador *g, const AstNodo *nodo)
{
    const AstNodo *condicion = nodo->hijo;
    const AstNodo *cuerpo = condicion != NULL ? condicion->siguiente : NULL;

    /* la condicion se vuelve a evaluar en cada vuelta, por eso va
       despues de la etiqueta de inicio */
    unsigned inicio = nueva_etiqueta(g);
    fprintf(g->salida, "L%u:\n", inicio);

    Operando prueba = generar_expresion(g, condicion);
    unsigned fin = nueva_etiqueta(g);

    fprintf(g->salida, "ifFalso %s goto L%u\n", prueba.texto, fin);
    generar_sentencia(g, cuerpo);
    fprintf(g->salida, "goto L%u\n", inicio);
    fprintf(g->salida, "L%u:\n", fin);
}

static void generar_sentencia(Generador *g, const AstNodo *nodo)
{
    if (nodo == NULL) {
        return;
    }

    switch (nodo->tipo) {
    case AST_DECLARACION:
        /* una declaracion sin inicializador no genera codigo */
        if (nodo->hijo != NULL) {
            Operando valor = generar_expresion(g, nodo->hijo);
            fprintf(g->salida, "%s = %s\n", nodo->token.lexeme, valor.texto);
        }
        break;

    case AST_ASIGNACION: {
        Operando valor = generar_expresion(g, nodo->hijo);
        fprintf(g->salida, "%s = %s\n", nodo->token.lexeme, valor.texto);
        break;
    }

    case AST_PRINT: {
        Operando valor = generar_expresion(g, nodo->hijo);
        fprintf(g->salida, "print %s\n", valor.texto);
        break;
    }

    case AST_IF:
        generar_if(g, nodo);
        break;

    case AST_WHILE:
        generar_while(g, nodo);
        break;

    case AST_BLOQUE:
    case AST_PROGRAMA:
        generar_lista(g, nodo->hijo);
        break;

    default:
        break;
    }
}

void tac_generar(const AstNodo *raiz, FILE *salida)
{
    Generador g;
    memset(&g, 0, sizeof g);
    g.salida = salida;

    generar_sentencia(&g, raiz);
}