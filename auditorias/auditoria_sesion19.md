# Bitácora de Requerimientos y Auditoría IA — Sesión 19
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Estructuras (struct) y Miembros  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo  
**Matrícula:** 24531368  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Offsets de miembros en estructuras  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Al realizar el ejercicio me di cuenta de que calcular las posiciones de los campos de una estructura no es tan sencillo como sumar el tamaño de cada variable. También hay que tomar en cuenta la alineación de memoria, porque algunos tipos de datos necesitan comenzar en posiciones específicas. Esto puede provocar que existan espacios vacíos entre los campos, conocidos como padding. La dificultad fue calcular correctamente esos espacios y obtener la posición de cada miembro sin equivocarme en las operaciones.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Alineación de memoria por palabras |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
Prompt C-R-E-O 

[Contexto]
Estoy trabajando en un laboratorio de la materia Lenguajes y Autómatas II en lenguaje C (estándar C11). Necesito implementar un simulador que calcule los offsets de los campos de una estructura y su tamaño total final, simulando las reglas reales de alineación (alignment) y relleno (padding) de la arquitectura.

[Rol]
Actúa como un compilador de C experto y especialista en optimización de memoria de bajo nivel.

[Especificación]
Implementa las funciones declaradas en el archivo de cabecera `api_sesion19.h`:

1. `int offset_campo(const Campo *campos, int total, const char *nombre);`
2. `int tamano_struct(const Campo *campos, int total);

```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion19.h"
#include <string.h>
#include <stddef.h>

static int obtener_alineacion(const char *tipo) {
    if (tipo == NULL) return -1;
    if (strcmp(tipo, "char") == 0) return 1;
    if (strcmp(tipo, "short") == 0) return 2;
    if (strcmp(tipo, "int") == 0) return 4;
    if (strcmp(tipo, "double") == 0) return 8;
    return -1;
}

int offset_campo(const Campo *campos, int total, const char *nombre) {
    if (campos == NULL || total <= 0 || nombre == NULL) {
        return -1;
    }

    int offset_actual = 0;
    int encontrado = 0;
    int offset_resultado = -1;

    for (int i = 0; i < total; i++) {
        int alineacion = obtener_alineacion(campos[i].tipo);
        if (alineacion == -1) {
            return -1;
        }

        // Alinear el offset actual al requerimiento del campo
        if (offset_actual % alineacion != 0) {
            offset_actual += (alineacion - (offset_actual % alineacion));
        }

        if (strcmp(campos[i].nombre, nombre) == 0) {
            offset_resultado = offset_actual;
            encontrado = 1;
        }

        // Avanzar el offset según el tamaño del tipo
        offset_actual += alineacion;
    }

    return encontrado ? offset_resultado : -1;
}

int tamano_struct(const Campo *campos, int total) {
    if (total == 0) return 0;
    if (campos == NULL || total < 0) return -1;

    int offset_actual = 0;
    int max_alineacion = 1;

    for (int i = 0; i < total; i++) {
        int alineacion = obtener_alineacion(campos[i].tipo);
        if (alineacion == -1) {
            return -1;
        }

        if (alineacion > max_alineacion) {
            max_alineacion = alineacion;
        }

        // Alinear el offset actual
        if (offset_actual % alineacion != 0) {
            offset_actual += (alineacion - (offset_actual % alineacion));
        }

        offset_actual += alineacion;
    }

    // Padding final: redondear al múltiplo del miembro más grande
    if (offset_actual % max_alineacion != 0) {
        offset_actual += (max_alineacion - (offset_actual % max_alineacion));
    }

    return offset_actual;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Acceso a miembros inexistentes

### Salida del harness
```text
=== AUDITORIA SESION 19 - Offsets de miembros y tamano de struct ===
Caso                               esperado:              obtenido:              resultado
[ 1] struct char e int: b               esperado: 4            obtenido: 4            OK
[ 2] struct de tres campos: c           esperado: 8            obtenido: 8            OK
[ 3] tamano total con padding           esperado: 12           obtenido: 12           OK
[ 4] double seguido de char             esperado: 16           obtenido: 16           OK
[ 5] miembro inexistente                esperado: -1           obtenido: -1           OK
[ 6] struct sin campos                  esperado: 0            obtenido: 0            OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion19.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Al revisar el código identifiqué que era necesario considerar los casos en los que se busca un campo que no existe dentro de la estructura. Para solucionar esto, la función offset_campo() recorre todos los campos y, si no encuentra el nombre solicitado, regresa -1.

También se agregó una validación para comprobar que el nombre del campo no sea NULL antes de utilizar strcmp(), evitando realizar una comparación con un apuntador nulo.

---
**Firma digital:** Juanp-Alvarado
