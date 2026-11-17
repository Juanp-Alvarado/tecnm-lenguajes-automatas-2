#ifndef SEMANTICA_H
#define SEMANTICA_H

#include <stdio.h>

#include "ast.h"

/*
 * Revisa el árbol y reporta errores semánticos:
 *   E01  variable no declarada, o leída antes de tener valor
 *   E02  variable ya declarada en el mismo ámbito
 * Escribe los diagnósticos en `diagnosticos` (máximo 20).
 * Devuelve cuántos errores encontró (0 = todo bien).
 *
 * Los ámbitos son anidados: el programa es el ámbito global y cada bloque
 * { } abre uno nuevo (push_scope / pop_scope).
 */
unsigned semantica_revisar(const AstNodo *raiz, const char *ruta,
                           FILE *diagnosticos);

#endif