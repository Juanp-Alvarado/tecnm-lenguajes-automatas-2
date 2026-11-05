#include "parser.h"

typedef struct {
    Lexer lexer;
    Token actual;
    const char *ruta;
    FILE *diagnosticos;
    unsigned errores;
    int sin_memoria;
} Parser;

static Token tomar(Parser *p)
{
    Token anterior = p->actual;
    p->actual = lexer_next_token(&p->lexer);
    return anterior;
}

static void error(Parser *p, const char *mensaje)
{
    if (p->errores < 20) {
        fprintf(p->diagnosticos, "%s:%d:%d: error S01: %s\n",
                p->ruta, p->actual.line, p->actual.col,
                p->actual.type == TOKEN_ERROR ? "token invalido" : mensaje);
        ++p->errores;
    }
}

static int aceptar(Parser *p, TokenType tipo)
{
    if (p->actual.type != tipo) {
        return 0;
    }
    tomar(p);
    return 1;
}

static int esperar(Parser *p, TokenType tipo, const char *mensaje)
{
    if (aceptar(p, tipo)) {
        return 1;
    }
    error(p, mensaje);
    return 0;
}

static AstNodo *crear(Parser *p, AstTipo tipo, Token token)
{
    AstNodo *nodo = ast_crear(tipo, token);
    if (nodo == NULL) {
        p->sin_memoria = 1;
        fprintf(p->diagnosticos, "%s: memoria insuficiente\n", p->ruta);
    }
    return nodo;
}

static AstNodo *expresion(Parser *p, int minimo);
static AstNodo *sentencia(Parser *p);

static AstNodo *primaria(Parser *p)
{
    Token token = p->actual;
    switch (token.type) {
    case TOKEN_NUM:
    case TOKEN_ID:
        tomar(p);
        return crear(p, token.type == TOKEN_NUM ? AST_NUMERO : AST_VARIABLE,
                     token);
    case TOKEN_LPAREN: {
        tomar(p);
        AstNodo *nodo = expresion(p, 1);
        if (nodo != NULL && !esperar(p, TOKEN_RPAREN, "se esperaba ')'")) {
            ast_liberar(nodo);
            return NULL;
        }
        return nodo;
    }
    default:
        error(p, "se esperaba una expresion");
        return NULL;
    }
}

static AstNodo *unaria(Parser *p)
{
    Token signo = p->actual;
    if (signo.type != TOKEN_PLUS && signo.type != TOKEN_MINUS) {
        return primaria(p);
    }

    tomar(p);
    AstNodo *hijo = primaria(p);
    if (hijo == NULL || signo.type == TOKEN_PLUS) {
        return hijo;
    }

    AstNodo *nodo = crear(p, AST_UNARIA, signo);
    if (nodo == NULL) {
        ast_liberar(hijo);
        return NULL;
    }
    ast_agregar_hijo(nodo, hijo);
    return nodo;
}

static int precedencia(TokenType tipo)
{
    switch (tipo) {
    case TOKEN_LT: case TOKEN_LTE: case TOKEN_GT: case TOKEN_GTE:
    case TOKEN_EQ: case TOKEN_NEQ:
        return 1;
    case TOKEN_PLUS: case TOKEN_MINUS:
        return 2;
    case TOKEN_MUL: case TOKEN_DIV:
        return 3;
    default:
        return 0;
    }
}

static AstNodo *expresion(Parser *p, int minimo)
{
    AstNodo *izq = unaria(p);
    while (izq != NULL) {
        int nivel = precedencia(p->actual.type);
        if (nivel < minimo) {
            break;
        }

        Token operador = tomar(p);
        AstNodo *der = expresion(p, nivel + 1);
        if (der == NULL) {
            ast_liberar(izq);
            return NULL;
        }

        AstNodo *nodo = crear(p, AST_BINARIA, operador);
        if (nodo == NULL) {
            ast_liberar(izq);
            ast_liberar(der);
            return NULL;
        }
        ast_agregar_hijo(nodo, izq);
        ast_agregar_hijo(nodo, der);
        izq = nodo;
        if (nivel == 1) {
            break;
        }
    }
    return izq;
}

static int agregar(AstNodo *padre, AstNodo *hijo)
{
    if (hijo == NULL) {
        return 0;
    }
    ast_agregar_hijo(padre, hijo);
    return 1;
}

static int inicia_sentencia(TokenType tipo)
{
    return tipo == TOKEN_INT || tipo == TOKEN_ID || tipo == TOKEN_IF ||
           tipo == TOKEN_WHILE || tipo == TOKEN_PRINT || tipo == TOKEN_LBRACE;
}

