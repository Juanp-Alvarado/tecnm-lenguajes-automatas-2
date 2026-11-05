## Lección 4 (Alternativa en C): Árboles de Análisis Semántico (AST) con Uniones Discriminadas

### 1. Concepto: Polimorfismo Manual en C
Como C no cuenta con clases, herencia ni punteros inteligentes (`std::unique_ptr`), simulamos el polimorfismo de la siguiente manera:
1. **Un `enum` de tipos (`NodeType`):** Permite saber en tiempo de ejecución qué tipo de estructura contiene la unión.
2. **Una estructura con una `union` (`ASTNode`):** Contiene los campos específicos según el tipo de nodo (por ejemplo, un valor numérico si es una hoja, o punteros a subárboles izquierdo y derecho si es una operación binaria).
3. **Liberación recursiva (`ast_free`):** Dado que C no gestiona la memoria automáticamente, debemos recorrer el árbol liberando cada nodo de abajo hacia arriba para evitar fugas de memoria.

### 2. Ejemplo Práctico: AST de Expresiones Aritméticas en C

```c
/* ast_expr.h */
#ifndef AST_EXPR_H
#define AST_EXPR_H

typedef enum {
    NODE_NUMBER,
    NODE_BINARY_OP
} NodeType;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV
} OpType;

// Declaración adelantada
typedef struct ASTNode ASTNode;

struct ASTNode {
    NodeType type;
    union {
        double number_value; // Usado si type == NODE_NUMBER
        struct {
            OpType op;
            ASTNode *left;
            ASTNode *right;
        } binary;            // Usado si type == NODE_BINARY_OP
    } data;
};

/* Prototipos */
ASTNode* ast_create_number(double value);
ASTNode* ast_create_binary(OpType op, ASTNode *left, ASTNode *right);
double ast_evaluate(const ASTNode *node);
void ast_free(ASTNode *node);

#endif
```

```c
/* ast_expr.c */
#include "ast_expr.h"
#include <stdio.h>
#include <stdlib.h>

ASTNode* ast_create_number(double value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) {
        perror("Error asignando memoria para ASTNode numérico");
        exit(EXIT_FAILURE);
    }
    node->type = NODE_NUMBER;
    node->data.number_value = value;
    return node;
}

ASTNode* ast_create_binary(OpType op, ASTNode *left, ASTNode *right) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) {
        perror("Error asignando memoria para ASTNode binario");
        exit(EXIT_FAILURE);
    }
    node->type = NODE_BINARY_OP;
    node->data.binary.op = op;
    node->data.binary.left = left;
    node->data.binary.right = right;
    return node;
}

double ast_evaluate(const ASTNode *node) {
    if (!node) return 0.0;

    switch (node->type) {
        case NODE_NUMBER:
            return node->data.number_value;

        case NODE_BINARY_OP: {
            double l_val = ast_evaluate(node->data.binary.left);
            double r_val = ast_evaluate(node->data.binary.right);

            switch (node->data.binary.op) {
                case OP_ADD: return l_val + r_val;
                case OP_SUB: return l_val - r_val;
                case OP_MUL: return l_val * r_val;
                case OP_DIV:
                    if (r_val == 0.0) {
                        fprintf(stderr, "Error Semantico: Division por cero detectada\n");
                        exit(EXIT_FAILURE);
                    }
                    return l_val / r_val;
            }
        }
    }
    return 0.0;
}

void ast_free(ASTNode *node) {
    if (!node) return;

    // Si es un nodo binario, liberamos recursivamente sus hijos primero
    if (node->type == NODE_BINARY_OP) {
        ast_free(node->data.binary.left);
        ast_free(node->data.binary.right);
    }
    free(node);
}

/* Compilacion: gcc -Wall -Wextra -std=c11 ast_expr.c -o ast_demo */
int main(void) {
    // Construimos el AST equivalente a la expresión: (3 + 5) * 2
    ASTNode *expr = ast_create_binary(
        OP_MUL,
        ast_create_binary(
            OP_ADD,
            ast_create_number(3.0),
            ast_create_number(5.0)
        ),
        ast_create_number(2.0)
    );

    // Evaluación del árbol sintáctico
    double resultado = ast_evaluate(expr);
    printf("Resultado de la evaluacion del AST en C: %.2f\n", resultado); // Debería dar 16.00

    // Liberación estricta de memoria para evitar fugas (muy importante en C)
    ast_free(expr);

    return 0;
}
```

### 3. ¿Por qué este enfoque es ideal para C y para herramientas como Bison?
* **Compatibilidad nativa con Bison/Yacc:** Las acciones semánticas en Bison (`$$ = ...`) devuelven punteros genéricos (`void*` o tipos definidos en `%union`), por lo que estructurar los nodos con `struct` y `union` encaja de forma perfecta en la pila de análisis sintáctico de C.
* **Control total del rendimiento:** Al no haber sobrecarga de clases virtuales de C++, el uso de memoria es compacto, plano y predecible.

### 4. Ejercicio Práctico para los Alumnos
* **Problema:** Extiende la estructura del nodo añadiendo un nuevo tipo `NODE_VARIABLE` que almacene el nombre de una variable (`char *name`). Modifica la función `ast_evaluate` para que reciba como parámetro adicional una tabla simple de variables (por ejemplo, una estructura clave-valor estática) y devuelva el valor numérico correspondiente al buscar la variable.
