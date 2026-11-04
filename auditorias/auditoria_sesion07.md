# Bitácora de Requerimientos y Auditoría IA — Sesión 07
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Evaluación de Expresiones Aritméticas  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo  
**Matrícula:** 24531368  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Evaluador recursivo de operaciones  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Al realizar el código manualmente, una de las principales dificultades fue comprender cómo recorrer el árbol utilizando recursividad y apuntadores. También fue necesario tener cuidado al comprobar si un nodo existía antes de acceder a sus datos, además de controlar la división entre cero para evitar errores durante la ejecución.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Manejo estricto de división entre cero |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
[C – Contexto]

Implementar una función evaluar(nodo, &error) en lenguaje C que recorra un árbol binario de expresiones y calcule su resultado.

[R – Rol]

Actúa como un ingeniero de compiladores especializado en C y estructuras de datos.

[E – Entregable]

Desarrollar la función recursiva evaluar() que realice operaciones aritméticas y controle los siguientes errores:

error = 0: Sin errores.

error = 1: División entre cero.

error = 2: Nodo inválido.

[O – Objetivo]

Obtener un evaluador funcional que devuelva el resultado correcto, identifique los errores y evite fallos durante la ejecución.
```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion07.h"
#include <stddef.h>

int evaluar(ASTNode *n, int *error) {
    if (error == NULL) {
        return 0;
    }
    
    /* Error 2: Nodo inválido (NULL) */
    if (n == NULL) {
        *error = 2;
        return 0;
    }

    /* Caso base: Nodo numérico */
    if (n->tipo == NODO_NUM) {
        return n->valor;
    }

    /* Caso recursivo: Operador binario */
    if (n->tipo == NODO_BINOP) {
        int izq = evaluar(n->izq, error);
        if (*error != 0) return 0; /* Propagación rápida de error */
        
        int der = evaluar(n->der, error);
        if (*error != 0) return 0; /* Propagación rápida de error */

        switch (n->op) {
            case '+': return izq + der;
            case '-': return izq - der;
            case '*': return izq * der;
            case '/':
                if (der == 0) {
                    *error = 1; /* División entre cero */
                    return 0;
                }
                return izq / der;
            case '%':
                if (der == 0) {
                    *error = 1; /* Módulo entre cero */
                    return 0;
                }
                return izq % der;
            default:
                *error = 2; /* Operador inválido */
                return 0;
        }
    }

    *error = 2; /* Tipo de nodo desconocido */
    return 0;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Expresiones complejas y división por 0

### Salida del harness
```text
=== AUDITORIA SESION 07 - Evaluador recursivo con deteccion de errores ===
Caso                               esperado:              obtenido:              resultado
[ 1] (2+3)*4                            esperado: 20           obtenido: 20           OK
[ 2] 7/0 -> error 1                     esperado: 1            obtenido: 1            OK
[ 3] 7%0 -> error 1                     esperado: 1            obtenido: 1            OK
[ 4] arbol NULL -> error 2              esperado: 2            obtenido: 2            OK
[ 5] 10-2-3                             esperado: 5            obtenido: 5            OK
[ 6] (8/2)+1                            esperado: 5            obtenido: 5            OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion07.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Inicialmente, la IA no contempló el operador módulo (%) al detectar divisiones entre cero. Esto provocaba que la prueba 7%0 devolviera el error 2 en lugar del error 1 esperado. Para solucionarlo, agregué el caso '%' dentro del switch y comprobé que el divisor fuera diferente de cero antes de realizar la operación. Después de este cambio, las seis pruebas se ejecutaron correctamente.

---
**Firma digital:** Juanp-Alvarado
