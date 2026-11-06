/**
 * @file compilador.c
 * @brief Módulo principal de control y enrutamiento del compilador de Lenguajito.
 * @curso Lenguajes y Autómatas II (SCD-1016)
 * 
 * Este archivo se encarga de recibir la ruta del archivo fuente y el modo 
 * de ejecución solicitado (--tokens, --ast, --check, etc.), gestionando la 
 * lectura de memoria y la invocación de las etapas correspondientes del compilador.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compilador.h"
#include "lexer.h"
#include "parser.h"

/**
 * @brief Lee un archivo fuente completo y lo carga en un búfer de memoria dinámico.
 * @param ruta Ruta del archivo fuente en formato .lj
 * @return Puntero al contenido del archivo terminado en '\0', o NULL si falla.
 */
static char *leer_archivo(const char *ruta) {
    // Abrir el archivo en modo lectura de texto
    FILE *f = fopen(ruta, "r");
    if (!f) return NULL;
    
    // Calcular el tamaño total del archivo moviendo el puntero al final
    fseek(f, 0, SEEK_END);
    long tam = ftell(f);
    fseek(f, 0, SEEK_SET); // Regresar al inicio
    
    // Reservar memoria para el contenido más el carácter nulo '\0'
    char *buffer = malloc(tam + 1);
    if (!buffer) {
        fclose(f);
        return NULL;
    }
    
    // Leer el archivo completo hacia el búfer
    long leidos = fread(buffer, 1, tam, f);
    buffer[leidos] = '\0';
    
    fclose(f);
    return buffer;
}

/**
 * @brief Convierte un tipo de token enumerado a su representación textual en string.
 * @param type Tipo de token (TokenType) definido en el lexer.
 * @return Cadena de caracteres con la etiqueta exacta que exige la especificación.
 */
static const char* token_a_string(TokenType type) {
    switch (type) {
        case TOKEN_INT:     return "KEYWORD int";
        case TOKEN_IF:      return "KEYWORD if";
        case TOKEN_ELSE:    return "KEYWORD else";
        case TOKEN_WHILE:   return "KEYWORD while";
        case TOKEN_PRINT:   return "KEYWORD print";
        case TOKEN_ID:      return "IDENT";
        case TOKEN_NUM:     return "NUM";
        case TOKEN_ASSIGN:  return "ASSIGN";
        case TOKEN_PLUS:    return "PLUS";
        case TOKEN_MINUS:   return "MINUS";
        case TOKEN_MUL:     return "MUL";
        case TOKEN_DIV:     return "DIV";
        case TOKEN_EQ:      return "EQ";
        case TOKEN_NEQ:     return "NEQ";
        case TOKEN_LT:      return "LT";
        case TOKEN_GT:      return "GT";
        case TOKEN_LTE:     return "LTE";
        case TOKEN_GTE:     return "GTE";
        case TOKEN_LPAREN:  return "LPAREN";
        case TOKEN_RPAREN:  return "RPAREN";
        case TOKEN_LBRACE:  return "LBRACE";
        case TOKEN_RBRACE:  return "RBRACE";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        default:            return "UNKNOWN";
    }
}

/**
 * @brief Función principal de compilación que enruta la ejecución según el modo.
 * @param ruta Ruta del archivo fuente a compilar.
 * @param modo Bandera de operación (--tokens, --ast, --check, etc.).
 * @return 0 si todo sale bien, 1 si ocurren errores de compilación o de lectura.
 */
int compilar(const char *ruta, const char *modo) {
    // 1. Cargar el código fuente en memoria
    char *codigo = leer_archivo(ruta);
    if (!codigo) {
        fprintf(stderr, "error: no se pudo abrir o leer el archivo fuente: %s\n", ruta);
        return 1;
    }

    // 2. Manejo del modo --tokens (Hito 1)
    if (strcmp(modo, "--tokens") == 0) {
        Lexer l;
        lexer_init(&l, codigo);
        Token t;
        
        // Bucle para extraer tokens secuencialmente hasta el final del archivo
        do {
            t = lexer_next_token(&l);
            
            // Si alcanzamos el fin de archivo, terminamos el recorrido
            if (t.type == TOKEN_EOF) break;
            
            // Si el lexer encuentra un carácter no reconocido, reporta error léxico
            if (t.type == TOKEN_ERROR) {
                fprintf(stderr, "%d:%d error léxico: carácter inesperado '%s'\n", t.line, t.col, t.lexeme);
                free(codigo);
                return 1;
            }
            
            // Imprime en el formato estricto requerido: linea:columna TIPO lexema
            printf("%d:%d %s %s\n", t.line, t.col, token_a_string(t.type), t.lexeme);
            
        } while (t.type != TOKEN_EOF);

        free(codigo);
        return 0;
    }
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

if (strcmp(modo, "--check") == 0) {
    AstNodo *raiz = parser_analizar(codigo, ruta, stdout);
    free(codigo);

    if (raiz == NULL) {
        return 1;
    }

    ast_liberar(raiz);
    return 0;
}
    // 3. Modos pendientes para siguientes etapas del proyecto
    free(codigo);
    fprintf(stderr, "%s: el modo '%s' aún no está implementado en este hito\n", ruta, modo);
    return 1;
}
