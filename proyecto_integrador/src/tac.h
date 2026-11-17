#ifndef TAC_H
#define TAC_H

#include <stdio.h>

#include "ast.h"

/*
 * Recorre el AST y escribe el codigo de tres direcciones en `salida`,
 * una instruccion por linea, en el formato de docs/gramatica.md:
 *
 *     x = a op b        operacion binaria
 *     x = a             copia
 *     x = -a            negacion
 *     goto L1           salto incondicional
 *     ifFalso a goto L1 salta si a vale 0
 *     L1:               etiqueta
 *     print a           impresion
 *
 * Los temporales se llaman _t0, _t1, ... y no se reutilizan.
 * La salida ya viene optimizada: las operaciones entre constantes se
 * resuelven aqui mismo (plegado), asi que no quedan en el TAC.
 *
 * Supone que el programa ya paso el analisis semantico.
 */
void tac_generar(const AstNodo *raiz, FILE *salida);

#endif