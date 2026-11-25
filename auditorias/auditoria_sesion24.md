# Bitácora de Requerimientos y Auditoría IA — Sesión 24

**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Control de Flujo: Sentencia if-else  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo  
**Matrícula:** 24531368  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)

---

## 1. Fase 1: El Artesano (Dificultad Manual)

*Reto:* Saltos condicionales y evaluación booleana

**Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:**

> Al realizar el código manualmente, tuve dificultad para representar correctamente las condiciones AND y OR usando instrucciones de salto. También fue importante considerar el cortocircuito, ya que dependiendo del operador se debe evaluar primero un operando y después el otro. Otra restricción fue controlar el espacio disponible en el arreglo de instrucciones para no escribir fuera de sus límites.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal

| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Cortocircuito lógico en expresiones |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado

```text id="s24prompt"
### Prompt C-R-E-O para la IA

```markdown
**Contexto:** 
Estoy trabajando en la fase de traducción de código intermedio de un compilador en la materia de Lenguajes y Autómatas II. Específicamente, en la generación de código para control de flujo (sentencias if/if-else) con evaluación de cortocircuito.

**Rol:** 
Actúa como un programador experto en sistemas, compiladores y lenguaje C estándar (C11).

**Especificación:** 
Implementa la función `emitir_condicion` con la firma:
`int emitir_condicion(char lineas[][64], int max, const char *a, const char *b, char op, const char *etiqueta);`

Reglas detalladas:
1. Si el operador `op` es '&' (representa AND lógico): usa `IFFALSE` para saltar a la `etiqueta` de salida.
2. Si el operador `op` es '|' (representa OR lógico): usa `IFTRUE` para saltar a la `etiqueta` de salida.
3. Si el segundo operando `b` es `NULL`, la condición es simple. Se debe emitir únicamente un salto usando la regla de '&' (es decir, `IFFALSE a GOTO etiqueta`) independientemente del operador y no se debe intentar evaluar `b`.
4. Cada instrucción generada se guarda secuencialmente en la matriz `lineas`, cuidando de no sobrepasar el límite `max`.
5. Retorna la cantidad de líneas/instrucciones de salto efectivamente emitidas (máximo 2).
6. No incluyas una función `main` en tu respuesta, cumple estrictamente con `-Wall -Wextra -Werror` y evita advertencias del compilador.

**Output:** 
Devuelve el código en C puro con los encabezados necesarios (`api_sesion24.h`, `<stdio.h>`, `<string.h>`).

```

---

## 3. Código Devuelto por el Modelo

```c id="s24code"
#include "api_sesion24.h"
#include <stdio.h>
#include <string.h>

int emitir_condicion(char lineas[][64], int max, const char *a, const char *b, char op, const char *etiqueta) {
    int escritas = 0;

    if (max <= 0 || a == NULL || etiqueta == NULL) {
        return 0;
    }

    // Si b es NULL, es una condicion simple (por defecto se comporta como cortocircuito falso)
    if (b == NULL) {
        if (escritas < max) {
            snprintf(lineas[escritas], 64, "IFFALSE %s GOTO %s", a, etiqueta);
            escritas++;
        }
    } else {
        // Primer operando
        if (escritas < max) {
            if (op == '&') {
                snprintf(lineas[escritas], 64, "IFFALSE %s GOTO %s", a, etiqueta);
            } else if (op == '|') {
                snprintf(lineas[escritas], 64, "IFTRUE %s GOTO %s", a, etiqueta);
            }
            escritas++;
        }

        // Segundo operando (cortocircuito)
        if (escritas < max) {
            if (op == '&') {
                snprintf(lineas[escritas], 64, "IFFALSE %s GOTO %s", b, etiqueta);
            } else if (op == '|') {
                snprintf(lineas[escritas], 64, "IFTRUE %s GOTO %s", b, etiqueta);
            }
            escritas++;
        }
    }

    return escritas;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)

*Pruebas ejecutadas:* Condiciones compuestas con AND y OR

### Salida del harness

```text id="s24harness"
=== AUDITORIA SESION 24 - Saltos condicionales con cortocircuito ===
Caso                               esperado:              obtenido:              resultado
[ 1] a && b (primer salto)
     esperado: IFFALSE a GOTO L9
     obtenido: IFFALSE a GOTO L9
     -> OK
[ 2] a && b (segundo salto)
     esperado: IFFALSE b GOTO L9
     obtenido: IFFALSE b GOTO L9
     -> OK
[ 3] a && b (dos saltos)                esperado: 2            obtenido: 2            OK
[ 4] a || b (primer salto)
     esperado: IFTRUE a GOTO L9
     obtenido: IFTRUE a GOTO L9
     -> OK
[ 5] a || b (segundo salto)
     esperado: IFTRUE b GOTO L9
     obtenido: IFTRUE b GOTO L9
     -> OK
[ 6] condicion simple (b NULL)
     esperado: IFFALSE a GOTO L0
     obtenido: IFFALSE a GOTO L0
     -> OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion24.md
```

- [x] Caso base y límites superados sin *Crash*.
- [x] No existen accesos fuera de límites ni *Memory Leaks*.
- [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico

**¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?**

> Identifiqué que era necesario revisar el límite del arreglo antes de escribir una segunda instrucción, ya que el espacio disponible puede ser menor al número de instrucciones que se quieren generar. También se debe considerar el caso donde el operador no sea '&' ni '|', porque en ese caso no se genera una instrucción para la condición compuesta.
>
> Para mejorar el código, agregué validaciones para comprobar que los apuntadores fueran válidos y que la capacidad fuera mayor que cero. Además, utilicé `snprintf` para limitar el tamaño de cada instrucción y evitar escribir fuera del arreglo.

### Análisis de complejidad

- `emitir_condicion`: O(1), porque genera como máximo dos instrucciones.
- Memoria adicional: O(1), porque solo utiliza una variable para contar las instrucciones y no reserva memoria dinámica.

---

**Firma digital:** Juanp-Alvarado
