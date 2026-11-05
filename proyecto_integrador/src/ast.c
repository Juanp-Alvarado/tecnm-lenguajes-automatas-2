#include "ast.h"

#include <stdlib.h>

AstNodo *ast_crear(AstTipo tipo, Token token)
{
    AstNodo *nodo = malloc(sizeof *nodo);
    if (nodo != NULL) {
        *nodo = (AstNodo){ .tipo = tipo, .token = token };
    }
    return nodo;
}

void ast_agregar_hijo(AstNodo *padre, AstNodo *hijo)
{
    AstNodo **enlace = &padre->hijo;
    while (*enlace != NULL) {
        enlace = &(*enlace)->siguiente;
    }
    *enlace = hijo;
}

static const char *etiqueta(const AstNodo *nodo)
{
    switch (nodo->tipo) {
    case AST_PROGRAMA:    return "programa";
    case AST_BLOQUE:      return "bloque";
    case AST_DECLARACION: return "declarar";
    case AST_ASIGNACION:  return "asignar";
    case AST_IF:          return "si";
    case AST_WHILE:       return "mientras";
    case AST_PRINT:       return "imprimir";
    case AST_BINARIA:     return nodo->token.lexeme;
    case AST_UNARIA:      return "neg";
    case AST_NUMERO:      return "num";
    case AST_VARIABLE:    return "var";
    }
    return "";
}

static void imprimir_nodo(FILE *salida, const AstNodo *nodo)
{
    fprintf(salida, "(%s", etiqueta(nodo));

    if (nodo->tipo == AST_DECLARACION || nodo->tipo == AST_ASIGNACION ||
        nodo->tipo == AST_NUMERO || nodo->tipo == AST_VARIABLE) {
        fprintf(salida, " %s", nodo->token.lexeme);
    }

    unsigned indice = 0;
    for (const AstNodo *hijo = nodo->hijo; hijo != NULL;
         hijo = hijo->siguiente, ++indice) {
        if (nodo->tipo == AST_IF && indice > 0) {
            fputs(indice == 1 ? " (entonces " : " (si_no ", salida);
            imprimir_nodo(salida, hijo);
            fputc(')', salida);
        } else {
            fputc(' ', salida);
            imprimir_nodo(salida, hijo);
        }
    }
    fputc(')', salida);
}

void ast_imprimir(FILE *salida, const AstNodo *raiz)
{
    if (raiz != NULL) {
        imprimir_nodo(salida, raiz);
        fputc('\n', salida);
    }
}

void ast_liberar(AstNodo *raiz)
{
    if (raiz == NULL) {
        return;
    }

    AstNodo *hijo = raiz->hijo;
    while (hijo != NULL) {
        AstNodo *siguiente = hijo->siguiente;
        ast_liberar(hijo);
        hijo = siguiente;
    }
    free(raiz);
}
