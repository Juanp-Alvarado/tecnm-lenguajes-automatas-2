# Bitácora de Requerimientos y Auditoría IA — Sesión 08
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Inferencia y Promoción de Tipos  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo  
**Matrícula:** 24531368  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Reglas de casteo implícito  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Al realizar el código manualmente, identifiqué que uno de los principales problemas era comparar correctamente los tipos de datos para determinar cuál tenía mayor capacidad. También fue necesario considerar los casos donde alguno de los tipos fuera TIPO_ERROR, ya que no debía realizarse una promoción cuando existiera un error. Otra dificultad fue identificar cuándo se podía perder información al convertir un tipo de dato a otro más pequeño.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Promoción entera y ensanchamiento |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
Contexto (C): Clase de Lenguajes y Autómatas II, Sesión 08 sobre Inferencia y Promoción de Tipos en C.

Rol (R): Experto en diseño de compiladores e ingeniería de software en C.

Especificación (E): Implementar las reglas de promoción (CHAR < INT < FLOAT; si hay ERROR, retorna ERROR), y detección de pérdida de información (hay pérdida si el destino es más angosto que el origen).

Output (O): Archivo ia_sesion08_c.c sin función main() que cumpla estrictamente con api_sesion08.h y compile con -Wall -Wextra -Werror.
```

---

## 3. Código Devuelto por el Modelo
```c
%%writefile ia_sesion08_c.c
/* ia_sesion08_c.c - Fase 2: El Cientifico
 *
 * Implementacion de reglas de promocion e inferencia de tipos
 * basada en el contrato api_sesion08.h
 */
#include "api_sesion08.h"
#include <stdio.h>

/*
 * Jerarquia de tipos: TIPO_ERROR (0), TIPO_CHAR (1), TIPO_INT (2), TIPO_FLOAT (3)
 * promover: Devuelve el tipo de mayor rango. Si alguno es TIPO_ERROR, devuelve TIPO_ERROR.
 */
Tipo promover(Tipo a, Tipo b) {
    if (a == TIPO_ERROR || b == TIPO_ERROR) {
        return TIPO_ERROR;
    }
    return (a > b) ? a : b;
}

/*
 * hay_perdida: Retorna 1 si el tipo destino es estrictamente mas angosto
 * que el tipo de origen (ej. pasar de INT a CHAR, o de FLOAT a INT).
 */
int hay_perdida(Tipo destino, Tipo origen) {
    if (destino == TIPO_ERROR || origen == TIPO_ERROR) {
        return 0; 
    }
    return (destino < origen) ? 1 : 0;
}

/*
 * nombre_tipo: Traduce el enum de Tipo a una cadena legible para el harness.
 */
const char *nombre_tipo(Tipo t) {
    switch (t) {
        case TIPO_CHAR:
            return "char";
        case TIPO_INT:
            return "int";
        case TIPO_FLOAT:
            return "float";
        case TIPO_ERROR:
        default:
            return "error";
    }
}

```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Mezcla de tipos enteros y flotantes

### Salida del harness
```text
=== AUDITORIA SESION 08 - Reglas de promocion y perdida ===
Caso                               esperado:              obtenido:              resultado
[ 1] promover(CHAR,INT)                 esperado: int          obtenido: int          OK
[ 2] promover(INT,FLOAT)                esperado: float        obtenido: float        OK
[ 3] promover(FLOAT,FLOAT)              esperado: float        obtenido: float        OK
[ 4] promover(ERROR,INT)                esperado: error        obtenido: error        OK
[ 5] hay_perdida(CHAR,INT)              esperado: 1            obtenido: 1            OK
[ 6] hay_perdida(INT,CHAR)              esperado: 0            obtenido: 0            OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion08.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Durante la revisión del código identifiqué que era importante considerar los casos donde alguno de los tipos recibidos fuera TIPO_ERROR. También se debía contemplar cualquier valor que no estuviera definido dentro del enum al utilizar la función nombre_tipo.

Para solucionar estos casos agregué condiciones if para identificar los errores y un default dentro del switch para devolver "error" cuando se recibe un valor no reconocido. También simplifiqué las comparaciones utilizando if y else para que el código fuera más fácil de entender.

Finalmente, comprobé que las funciones utilizan la jerarquía numérica de los tipos para realizar las promociones y detectar posibles pérdidas de información.

---
**Firma digital:** Juanp-Alvarado
