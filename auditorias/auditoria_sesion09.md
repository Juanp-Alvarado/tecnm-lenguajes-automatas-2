# Bitácora de Requerimientos y Auditoría IA — Sesión 09
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Reporte Formal de Errores Semánticos  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo  
**Matrícula:** 24531368  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Diagnóstico con línea y columna  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Al realizar esta actividad identifiqué que una de las dificultades era acomodar correctamente la información del error, ya que se necesitaba mostrar el nombre del archivo, la línea, la columna y el mensaje en un mismo texto.

También debía tener cuidado con el espacio disponible en la variable de salida, porque si el mensaje era más largo que el espacio reservado podía ocasionar problemas de memoria. Por eso fue necesario utilizar una función que permitiera controlar la cantidad de caracteres que se escriben.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Estructura estándar de errores GCC |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
[C] Contexto: Módulo de diagnóstico para un
compilador en C.

[R] Rol: Programador experto en C preocupado por la seguridad de memoria.

[E] Especificación:
Implementa:
void reportar(const char *archivo, int linea, int columna, const char *mensaje, char *salida, int cap);
Formato: "%s:%d:%d: error: %s"
Seguridad: Usa snprintf para prevenir desbordamientos y asegurar que 'salida' siempre termine en '\0' truncando de forma segura si cap es pequeño.

[O] Output: Código C limpio, sin main().
```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion09.h"
#include <stdio.h>

void reportar(const char *archivo, int linea, int columna, const char *mensaje, char *salida, int cap) {
    if (salida == NULL || cap <= 0) {
        return;
    }
    /* snprintf garantiza que escribe como máximo cap-1 caracteres y añade siempre '\0' */
    snprintf(salida, (size_t)cap, "%s:%d:%d: error: %s", archivo, linea, columna, mensaje);
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Múltiples errores sin abortar parser

### Salida del harness
```text
Al realizar esta actividad identifiqué que una de las dificultades era acomodar correctamente la información del error, ya que se necesitaba mostrar el nombre del archivo, la línea, la columna y el mensaje en un mismo texto.

También debía tener cuidado con el espacio disponible en la variable de salida, porque si el mensaje era más largo que el espacio reservado podía ocasionar problemas de memoria. Por eso fue necesario utilizar una función que permitiera controlar la cantidad de caracteres que se escriben.
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Al revisar el código encontré que se comprobaba si la variable salida era NULL o si su capacidad era menor o igual a cero. Sin embargo, no se consideraba qué pasaría si los parámetros archivo o mensaje fueran NULL.

Esto podría ocasionar problemas al intentar utilizarlos con el formato %s de snprintf. Para evitarlo, se pueden agregar comprobaciones y asignar un texto predeterminado cuando alguno de esos datos no esté disponible.

---
**Firma digital:** Juanp-Alvarado
