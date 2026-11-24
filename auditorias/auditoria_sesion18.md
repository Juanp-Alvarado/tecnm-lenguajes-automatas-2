# Bitácora de Requerimientos y Auditoría IA — Sesión 18
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Arreglos y Verificación de Límites  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo  
**Matrícula:** 24531368  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Cálculo de tamaño de arreglo en memoria  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Al realizar el código manualmente, tuve dificultad para calcular correctamente el espacio que ocupa un arreglo dependiendo de su tipo de dato. También identifiqué que era necesario validar la cantidad de elementos para evitar valores negativos y comprobar que los índices no sobrepasaran los límites del arreglo. Otro detalle importante fue considerar los tipos de datos permitidos y devolver un valor de error cuando se recibe un tipo desconocido. La condición más importante fue respetar que el índice debe comenzar en cero y ser menor que la cantidad total de elementos.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Indexación entera no negativa |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
### Prompt C-R-E-O para la IA

```markdown
**Contexto (C):** Estoy trabajando en un laboratorio de compiladores (Lenguajes y Autómatas II) sobre la verificación de límites estáticos y cálculo de tamaños de arreglos en memoria.

**Rol (R):** Actúa como un Ingeniero de Software experto en C y sistemas embebidos, enfocado en código seguro y sin advertencias de compilación (-Wall -Wextra -Werror).

**Especificación (E):** Necesito implementar dos funciones declaradas en `api_sesion18.h`:

1. `long bytes_arreglo(const char *tipo, int elementos);`
   - Debe calcular el tamaño en bytes multiplicando `elementos` por el tamaño del tipo de dato indicado.
   - Los tipos válidos son: "char" (1 byte), "short" (2 bytes), "int" (4 bytes), "float" (4 bytes), "double" (8 bytes) y "puntero" (8 bytes en arquitectura de 64 bits).
   - Si el tipo no es uno de estos, debe retornar -1.
   - Si el número de elementos es negativo, debe retornar -1. Si es cero, debe retornar 0.

2. `int indice_en_rango(int indice, int elementos);`
   - Debe validar si un índice está dentro de los límites del arreglo.
   - Retorna 1 si `0 <= indice < elementos`. En cualquier otro caso (índices negativos o mayores/iguales al límite), retorna 0.

**Output (O):** Devuelve únicamente el código fuente en C listo para guardar en `ia_sesion18_c.c`. Incluye `#include "api_sesion18.h"`, `#include <string.h>` y las funciones completas. No incluyas la función `main()`.

```

---

## 3. Código Devuelto por el Modelo
```c
%%writefile ia_sesion18_c.c
/* ia_sesion18_c.c - Fase 2: El Cientifico
 * 
 * Implementación generada a partir del contrato C-R-E-O.
 */
#include "api_sesion18.h"
#include <string.h>

long bytes_arreglo(const char *tipo, int elementos) {
    if (elementos < 0) {
        return -1;
    }
    if (elementos == 0) {
        return 0;
    }
    if (tipo == NULL) {
        return -1;
    }

    long tamano_tipo = -1;
    if (strcmp(tipo, "char") == 0) {
        tamano_tipo = 1;
    } else if (strcmp(tipo, "short") == 0) {
        tamano_tipo = 2;
    } else if (strcmp(tipo, "int") == 0) {
        tamano_tipo = 4;
    } else if (strcmp(tipo, "float") == 0) {
        tamano_tipo = 4;
    } else if (strcmp(tipo, "double") == 0) {
        tamano_tipo = 8;
    } else if (strcmp(tipo, "puntero") == 0) {
        tamano_tipo = 8; /* Arquitectura de 64 bits estándar */
    } else {
        return -1;
    }

    return tamano_tipo * elementos;
}

int indice_en_rango(int indice, int elementos) {
    if (indice >= 0 && indice < elementos) {
        return 1;
    }
    return 0;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Límites estáticos y dimensiones inválidas

### Salida del harness
```text
=== AUDITORIA SESION 18 - Tamano de arreglo y limites ===
Caso                               esperado:              obtenido:              resultado
[ 1] int[10]                            esperado: 40           obtenido: 40           OK
[ 2] double[3]                          esperado: 24           obtenido: 24           OK
[ 3] int[-4]                            esperado: -1           obtenido: -1           OK
[ 4] indice 0 de 5                      esperado: 1            obtenido: 1            OK
[ 5] indice 5 de 5                      esperado: 0            obtenido: 0            OK
[ 6] indice negativo                    esperado: 0            obtenido: 0            OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion18.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Aunque la implementación superó las seis pruebas del harness, identifiqué que no se estaban comprobando todos los casos posibles. Por ejemplo, no se incluyeron pruebas para un tipo de dato nulo, un tipo desconocido, una cantidad de elementos igual a cero o los tipos short, char y puntero.

También observé que la multiplicación para calcular los bytes no contempla una validación explícita de desbordamiento de tipo long. Para mejorar la seguridad, se puede utilizar LONG_MAX y comprobar que la cantidad de elementos no supere el valor máximo permitido antes de realizar la multiplicación.



---
**Firma digital:** Juanp-Alvarado
