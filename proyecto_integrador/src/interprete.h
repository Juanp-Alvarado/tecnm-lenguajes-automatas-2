#ifndef INTERPRETE_H
#define INTERPRETE_H

#include <stdio.h>

#include "ast.h"

/*
 * Recorre el AST y ejecuta el programa, escribiendo lo que imprima
 * `print` en `salida`. Se usa desde --run en el Hito 2; a partir del
 * Hito 4 ese modo debe ejecutar el binario generado, no este interprete.
 *
 * Supone que el programa ya paso el analisis semantico.
 */
void interprete_ejecutar(const AstNodo *raiz, FILE *salida);

#endif