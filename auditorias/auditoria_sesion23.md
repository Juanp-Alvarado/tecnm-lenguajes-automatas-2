# Bitácora de Requerimientos y Auditoría IA — Sesión 23
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Etiquetas y Saltos Incondicionales  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo 
**Matrícula:** 24531368  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Generador de etiquetas e instrucciones goto  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Al realizar el código manualmente, identifiqué que era necesario generar etiquetas únicas para evitar que los saltos se dirigieran a una parte incorrecta del programa. También tuve que cuidar que las etiquetas se escribieran con el formato correcto, por ejemplo L0: o GOTO L1. Otra dificultad fue controlar el tamaño del arreglo donde se guarda la instrucción, para evitar que se escribiera más texto del que puede almacenar.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Trazabilidad del flujo de control |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
### Prompt C-R-E-O (Contexto, Rol, Especificación, Output)

* **Contexto:** Estoy desarrollando un compilador en C para la materia de Lenguajes y Autómatas II. Necesito implementar la generación de código intermedio, específicamente la creación de etiquetas únicas consecuenciales (como `L0`, `L1`, ...) y las instrucciones de control de flujo correspondientes a saltos incondicionales (`GOTO Lx`) y marcas de destino (`Lx:`).
* **Rol:** Actúa como un Ingeniero de Software experto en sistemas y desarrollo de compiladores en C, que escribe código altamente seguro, eficiente, libre de advertencias (`-Wall -Wextra -Werror`) y robusto contra desbordamientos de búfer.
* **Especificación:**
  1. Implementa la función `void nueva_etiqueta(char *dst, int cap, int *contador)` que formatea la etiqueta como `Lx` donde `x` es el valor actual apuntado por `contador`, asegurando no desbordar `cap`. Después, incrementa el valor de `contador`.
  2. Implementa la función `void emitir_flujo(char *dst, int cap, const char *etiqueta, int es_salto)` que escribe en `dst` la cadena `GOTO <etiqueta>` si `es_salto` es verdadero (distinto de cero), o `<etiqueta>:` si `es_salto` es falso (cero). Si `etiqueta` es `NULL`, debe escribir una cadena vacía en `dst` de forma segura.
  3. Usa funciones seguras como `snprintf` para respetar el límite de capacidad `cap` y evitar vulnerabilidades.
* **Output:** Devuelve únicamente el código de implementación en C limpio, listo para colocarse en `ia_sesion23_c.c`, sin incluir la función `main`.
```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion23.h"
#include <stdio.h>
#include <string.h>

void nueva_etiqueta(char *dst, int cap, int *contador) {
    if (dst == NULL || cap <= 0 || contador == NULL) {
        return;
    }
    snprintf(dst, cap, "L%d", *contador);
    (*contador)++;
}

void emitir_flujo(char *dst, int cap, const char *etiqueta, int es_salto) {
    if (dst == NULL || cap <= 0) {
        return;
    }
    if (etiqueta == NULL) {
        dst[0] = '\0';
        return;
    }
    if (es_salto) {
        snprintf(dst, cap, "GOTO %s", etiqueta);
    } else {
        snprintf(dst, cap, "%s:", etiqueta);
    }
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Saltos adelante y atrás estructurados

### Salida del harness
```text
=== AUDITORIA SESION 23 - Etiquetas y saltos incondicionales ===
Caso                               esperado:              obtenido:              resultado
[ 1] nueva_etiqueta(L0)                 esperado: L0           obtenido: L0           OK
[ 2] nueva_etiqueta(L5)                 esperado: L5           obtenido: L5           OK
[ 3] emitir_flujo(GOTO L2)              esperado: GOTO L2      obtenido: GOTO L2      OK
[ 4] emitir_flujo(marca L2)             esperado: L2:          obtenido: L2:          OK
[ 5] emitir_flujo(NULL)                 esperado:              obtenido:              OK
[ 6] nueva_etiqueta(L10)                esperado: L10          obtenido: L10          OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion23.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> La IA contempló las validaciones principales, pero identifiqué que todavía podían existir casos donde el arreglo fuera demasiado pequeño para guardar toda la etiqueta o la instrucción. En ese caso, snprintf evita escribir fuera del espacio disponible, aunque el texto puede quedar incompleto. También se debe considerar que el contador no debe tener un valor negativo si se espera generar etiquetas desde cero.

---
**Firma digital:** Juanp-Alvarado