static void recuperar(Parser *p, size_t inicio, int en_bloque)
{
    while (p->actual.type != TOKEN_EOF) {
        if (en_bloque && p->actual.type == TOKEN_RBRACE) {
            return;
        }
        if (aceptar(p, TOKEN_SEMICOLON)) {
            return;
        }
        if (p->lexer.pos != inicio && inicia_sentencia(p->actual.type)) {
            return;
        }
        tomar(p);
    }
}

static void lista(Parser *p, AstNodo *padre, int en_bloque)
{
    while (!p->sin_memoria && p->errores < 20 &&
           p->actual.type != TOKEN_EOF &&
           !(en_bloque && p->actual.type == TOKEN_RBRACE)) {
        size_t inicio = p->lexer.pos;
        if (!agregar(padre, sentencia(p)) && !p->sin_memoria) {
            recuperar(p, inicio, en_bloque);
        }
    }
}

static AstNodo *bloque(Parser *p)
{
    Token llave = p->actual;
    if (!esperar(p, TOKEN_LBRACE, "se esperaba '{'")) {
        return NULL;
    }

    AstNodo *nodo = crear(p, AST_BLOQUE, llave);
    if (nodo != NULL) {
        lista(p, nodo, 1);
        if (p->sin_memoria || p->errores == 20 ||
            !esperar(p, TOKEN_RBRACE, "se esperaba '}'")) {
            ast_liberar(nodo);
            return NULL;
        }
    }
    return nodo;
}

static AstNodo *sentencia_variable(Parser *p, int declaracion)
{
    Token nombre = p->actual;
    if (!esperar(p, TOKEN_ID, "se esperaba un identificador")) {
        return NULL;
    }

    AstNodo *nodo = crear(p, declaracion ? AST_DECLARACION : AST_ASIGNACION,
                         nombre);
    if (nodo == NULL) {
        return NULL;
    }

    if (aceptar(p, TOKEN_ASSIGN)) {
        if (!agregar(nodo, expresion(p, 1))) {
            ast_liberar(nodo);
            return NULL;
        }
    } else if (!declaracion) {
        error(p, "se esperaba '='");
        ast_liberar(nodo);
        return NULL;
    }

    if (!esperar(p, TOKEN_SEMICOLON, "se esperaba ';'")) {
        ast_liberar(nodo);
        return NULL;
    }
    return nodo;
}

static AstNodo *sentencia_con_parentesis(Parser *p, AstTipo tipo)
{
    Token palabra = tomar(p);
    AstNodo *nodo = crear(p, tipo, palabra);
    if (nodo == NULL) {
        return NULL;
    }

    int valida = esperar(p, TOKEN_LPAREN, "se esperaba '('") &&
                 agregar(nodo, expresion(p, 1)) &&
                 esperar(p, TOKEN_RPAREN, "se esperaba ')'");

    if (valida && tipo == AST_PRINT) {
        valida = esperar(p, TOKEN_SEMICOLON, "se esperaba ';'");
    } else if (valida) {
        valida = agregar(nodo, bloque(p));
        if (valida && tipo == AST_IF && aceptar(p, TOKEN_ELSE)) {
            valida = agregar(nodo, bloque(p));
        }
    }

    if (!valida) {
        ast_liberar(nodo);
        return NULL;
    }
    return nodo;
}

static AstNodo *sentencia(Parser *p)
{
    switch (p->actual.type) {
    case TOKEN_INT:
        tomar(p);
        return sentencia_variable(p, 1);
    case TOKEN_ID:
        return sentencia_variable(p, 0);
    case TOKEN_IF:
        return sentencia_con_parentesis(p, AST_IF);
    case TOKEN_WHILE:
        return sentencia_con_parentesis(p, AST_WHILE);
    case TOKEN_PRINT:
        return sentencia_con_parentesis(p, AST_PRINT);
    case TOKEN_LBRACE:
        return bloque(p);
    default:
        error(p, "se esperaba una sentencia");
        return NULL;
    }
}

AstNodo *parser_analizar(const char *fuente, const char *ruta,
                         FILE *diagnosticos)
{
    Parser p = { .ruta = ruta, .diagnosticos = diagnosticos };
    lexer_init(&p.lexer, fuente);
    p.actual = lexer_next_token(&p.lexer);

    AstNodo *raiz = crear(&p, AST_PROGRAMA, p.actual);
    if (raiz != NULL) {
        lista(&p, raiz, 0);
        if (p.errores != 0 || p.sin_memoria) {
            ast_liberar(raiz);
            return NULL;
        }
    }
    return raiz;
}
