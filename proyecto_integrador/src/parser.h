#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

/*
 * Analiza el código fuente y devuelve la raíz del árbol.
 * Si hay errores, escribe los diagnósticos y devuelve NULL.
 * Quien recibe el árbol debe liberarlo con ast_liberar().
 */
AstNodo *parser_analizar(const char *fuente, const char *ruta,
                         FILE *diagnosticos);

#endif
