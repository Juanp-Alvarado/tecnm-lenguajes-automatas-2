# Bitácora de Requerimientos y Auditoría IA — Sesión 17
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Comprobación Semántica de Retornos  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo  
**Matrícula:** 24531368  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Validación de tipo de sentencia return  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Al realizar el código manualmente tuve algunas dificultades para comparar los tipos de datos que recibe una función, ya que estos se manejan como cadenas de texto y fue necesario utilizar strcmp para poder compararlos. También tuve que considerar los casos donde una función es de tipo void y no necesita regresar un valor. Otro problema fue revisar que todas las rutas de ejecución de una función tengan su correspondiente return, evitando aceptar funciones que puedan terminar sin regresar un resultado.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Rutas de ejecución con retorno garantizado |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
Contexto (C): Analizador semántico en C (C11) que valida tipos de retorno y flujo de control.

Rol (R): Ingeniero de Compiladores Senior.

Especificación (E): Implementa int diagnosticar_retorno(const char *tipo_funcion, const char *tipo_expresion, int rutas_totales, int rutas_con_return) bajo estas reglas:

Errores (-1): Si algún tipo es NULL, rutas_totales <= 0, rutas_con_return < 0 o rutas_con_return > rutas_totales.
Incompatibilidad (1): Si el tipo es incompatible. Evaluado antes que las rutas.
void solo acepta void (otro tipo retorna 1).
Tipos idénticos son compatibles.
Conversión implícita permitida: destino double acepta origen int, pero destino int NO acepta double.
Ruta sin retorno (2): Si los tipos son compatibles pero rutas_con_return < rutas_totales (excepto si la función es void).
Correcto (0): Si no hay errores de tipo y todas las rutas retornan.
Output (O): Devuelve solo el código C de la función, listo para -Wall -Wextra -Werror sin main().
```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion17.h"
#include <string.h>
#include <stddef.h>

int diagnosticar_retorno(const char *tipo_funcion, const char *tipo_expresion, int rutas_totales, int rutas_con_return) {
    /* 1. Validación de parámetros de entrada e incoherencias básicas */
    if (tipo_funcion == NULL || tipo_expresion == NULL) {
        return -1;
    }
    if (rutas_totales <= 0 || rutas_con_return > rutas_totales || rutas_con_return < 0) {
        return -1;
    }

    /* 2. Comprobación y diagnóstico de compatibilidad de tipos (Prioridad 1) */
    int tipos_compatibles = 0;

    if (strcmp(tipo_funcion, "void") == 0) {
        if (strcmp(tipo_expresion, "void") == 0) {
            tipos_compatibles = 1;
        } else {
            tipos_compatibles = 0; /* Devolver un valor en una función void siempre es error */
        }
    } else if (strcmp(tipo_funcion, tipo_expresion) == 0) {
        tipos_compatibles = 1;
    } else if (strcmp(tipo_funcion, "double") == 0 && strcmp(tipo_expresion, "int") == 0) {
        tipos_compatibles = 1; /* double acepta int (promoción) */
    } else {
        tipos_compatibles = 0; /* int no acepta double u otras combinaciones incompatibles */
    }

    if (!tipos_compatibles) {
        return 1; /* Tipo incompatible */
    }

    /* 3. Validación de rutas de control (Prioridad 2) */
    /* Si la función es void y la expresión es void, no es obligatorio que retorne explícitamente */
    if (strcmp(tipo_funcion, "void") == 0) {
        return 0;
    }

    if (rutas_con_return < rutas_totales) {
        return 2; /* Ruta de control sin retorno en función no-void */
    }

    return 0; /* Todo correcto */
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Funciones sin retorno o retornos mixtos

### Salida del harness
```text
=== AUDITORIA SESION 17 - Validacion de retornos y rutas ===
Caso                               esperado:              obtenido:              resultado
[ 1] int con int y dos rutas            esperado: 0            obtenido: 0            OK
[ 2] void devuelve valor                esperado: 1            obtenido: 1            OK
[ 3] ruta sin retorno                   esperado: 2            obtenido: 2            OK
[ 4] double acepta int                  esperado: 0            obtenido: 0            OK
[ 5] int no acepta double               esperado: 1            obtenido: 1            OK
[ 6] cero rutas de ejecucion            esperado: -1           obtenido: -1           OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion17.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Al revisar el código me di cuenta de que era importante considerar los casos donde las rutas de ejecución tienen valores incorrectos, por ejemplo, cuando las rutas totales son cero o cuando existen más rutas con return que rutas totales. También se tenía que comprobar qué pasaba cuando se recibía un puntero NULL.

Para solucionar estos problemas agregué validaciones al inicio del programa. También simplifiqué algunas condiciones para que fuera más fácil entender cómo se comparan los tipos de datos y cómo se comprueba que todas las rutas tengan return.

Otro punto importante fue conservar la regla de que una función void no está obligada a tener un return explícito, mientras que una función que devuelve un valor sí debe tener retorno en todas sus rutas.

---
**Firma digital:** Juanp-Alvarado
