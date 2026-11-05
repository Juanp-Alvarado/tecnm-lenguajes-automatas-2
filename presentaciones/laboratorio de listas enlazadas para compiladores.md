# Listas Enlazadas en la Construcción de Compiladores (GCC / Unix / Linux)

En la construcción de compiladores bajo el estándar Unix/Linux con GCC, las listas enlazadas son una de las estructuras de datos fundamentales. Se utilizan para modelar desde secuencias lineales de tokens y listas de parámetros, hasta tablas de símbolos encadenadas (manejo de colisiones y ámbitos léxicos) y listas de instrucciones intermedias o cuádruplos.

A continuación se presenta un programa formativo estructurado en tres lecciones progresivas: desde la gestión de memoria dinámica pura en C99/C11, pasando por la representación de sentencias de análisis sintáctico/semántico, hasta la implementación de una tabla de símbolos con resolución de ámbitos (*scopes*) en C++.

---

## Lección 1: Fundamentos de Listas Enlazadas en C (Nivel Básico)

### 1. Concepto y Manejo de Punteros
En C, un nodo de una lista es un bloque de memoria asignado dinámicamente con `malloc` o `calloc` que almacena un valor y la dirección (puntero) al siguiente bloque.

Para herramientas como Flex y Bison, es crucial garantizar:
* Asignación correcta de memoria.
* Comprobación estricta de punteros nulos (`NULL`).
* Liberación recursiva o iterativa para evitar fugas de memoria (*memory leaks*) que degraden el compilador.

### 2. Ejemplo Práctico: Lista Enlazada Simple para Tokens

```c
/* token_list.h */
#ifndef TOKEN_LIST_H
#define TOKEN_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TOKEN_ID,
    TOKEN_NUM,
    TOKEN_OP,
    TOKEN_EOF
} TokenType;

typedef struct TokenNode {
    TokenType type;
    char *lexeme;
    int line;
    struct TokenNode *next;
} TokenNode;

/* Prototipos */
TokenNode* token_node_create(TokenType type, const char *lexeme, int line);
void token_list_append(TokenNode **head, TokenType type, const char *lexeme, int line);
void token_list_print(const TokenNode *head);
void token_list_free(TokenNode **head);

#endif
```

```c
/* token_list.c */
#include "token_list.h"

TokenNode* token_node_create(TokenType type, const char *lexeme, int line) {
    TokenNode *node = (TokenNode *)malloc(sizeof(TokenNode));
    if (!node) {
        perror("Error asignando memoria para TokenNode");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    node->lexeme = strdup(lexeme); /* Duplica cadena; requiere free() */
    node->line = line;
    node->next = NULL;
    return node;
}

void token_list_append(TokenNode **head, TokenType type, const char *lexeme, int line) {
    TokenNode *new_node = token_node_create(type, lexeme, line);
    if (*head == NULL) {
        *head = new_node;
        return;
    }
    TokenNode *curr = *head;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = new_node;
}

void token_list_print(const TokenNode *head) {
    const TokenNode *curr = head;
    printf("--- Secuencia de Tokens ---\n");
    while (curr != NULL) {
        printf("Linea %3d | Tipo: %d | Lexema: %s\n", curr->line, curr->type, curr->lexeme);
        curr = curr->next;
    }
}

void token_list_free(TokenNode **head) {
    TokenNode *curr = *head;
    while (curr != NULL) {
        TokenNode *temp = curr;
        curr = curr->next;
        free(temp->lexeme);
        free(temp);
    }
    *head = NULL;
}

/* Compilacion: gcc -Wall -Wextra -std=c11 token_list.c -o token_list */
int main(void) {
    TokenNode *tokens = NULL;

    token_list_append(&tokens, TOKEN_ID, "suma", 1);
    token_list_append(&tokens, TOKEN_OP, "=", 1);
    token_list_append(&tokens, TOKEN_NUM, "42", 1);
    token_list_append(&tokens, TOKEN_EOF, "EOF", 2);

    token_list_print(tokens);
    token_list_free(&tokens);

    return 0;
}
```

### 3. Ejercicio de Programación
* **Problema:** Modifica `token_list_append` para mantener un puntero al último elemento (`tail`), logrando inserciones con complejidad $O(1)$ en lugar de $O(n)$.
* **Validación en Linux:** Compila con banderas de depuración y verifica la ausencia de fugas de memoria ejecutando:
  ```bash
  gcc -Wall -Wextra -g token_list.c -o token_list
  valgrind --leak-check=full ./token_list
  ```

---

## Lección 2: Listas Heterogéneas y Árboles Sintácticos (Nivel Intermedio)

