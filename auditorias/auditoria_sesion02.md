# Bitácora de Requerimientos y Auditoría IA — Sesión 02
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Gestión en GitHub Web y Nodos Dinámicos  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo  
**Matrícula:** 24531368  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Lista enlazada de tokens en Heap  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Durante la codificación manual, fue fundamental asegurar la correcta asignación de memoria dinámica mediante malloc y manejar adecuadamente el límite del arreglo estático char lexema[16] en la estructura NodoToken. Se identificó la necesidad de usar strncpy junto con un truncamiento explícito con el carácter nulo (\0) para prevenir desbordamientos de búfer (buffer overflows). Asimismo, al implementar liberar_lista, se cuidó almacenar la referencia al siguiente nodo antes de invocar free(actual) para evitar errores de acceso a memoria liberada (use-after-free).

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Gestión de memoria dinámica C99/Rust |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
[Contexto] Trabajas en un entorno C99 estándar utilizando GCC, sin librerías externas o no estándar.
[Rol] Ingeniero de software de sistemas y compiladores experto en gestión de memoria dinámica en C.
[Especificación] Implementa una lista enlazada de tokens en el heap mediante las funciones: crear_nodo, agregar_token, contar_tokens y liberar_lista, respetando estrictamente la estructura NodoToken definida en el contrato (con un arreglo char lexema[16] y un puntero *siguiente). El código debe ser robusto ante casos límite como punteros NULL y evitar fugas de memoria o use-after-free.
[Output] Proporciona únicamente el código fuente modular en C sin la función main().
```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion02.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

NodoToken *crear_nodo(const char *lexema) {
    NodoToken *nuevo = (NodoToken *)malloc(sizeof(NodoToken));
    if (nuevo == NULL) {
        return NULL;
    }
    strncpy(nuevo->lexema, lexema, sizeof(nuevo->lexema) - 1);
    nuevo->lexema[sizeof(nuevo->lexema) - 1] = '\0';
    nuevo->siguiente = NULL;
    return nuevo;
}

void agregar_token(NodoToken **cabeza, const char *lexema) {
    if (cabeza == NULL) return;

    NodoToken *nuevo = crear_nodo(lexema);
    if (nuevo == NULL) return;

    if (*cabeza == NULL) {
        *cabeza = nuevo;
    } else {
        NodoToken *actual = *cabeza;
        while (actual->siguiente != NULL) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevo;
    }
}

int contar_tokens(NodoToken *cabeza) {
    int contador = 0;
    NodoToken *actual = cabeza;
    while (actual != NULL) {
        contador++;
        actual = actual->siguiente;
    }
    return contador;
}

void liberar_lista(NodoToken *cabeza) {
    NodoToken *actual = cabeza;
    while (actual != NULL) {
        NodoToken *siguiente_nodo = actual->siguiente;
        free(actual);
        actual = siguiente_nodo;
    }
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Prevención de Use-After-Free y cadenas nulas

### Salida del harness
```text
=== AUDITORIA SESION 02 - Lista enlazada de tokens en heap ===
Caso                               esperado:              obtenido:              resultado
[ 1] crear_nodo("if")->lexema           esperado: if           obtenido: if           OK
[ 2] contar_tokens(3 tokens)            esperado: 3            obtenido: 3            OK
[ 3] orden: segundo token               esperado: dos          obtenido: dos          OK
[ 4] ultimo->siguiente == NULL          esperado: 1            obtenido: 1            OK
[ 5] contar_tokens(NULL)                esperado: 0            obtenido: 0            OK
[ 6] liberar_lista(NULL)                esperado: sin crash    obtenido: sin crash    OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion02.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> La IA generó una estructura funcional para el manejo del Heap, pero omitió inicialmente asegurar de forma defensiva la validación de punteros dobles nulos en agregar_token y el aseguramiento del carácter terminador \0 al usar strncpy. Esto se solucionó añadiendo explícitamente la condición de protección if (cabeza == NULL) return; y forzando el último byte del arreglo lexema a cero para garantizar la seguridad en cadenas que alcancen el límite de los 16 bytes.

---
**Firma digital:** ElJuan11
