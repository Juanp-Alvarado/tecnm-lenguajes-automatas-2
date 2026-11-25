# Bitácora de Requerimientos y Auditoría IA — Sesión 22
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Generación de TAC para Expresiones  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo  
**Matrícula:** 24531368  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Traductor de nodos AST a TAC  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Al realizar el ejercicio tuve dificultad para convertir los nodos del árbol AST en instrucciones de código de tres direcciones. Fue necesario entender cómo recorrer el árbol para que las operaciones se realizaran en el orden correcto.
También identifiqué que no se puede generar primero la operación principal cuando sus operandos todavía necesitan ser calculados. Por eso se utiliza el recorrido en postorden, donde primero se visitan los hijos y después se genera la operación del nodo actual.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Conservación de precedencia de operadores |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
### Contrato C-R-E-O 

```markdown
**Contexto**:
Estoy trabajando en la Sesión 22 del curso de Lenguajes y Autómatas II, traduciendo un Árbol de Sintaxis Abstracta (AST) de expresiones aritmético-lógicas a Código de Tres Direcciones (TAC). Tengo un entorno en C y dispongo del archivo de cabecera `api_sesion22.h`.

**Rol**:
Actúa como un Ingeniero de Compiladores experto en C, especializado en generación de código intermedio y optimización de ASTs.

**Especificación**:
Debes implementar la función `int generar_tac(const Nodo *raiz, char lineas[][64], int max)` en C.
1. Realiza un recorrido en postorden (primero procesa el subárbol izquierdo, luego el derecho, y finalmente el nodo actual).
2. El nodo puede ser una hoja (tipo `'H'`) o un operador binario (tipo `'B'`).
3. Las hojas no emiten líneas TAC por sí solas, pero su valor o el temporal resultante debe retornarse para ser usado por sus padres.
4. Para cada operador binario (como `+`, `-`, `*`), genera una nueva línea de código de tres direcciones con el formato `"_tX = operando_izq op operando_der"` empleando un pool de temporales que se incremente de manera secuencial (`_t0`, `_t1`, etc.).
5. Copia cada instrucción TAC generada de forma segura en `lineas[n]` sin superar `max` líneas totales, asegurando no desbordar los 63 caracteres máximos por línea.
6. Retorna el número total de líneas TAC escritas.
7. La firma debe coincidir exactamente con la definida en `api_sesion22.h`. No incluyas una función `main`.

**Output**:
Genera únicamente el código C limpio, optimizado y libre de advertencias (`-Wall -Wextra -Werror`).
```

---

## 3. Código Devuelto por el Modelo
```c
/* ia_sesion22_c.c - Fase 2: El Cientifico
 *
 * Solución óptima generada que realiza recorrido en postorden
 * y genera la tabla de tres direcciones (TAC) de forma secuencial.
 */
#include "api_sesion22.h"
#include <stdio.h>
#include <string.h>

/* Pool para gestionar los temporales asignados de manera consecutiva */
typedef struct {
    char nombre[64][16];
    int usados;
} Pool;

static const char *nuevo_temporal(Pool *p, int *contador) {
    if (p->usados >= 64) return "_t?";
    snprintf(p->nombre[p->usados], sizeof p->nombre[0], "_t%d", (*contador)++);
    return p->nombre[p->usados++];
}

/* Función auxiliar recursiva para recorrer en postorden */
static const char *recorrer_ast(const Nodo *n, char lineas[][64], int max, int *lineas_emitidas, Pool *p, int *contador) {
    if (!n) return "";

    if (n->tipo == 'H') {
        return n->valor;
    }

    /* Recorrido Postorden: Primero hijos, luego padre */
    const char *izq_val = recorrer_ast(n->izq, lineas, max, lineas_emitidas, p, contador);
    const char *der_val = recorrer_ast(n->der, lineas, max, lineas_emitidas, p, contador);

    if (*lineas_emitidas < max) {
        const char *temp = nuevo_temporal(p, contador);
        snprintf(lineas[*lineas_emitidas], 64, "%s = %s %c %s", temp, izq_val, n->op, der_val);
        (*lineas_emitidas)++;
        return temp;
    }

    return "_t?";
}

int generar_tac(const Nodo *raiz, char lineas[][64], int max) {
    if (!raiz) return 0;
    
    Pool p;
    p.usados = 0;
    int contador = 0;
    int lineas_emitidas = 0;

    recorrer_ast(raiz, lineas, max, &lineas_emitidas, &p, &contador);
    return lineas_emitidas;
}

```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Expresiones anidadas con paréntesis

### Salida del harness
```text
=== AUDITORIA SESION 22 - Traductor de AST a TAC con precedencia ===
Caso                               esperado:              obtenido:              resultado
[ 1] a + b * c                          esperado: _t0 = b * c  obtenido: _t0 = b * c  OK
[ 2] a + b * c (2a linea)
     esperado: _t1 = a + _t0
     obtenido: _t1 = a + _t0
     -> OK
[ 3] (a + b) * c
     esperado: _t1 = _t0 * c
     obtenido: _t1 = _t0 * c
     -> OK
[ 4] x * (y - z) + w
     esperado: _t2 = _t1 + w
     obtenido: _t2 = _t1 + w
     -> OK
[ 5] hoja sin operaciones               esperado: 0            obtenido: 0            OK
[ 6] (a + b) * (c - d)
     esperado: _t2 = _t0 * _t1
     obtenido: _t2 = _t0 * _t1
     -> OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion22.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Al revisar el código identifiqué que era importante considerar los árboles que no tienen una raíz válida y los casos donde el arreglo de instrucciones no tiene espacio suficiente.
Para solucionar esto, agregué validaciones al inicio de la función generar_tac() para comprobar que la raíz y el arreglo de salida sean válidos y que exista espacio para generar instrucciones.

---
**Firma digital:** Juanp-Alvarado