### 1. Concepto: Listas en el Análisis Sintáctico (AST)
Durante el análisis sintáctico (usualmente con Yacc/Bison), una gramática frecuentemente produce listas de elementos:
* Lista de declaraciones: `int a, b, c;`
* Lista de sentencias: `{ stmt1; stmt2; ... }`
* Lista de argumentos: `foo(x, y + 1, z)`

Para representar sentencias o expresiones heterogéneas, se utiliza un nodo base genérico o una estructura con una unión discriminada (*tagged union*).

### 2. Ejemplo Práctico: Lista Enlazada de Parámetros y Sentencias

```c
/* ast_list.h */
#ifndef AST_LIST_H
#define AST_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR
} DataType;

/* Nodo para representar parámetros formales de una función: (int x, float y) */
typedef struct ParamNode {
    char *name;
    DataType type;
    struct ParamNode *next;
} ParamNode;

ParamNode* param_create(const char *name, DataType type);
ParamNode* param_append(ParamNode *head, ParamNode *new_param);
void param_list_free(ParamNode *head);
void param_list_print(const ParamNode *head);

#endif
```

```c
/* ast_list.c */
#include "ast_list.h"

ParamNode* param_create(const char *name, DataType type) {
    ParamNode *node = (ParamNode *)malloc(sizeof(ParamNode));
    if (!node) exit(EXIT_FAILURE);
    node->name = strdup(name);
    node->type = type;
    node->next = NULL;
    return node;
}

/* En Bison es comun construir listas encadenando hacia la izquierda o derecha:
   param_list: param_list ',' param { $$ = param_append($1, $3); } \vert{} param                {$$ = $1; }
*/
ParamNode* param_append(ParamNode *head, ParamNode *new_param) {
    if (!head) return new_param;
    ParamNode *curr = head;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = new_param;
    return head;
}

const char* type_to_string(DataType type) {
    switch(type) {
        case TYPE_INT:   return "int";
        case TYPE_FLOAT: return "float";
        case TYPE_CHAR:  return "char";
        default:         return "unknown";
    }
}

void param_list_print(const ParamNode *head) {
    const ParamNode *curr = head;
    printf("(");
    while (curr) {
        printf("%s %s", type_to_string(curr->type), curr->name);
        if (curr->next) printf(", ");
        curr = curr->next;
    }
    printf(")\n");
}

void param_list_free(ParamNode *head) {
    while (head) {
        ParamNode *next = head->next;
        free(head->name);
        free(head);
        head = next;
    }
}

/* Compilacion: gcc -Wall -Wextra -std=c11 ast_list.c -o ast_list */
int main(void) {
    ParamNode *params = param_create("x", TYPE_INT);
    params = param_append(params, param_create("y", TYPE_FLOAT));
    params = param_append(params, param_create("z", TYPE_INT));

    printf("Firma de funcion generada: ");
    param_list_print(params);

    param_list_free(params);
    return 0;
}
```

### 3. Ejercicio de Programación
* **Problema:** Implementa una función de validación semántica `int param_check_duplicates(const ParamNode *head)` que recorra la lista y devuelva `1` si hay identificadores duplicados (ejemplo: `(int x, float x)`), o `0` si todos los nombres son únicos.

---

## Lección 3: Tabla de Símbolos con Encadenamiento y Ámbitos (Nivel Avanzado)

### 1. Concepto: Tablas de Símbolos Jerárquicas
En el análisis semántico es obligatorio validar tipos, visibilidad y asignación de memoria (*offsets*). Para ello se usa una Tabla de Símbolos.

Se implementa comúnmente con dos niveles de listas enlazadas:
* **Encadenamiento interno (Hash Table Bucket List):** Maneja colisiones de nombres de variables dentro de un mismo ámbito.
* **Encadenamiento de ámbitos (Scope Chain / Parent Pointer):** Permite anidar bloques de código `{ ... }`. Al buscar un símbolo, se busca en el ámbito local; si no se encuentra, se sigue el enlace al ámbito padre (*enclosing scope*).

### 2. Ejemplo Práctico: Tabla de Símbolos en C++

