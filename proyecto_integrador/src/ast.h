#ifndef AST_H
#define AST_H

#include <stdio.h>

#include "lexer.h"

typedef enum {
    AST_PROGRAMA,
    AST_BLOQUE,
    AST_DECLARACION,
    AST_ASIGNACION,
    AST_IF,
    AST_WHILE,
    AST_PRINT,
    AST_BINARIA,
    AST_UNARIA,
    AST_NUMERO,
    AST_VARIABLE
} AstTipo;

typedef struct AstNodo AstNodo;

struct AstNodo {
    AstTipo tipo;
    Token token;       /* Nombre, número u operador, con su ubicación. */
    AstNodo *hijo;     /* Primer hijo del nodo. */
    AstNodo *siguiente; /* Siguiente hijo del mismo padre. */
};

/* Devuelve NULL si falla la reserva de memoria. */
AstNodo *ast_crear(AstTipo tipo, Token token);

/* Agrega el hijo al final de la lista. El padre pasa a ser su dueño. */
void ast_agregar_hijo(AstNodo *padre, AstNodo *hijo);

/* Escribe el árbol en el formato de --ast, en una sola línea. */
void ast_imprimir(FILE *salida, const AstNodo *raiz);

/* Libera la raíz y todos sus descendientes. */
void ast_liberar(AstNodo *raiz);

#endif
