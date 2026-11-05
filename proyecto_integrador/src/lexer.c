#include "lexer.h"

#include <string.h>

static int es_letra(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static int es_digito(char c)
{
    return c >= '0' && c <= '9';
}

static char avanzar(Lexer *lexer)
{
    char c = lexer->source[lexer->pos++];

    if (c == '\n') {
        lexer->line++;
        lexer->col = 1;
    } else {
        lexer->col++;
    }

    return c;
}

static int consumir(Lexer *lexer, char esperado)
{
    if (lexer->source[lexer->pos] != esperado) {
        return 0;
    }

    avanzar(lexer);
    return 1;
}

static void saltar_espacios_y_comentarios(Lexer *lexer)
{
    for (;;) {
        char c = lexer->source[lexer->pos];

        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            avanzar(lexer);
        } else if (c == '/' && lexer->source[lexer->pos + 1] == '/') {
            while (lexer->source[lexer->pos] != '\0' &&
                   lexer->source[lexer->pos] != '\n') {
                avanzar(lexer);
            }
        } else {
            return;
        }
    }
}

static TokenType tipo_de_identificador(const char *lexema)
{
    if (strcmp(lexema, "int") == 0) return TOKEN_INT;
    if (strcmp(lexema, "if") == 0) return TOKEN_IF;
    if (strcmp(lexema, "else") == 0) return TOKEN_ELSE;
    if (strcmp(lexema, "while") == 0) return TOKEN_WHILE;
    if (strcmp(lexema, "print") == 0) return TOKEN_PRINT;

    return TOKEN_ID;
}

static TokenType tipo_de_simbolo(Lexer *lexer, char c)
{
    switch (c) {
    case '+': return TOKEN_PLUS;
    case '-': return TOKEN_MINUS;
    case '*': return TOKEN_MUL;
    case '/': return TOKEN_DIV;
    case '(': return TOKEN_LPAREN;
    case ')': return TOKEN_RPAREN;
    case '{': return TOKEN_LBRACE;
    case '}': return TOKEN_RBRACE;
    case ';': return TOKEN_SEMICOLON;
    case '=': return consumir(lexer, '=') ? TOKEN_EQ : TOKEN_ASSIGN;
    case '!': return consumir(lexer, '=') ? TOKEN_NEQ : TOKEN_ERROR;
    case '<': return consumir(lexer, '=') ? TOKEN_LTE : TOKEN_LT;
    case '>': return consumir(lexer, '=') ? TOKEN_GTE : TOKEN_GT;
    default:  return TOKEN_ERROR;
    }
}

void lexer_init(Lexer *lexer, const char *source)
{
    lexer->source = source;
    lexer->pos = 0;
    lexer->line = 1;
    lexer->col = 1;
}

Token lexer_next_token(Lexer *lexer)
{
    saltar_espacios_y_comentarios(lexer);

    size_t inicio = lexer->pos;
    Token token = {
        .type = TOKEN_EOF,
        .line = lexer->line,
        .col = lexer->col
    };

    if (lexer->source[inicio] == '\0') {
        return token;
    }

    char c = avanzar(lexer);

    if (es_letra(c)) {
        while (es_letra(lexer->source[lexer->pos]) ||
               es_digito(lexer->source[lexer->pos])) {
            avanzar(lexer);
        }
        token.type = TOKEN_ID;
    } else if (es_digito(c)) {
        while (es_digito(lexer->source[lexer->pos])) {
            avanzar(lexer);
        }
        token.type = TOKEN_NUM;
    } else {
        token.type = tipo_de_simbolo(lexer, c);
    }

    size_t longitud = lexer->pos - inicio;
    if (longitud >= sizeof token.lexeme) {
        token.type = TOKEN_ERROR;
        longitud = sizeof token.lexeme - 1;
    }

    memcpy(token.lexeme, lexer->source + inicio, longitud);
    token.lexeme[longitud] = '\0';

    if (token.type == TOKEN_ID) {
        token.type = tipo_de_identificador(token.lexeme);
    }

    return token;
}
