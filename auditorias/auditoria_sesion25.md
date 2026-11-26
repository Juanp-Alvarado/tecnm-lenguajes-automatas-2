# Bitácora de Requerimientos y Auditoría IA — Sesión 25

**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Control de Flujo: Bucles while y for  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo  
**Matrícula:** 24531368  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)

---

## 1. Fase 1: El Artesano (Dificultad Manual)

*Reto:* Estructura de ciclo en instrucciones TAC

**Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:**

> Al codificar los ciclos manualmente, tuve que cuidar el orden de las instrucciones para que el programa pudiera regresar al inicio y comprobar nuevamente la condición. También fue necesario generar etiquetas diferentes para el inicio y el final de cada ciclo, especialmente cuando existen bucles anidados. Otra dificultad fue revisar que hubiera suficiente espacio en el arreglo antes de agregar las instrucciones y que la variable de actualización del ciclo `for` no se omitiera.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal

| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Garantía de actualización de iterador |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado

```text id="s25prompt"
### Contrato C-R-E-O para la IA

**Contexto:** Estoy desarrollando un generador de código intermedio (TAC - Código de Tres Direcciones) para un compilador en C dentro de la materia de Lenguajes y Autómatas II. Necesito generar las estructuras de control para bucles `while` y `for` utilizando etiquetas únicas secuenciales.

**Rol:** Actúa como un Programador Senior experto en C y Teoría de Compiladores.

**Especificación:** 
Implementa las siguientes dos funciones declaradas en `api_sesion25.h`:

1. `void emitir_while(char lineas[][64], int max, int *n, const char *cond, int *etiquetas);`
   - Debe emitir 4 líneas de código TAC en el arreglo `lineas` a partir del índice `*n` actualizando su valor.
   - Línea 1: Marca de inicio: `L<etiqueta_inicio>:`
   - Línea 2: Salto condicional: `IFFALSE <cond> GOTO L<etiqueta_fin>`
   - Línea 3: GOTO de retorno: `GOTO L<etiqueta_inicio>`
   - Línea 4: Marca de fin: `L<etiqueta_fin>:`
   - Cada llamada consume 2 etiquetas únicas del entero apuntado por `etiquetas` (p. ej., si `*etiquetas` es 0, usa `L0` y `L1`, e incrementa `*etiquetas` en 2 para evitar colisiones).

2. `void emitir_for(char lineas[][64], int max, int *n, const char *inicio, const char *cond, const char *post, int *etiquetas);`
   - Si `inicio` no es NULL, emite la inicialización: `<inicio>`
   - Emite la marca de inicio de bucle: `L<etiqueta_inicio>:`
   - Emite el salto condicional: `IFFALSE <cond> GOTO L<etiqueta_fin>`
   - Si `post` no es NULL, emite la actualización: `<post>` (colocada justo antes del salto de retorno)
   - Emite el salto de retorno: `GOTO L<etiqueta_inicio>`
   - Emite la marca de fin de bucle: `L<etiqueta_fin>:`
   - Consume 2 etiquetas incrementando `*etiquetas` en 2.

**Output:** Devuelve únicamente el código fuente en C limpio, compatible con `-Wall -Wextra -Werror` y estándar C11, sin función `main()`.
```
```

---

## 3. Código Devuelto por el Modelo

```c id="s25code"
%%writefile ia_sesion25_c.c
/* ia_sesion25_c.c - Fase 2: El Cientifico
 * 
 * Implementacion generada bajo contrato C-R-E-O
 */
#include "api_sesion25.h"
#include <stdio.h>
#include <string.h>

void emitir_while(char lineas[][64], int max, int *n, const char *cond, int *etiquetas) {
    if (*n + 4 > max) return; // Evitar desbordamiento de memoria

    int lbl_inicio = (*etiquetas)++;
    int lbl_fin = (*etiquetas)++;

    // 1. Marca de inicio
    snprintf(lineas[*n], 64, "L%d:", lbl_inicio);
    (*n)++;

    // 2. Condicion de salida
    snprintf(lineas[*n], 64, "IFFALSE %s GOTO L%d", cond, lbl_fin);
    (*n)++;

    // 3. GOTO de regreso
    snprintf(lineas[*n], 64, "GOTO L%d", lbl_inicio);
    (*n)++;

    // 4. Marca de fin
    snprintf(lineas[*n], 64, "L%d:", lbl_fin);
    (*n)++;
}

void emitir_for(char lineas[][64], int max, int *n, const char *inicio, const char *cond, const char *post, int *etiquetas) {
    int lineas_necesarias = 4 + (inicio != NULL ? 1 : 0) + (post != NULL ? 1 : 0);
    if (*n + lineas_necesarias > max) return;

    int lbl_inicio = (*etiquetas)++;
    int lbl_fin = (*etiquetas)++;

    // 1. Inicializacion (si existe)
    if (inicio != NULL) {
        snprintf(lineas[*n], 64, "%s", inicio);
        (*n)++;
    }

    // 2. Marca de inicio
    snprintf(lineas[*n], 64, "L%d:", lbl_inicio);
    (*n)++;

    // 3. Salto condicional
    snprintf(lineas[*n], 64, "IFFALSE %s GOTO L%d", cond, lbl_fin);
    (*n)++;

    // 4. Actualizacion/post (si existe)
    if (post != NULL) {
        snprintf(lineas[*n], 64, "%s", post);
        (*n)++;
    }

    // 5. GOTO de regreso
    snprintf(lineas[*n], 64, "GOTO L%d", lbl_inicio);
    (*n)++;

    // 6. Marca de fin
    snprintf(lineas[*n], 64, "L%d:", lbl_fin);
    (*n)++;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)

*Pruebas ejecutadas:* Bucles anidados y condiciones de parada

### Salida del harness

```text id="s25harness"
=== AUDITORIA SESION 25 - Esqueleto TAC de while y for ===
Caso                               esperado:              obtenido:              resultado
[ 1] while: marca de inicio             esperado: L0:          obtenido: L0:          OK
[ 2] while: condicion
     esperado: IFFALSE a < b GOTO L1
     obtenido: IFFALSE a < b GOTO L1
     -> OK
[ 3] dos ciclos sin colision            esperado: L2:          obtenido: L2:          OK
[ 4] for: inicializacion                esperado: i = 0        obtenido: i = 0        OK
[ 5] for: actualizacion                 esperado: i = i + 1    obtenido: i = i + 1    OK
[ 6] for sin post (NULL)                esperado: 5            obtenido: 5            OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion25.md
```

- [x] Caso base y límites superados sin *Crash*.
- [x] No existen accesos fuera de límites ni *Memory Leaks*.
- [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico

**¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?**

> Identifiqué que era necesario revisar el espacio disponible antes de escribir todas las instrucciones del ciclo. Si el arreglo no tiene capacidad suficiente, la función debe salir sin modificar las líneas.
>
> También revisé que las etiquetas fueran diferentes para evitar confusiones en ciclos anidados. En el caso del `for`, se debe considerar que la instrucción de actualización normalmente se ejecuta después del cuerpo del ciclo y antes de regresar a evaluar la condición.
>
---

**Firma digital:** Juanp-Alvarado
