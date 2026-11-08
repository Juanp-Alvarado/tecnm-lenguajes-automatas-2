#ifndef SEMANTICA_H
#define SEMANTICA_H

#include <stdio.h>

#include "ast.h"

/*
 * Revisa el árbol y reporta errores E01 (variable no declarada o leída
 * antes de tener valor). Escribe los diagnósticos en `diagnosticos`.
 * Devuelve cuántos errores encontró (0 = todo bien).
 *
 * Hito 1: usa una sola lista de variables. En el Hito 2 se cambia por
 * ámbitos anidados y se agregan E02 y E04.
 */
unsigned semantica_revisar(const AstNodo *raiz, const char *ruta,
                           FILE *diagnosticos);

#endif