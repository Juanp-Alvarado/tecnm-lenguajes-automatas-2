/**
 * @file compilador.c
 * @brief Modulo principal del compilador de Lenguajito: recibe la ruta del
 *        archivo y el modo (--tokens, --ast, --check) y llama a cada etapa.
 * @curso Lenguajes y Automatas II (SCD-1016)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compilador.h"
#include "lexer.h"
#include "parser.h"
#include "semantica.h"

/* Lee todo el archivo fuente a un buffer terminado en '\0' (NULL si falla). */
static char *leer_archivo(const char *ruta) {
    FILE *f = fopen(ruta, "r");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long tam = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = malloc(tam + 1);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    long leidos = fread(buffer, 1, tam, f);
    buffer[leidos] = '\0';

    fclose(f);
    return buffer;
}

/* Categoria del token para --tokens: KW, ID, NUM, OP, DELIM o EOF. */
static const char *tipo_a_string(TokenType type) {
    switch (type) {
        case TOKEN_INT:
        case TOKEN_IF:
        case TOKEN_ELSE:
        case TOKEN_WHILE:
        case TOKEN_PRINT:     return "KW";
        case TOKEN_ID:        return "ID";
        case TOKEN_NUM:       return "NUM";
        case TOKEN_ASSIGN:
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_MUL:
        case TOKEN_DIV:
        case TOKEN_EQ:
        case TOKEN_NEQ:
        case TOKEN_LT:
        case TOKEN_GT:
        case TOKEN_LTE:
        case TOKEN_GTE:       return "OP";
        case TOKEN_LPAREN:
        case TOKEN_RPAREN:
        case TOKEN_LBRACE:
        case TOKEN_RBRACE:
        case TOKEN_SEMICOLON: return "DELIM";
        case TOKEN_EOF:       return "EOF";
        default:              return "ERROR";
    }
}

/* --tokens: un token por linea, "linea:columna TIPO lexema". */
static int modo_tokens(const char *codigo, const char *ruta) {
    Lexer l;
    lexer_init(&l, codigo);
    int hubo_error = 0;
    Token t;

    do {
        t = lexer_next_token(&l);

        if (t.type == TOKEN_ERROR) {
            /* caracter invalido: error lexico S01, con el formato de diagnostico */
            printf("%s:%d:%d: error S01: carácter inválido '%s'\n",
                   ruta, t.line, t.col, t.lexeme);
            hubo_error = 1;
        } else if (t.type == TOKEN_EOF) {
            /* el EOF sale una sola vez, al final, sin lexema */
            printf("%d:%d EOF\n", t.line, t.col);
        } else {
            printf("%d:%d %s %s\n", t.line, t.col, tipo_a_string(t.type), t.lexeme);
        }
    } while (t.type != TOKEN_EOF);

    return hubo_error;
}

/* Funcion principal: elige que hacer segun el modo. */
int compilar(const char *ruta, const char *modo) {
    char *codigo = leer_archivo(ruta);
    if (!codigo) {
        fprintf(stderr, "error: no se pudo abrir o leer el archivo fuente: %s\n", ruta);
        return 1;
    }

    /* --tokens (Hito 1) */
    if (strcmp(modo, "--tokens") == 0) {
        int resultado = modo_tokens(codigo, ruta);
        free(codigo);
        return resultado;
    }

    /* --ast (Hito 1) */
    if (strcmp(modo, "--ast") == 0) {
        AstNodo *raiz = parser_analizar(codigo, ruta, stdout);
        free(codigo);

        if (raiz == NULL) {
            return 1;
        }

        ast_imprimir(stdout, raiz);
        ast_liberar(raiz);
        return 0;
    }

    /* --check (Hito 1): sintaxis (S01) y variables (E01), no genera archivos */
    if (strcmp(modo, "--check") == 0) {
        AstNodo *raiz = parser_analizar(codigo, ruta, stdout);
        free(codigo);

        if (raiz == NULL) {
            return 1;
        }

        unsigned errores = semantica_revisar(raiz, ruta, stdout);
        ast_liberar(raiz);
        return errores > 0 ? 1 : 0;
    }

    /* modos pendientes para los siguientes hitos */
    free(codigo);
    fprintf(stderr, "%s: el modo '%s' aún no está implementado en este hito\n", ruta, modo);
    return 1;
}