```cpp
// symbol_table.hpp
#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <iostream>
#include <string>
#include <memory>
#include <vector>

enum class SymbolType { INT, FLOAT, VOID, FUNCTION };

struct Symbol {
    std::string name;
    SymbolType type;
    int memory_offset; // Offset relativo en el stack frame
    std::shared_ptr<Symbol> next_collision; // Lista de colisiones

    Symbol(const std::string& n, SymbolType t, int offset)
        : name(n), type(t), memory_offset(offset), next_collision(nullptr) {}
};

class Scope {
private:
    static const size_t BUCKET_SIZE = 16;
    std::vector<std::shared_ptr<Symbol>> buckets;
    std::shared_ptr<Scope> parent_scope;
    int current_offset;

    size_t hash_function(const std::string& key) const {
        size_t hash = 5381;
        for (char c : key) hash = ((hash << 5) + hash) + c; // djb2
        return hash % BUCKET_SIZE;
    }

public:
    explicit Scope(std::shared_ptr<Scope> parent = nullptr)
        : buckets(BUCKET_SIZE, nullptr), parent_scope(parent), current_offset(0) {}

    bool insert(const std::string& name, SymbolType type, int size_in_bytes) {
        size_t index = hash_function(name);
        std::shared_ptr<Symbol> curr = buckets[index];

        // Verificar si ya existe en el ámbito local
        while (curr != nullptr) {
            if (curr->name == name) {
                return false; // Error semantico: Redeclaracion
            }
            curr = curr->next_collision;
        }

        // Insercion al frente de la lista enlazada (bucket)
        auto new_sym = std::make_shared<Symbol>(name, type, current_offset);
        new_sym->next_collision = buckets[index];
        buckets[index] = new_sym;

        current_offset += size_in_bytes;
        return true;
    }

    std::shared_ptr<Symbol> lookup(const std::string& name) {
        // 1. Buscar en el ambito actual
        size_t index = hash_function(name);
        std::shared_ptr<Symbol> curr = buckets[index];
        while (curr != nullptr) {
            if (curr->name == name) return curr;
            curr = curr->next_collision;
        }

        // 2. Si no se encuentra, ascender en la jerarquia de ambitos
        if (parent_scope != nullptr) {
            return parent_scope->lookup(name);
        }

        return nullptr; // No declarado
    }

    std::shared_ptr<Scope> get_parent() const { return parent_scope; }
    int get_scope_size() const { return current_offset; }
};

#endif
```

```cpp
// main.cpp
#include "symbol_table.hpp"

// Compilacion: g++ -Wall -Wextra -std=c++17 main.cpp -o symtab
int main() {
    // Ambito Global
    auto global_scope = std::make_shared<Scope>(nullptr);
    global_scope->insert("global_var", SymbolType::INT, 4);

    // Entramos a un ambito local (ej. funcion foo)
    auto function_scope = std::make_shared<Scope>(global_scope);
    function_scope->insert("local_x", SymbolType::FLOAT, 8);
    function_scope->insert("temp", SymbolType::INT, 4);

    // Entramos a un bloque anidado (ej. bloque if)
    auto block_scope = std::make_shared<Scope>(function_scope);
    block_scope->insert("block_var", SymbolType::INT, 4);

    // Pruebas de resolucion semantica
    std::cout << "--- Resolucion de Simbolos ---\n";

    std::string test_symbols[] = {"block_var", "local_x", "global_var", "no_existe"};

    for (const auto& sym_name : test_symbols) {
        auto found = block_scope->lookup(sym_name);
        if (found) {
            std::cout << "Simbolo [" << sym_name << "] encontrado. Offset: " 
                      << found->memory_offset << "\n";
        } else {
            std::cout << "Error Semantico: Simbolo [" << sym_name << "] no declarado.\n";
        }
    }

    // Comprobacion de redeclaracion en el mismo ambito
    bool redeclared = !block_scope->insert("block_var", SymbolType::INT, 4);
    if (redeclared) {
        std::cout << "Correcto: Se detecto redeclaracion invalida de 'block_var'.\n";
    }

    return 0;
}
```

### 3. Ejercicio de Programación
* **Problema:** En el análisis semántico de lenguajes como C, las variables declaradas dentro de un bloque ocultan las del bloque externo (*shadowing*).
  * Agrega al método `insert` una verificación que advierta si la variable local está haciendo *shadowing* sobre una variable existente en `parent_scope`.
  * Implementa una función `void print_scope_hierarchy(std::shared_ptr<Scope> current)` que imprima todos los símbolos accesibles desde el punto actual hasta la raíz global.

---

## Integración con Flex y Bison (Patrón Canónico)

Al conectar estas estructuras con flex y bison, las listas enlazadas suelen exponerse dentro de la unión semántica `%union`:

```bison
%union {
    int ival;
    char *sval;
    struct ParamNode *param_list;
    struct AstNode *ast_node;
}

%token <sval> IDENTIFIER
%type <param_list> param_list param

%%

param_list:
      param_list ',' param { $$ = param_append($1, $3); } \vert{} param                {$$ = $1; } ;  param: IDENTIFIER { $$ = param_create($1, TYPE_INT); free($1); }
    ;
%%
```

Esta técnica mantiene el código del analizador limpio, desacoplado y listo para la fase de generación de código intermedio (código de 3 direcciones / cuádruplos).
