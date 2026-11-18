# Bitácora de Requerimientos y Auditoría IA — Sesión 15
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Bloques Anidados y Ciclo de Vida  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo  
**Matrícula:** 24531368  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Operaciones Push y Pop de ámbito  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Al implementar las operaciones Push y Pop, identifiqué que una de las principales dificultades fue controlar correctamente los punteros de los ámbitos. Al crear un nuevo bloque era necesario reservar memoria dinámica y enlazarlo con su ámbito padre. También tuve que considerar que, al cerrar un bloque, solamente se debe liberar la memoria del ámbito actual sin afectar los anteriores. Otro problema fue evitar errores cuando se reciben punteros NULL o cuando se intenta salir del ámbito raíz.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Liberación de memoria al cerrar bloque |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
### Prompt C-R-E-O 

**Contexto:** Estoy desarrollando un analizador semántico en C para un compilador en la materia de Lenguajes y Autómatas II. Necesito implementar la gestión de una pila de ámbitos enlazados para controlar bloques anidados y el ciclo de vida de los identificadores.

**Rol:** Actúa como un programador experto en lenguaje C, con un fuerte enfoque en buenas prácticas de gestión de memoria, robustez ante punteros nulos y optimización de código.

**Especificación:** 
Implementa las funciones descritas en el siguiente contrato header (`api_sesion15.h`):


typedef struct Ambito {
    const char *nombre;
    struct Ambito *padre;
} Ambito;

Ambito *push_ambito(Ambito *actual, const char *nombre);
Ambito *pop_ambito(Ambito *actual);


**Reglas de comportamiento:**
1. `push_ambito` debe reservar memoria dinámica (`malloc`) para una nueva estructura `Ambito`.
2. Si `actual` es `NULL` o `nombre` es `NULL`, `push_ambito` debe retornar `NULL` de inmediato (sin reservar memoria).
3. El nuevo ámbito debe quedar enlazado con su `padre` (que es el ámbito `actual` recibido) y almacenar el puntero al `nombre`.
4. `pop_ambito` debe liberar únicamente la memoria del ámbito `actual` provisto, pero antes de liberarla, debe recuperar y retornar el puntero a su ámbito `padre`.
5. Si el ámbito recibido en `pop_ambito` tiene un `padre` que es `NULL` (es decir, es la raíz), o si el puntero `actual` en sí es `NULL`, la función debe retornar `NULL` de manera segura sin causar fugas ni liberar memoria que no le corresponde (o si es la raíz física de prueba asignada de forma estática en el arnés, evitar liberarla).
6. No escribas una función `main()`. El código debe compilar con `-Wall -Wextra -Werror` de forma limpia.

**Output:** Devuelve únicamente el código de implementación en C listo para ser insertado en `ia_sesion15_c.c`, incluyendo las cabeceras estándar necesarias (como `<stdlib.h>`).

```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion15.h"
#include <stdlib.h>

Ambito *push_ambito(Ambito *actual, const char *nombre) {
    if (actual == NULL || nombre == NULL) {
        return NULL;
    }

    Ambito *nuevo = (Ambito *)malloc(sizeof(Ambito));
    if (nuevo == NULL) {
        return NULL;
    }

    nuevo->nombre = nombre;
    nuevo->padre = actual;

    return nuevo;
}

Ambito *pop_ambito(Ambito *actual) {
    if (actual == NULL || actual->padre == NULL) {
        return NULL;
    }

    Ambito *padre = actual->padre;
    free(actual);
    return padre;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Aislamiento de variables fuera de bloque

### Salida del harness
```text
=== AUDITORIA SESION 15 - Push y pop de ambitos ===
Caso                               esperado:              obtenido:              resultado
[ 1] nombre del ambito nuevo            esperado: if           obtenido: if           OK
[ 2] profundidad con dos push           esperado: 3            obtenido: 3            OK
[ 3] pop devuelve el padre              esperado: 1            obtenido: 1            OK
[ 4] pop sobre la raiz                  esperado: 1            obtenido: 1            OK
[ 5] push con padre NULL                esperado: 1            obtenido: 1            OK
[ 6] profundidad tras pop               esperado: 2            obtenido: 2            OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion15.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> La IA resolvió correctamente las operaciones Push y Pop, pero una situación importante que debía considerarse era el manejo del ámbito raíz. Si se libera este ámbito, podría provocar un error porque puede tratarse de una estructura asignada de forma estática por el programa de pruebas. Para evitarlo, se agregó una condición que verifica si el ámbito actual es NULL o si no tiene un padre antes de liberar memoria. De esta manera se protege el ámbito raíz y se evita liberar memoria que no corresponde. Las seis pruebas del harness confirmaron que el funcionamiento es correcto.

---
**Firma digital:** Juanp-Alvarado
