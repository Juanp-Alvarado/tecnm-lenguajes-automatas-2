# Bitácora de Requerimientos y Auditoría IA — Sesión 12
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Atributos de Símbolos y Memoria  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo  
**Matrícula:** 24531368  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Estructura de registro de símbolos  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Al realizar el código manualmente, una de las dificultades fue identificar cuánto espacio ocupa cada tipo de dato en memoria. También tuve que considerar que las variables no siempre pueden colocarse una después de otra, ya que algunas necesitan una alineación específica. Otro problema fue calcular correctamente los desplazamientos (offsets) para evitar que los datos quedaran en posiciones incorrectas.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Cálculo de desplazamientos y alineación |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
- **Contexto**: Cálculo de memoria y alineación de variables para un compilador en C.
- **Rol**: Ingeniero de Compiladores experto en C.
- **Especificación**:
  1. `int ancho_de_tipo(const char *tipo)`: retorna 1 para `char`, 2 para `short`, 4 para `int`/`float`, 8 para `double`/punteros. Retorna -1 si es inválido o `NULL`.
  2. `int asignar_offset(Simbolo *simbolo, int cursor)`: valida `simbolo != NULL` y `cursor >= 0`. Alinea el cursor al múltiplo del ancho del tipo, lo guarda en `simbolo->offset` y retorna el nuevo cursor (offset + ancho).
- **Output**: Archivo `ia_sesion12_c.c` limpio, sin `main()`, compatible con `-Wall -Wextra -Werror`.
```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion12.h"
#include <string.h>
#include <stddef.h>

int ancho_de_tipo(const char *tipo) {
    if (tipo == NULL) {
        return -1;
    }
    if (strcmp(tipo, "char") == 0) {
        return 1;
    } else if (strcmp(tipo, "short") == 0) {
        return 2;
    } else if (strcmp(tipo, "int") == 0 || strcmp(tipo, "float") == 0) {
        return 4;
    } else if (strcmp(tipo, "double") == 0 || strcmp(tipo, "puntero") == 0 || strchr(tipo, '*') != NULL) {
        return 8;
    }
    return -1;
}

int asignar_offset(Simbolo *simbolo, int cursor) {
    if (simbolo == NULL || cursor < 0) {
        return -1;
    }
    
    int ancho = ancho_de_tipo(simbolo->tipo);
    if (ancho == -1) {
        return -1;
    }
    
    // Calcular alineación natural: redondear cursor al siguiente múltiplo de 'ancho'
    int residuo = cursor % ancho;
    int offset = (residuo == 0) ? cursor : (cursor + (ancho - residuo));
    
    simbolo->offset = offset;
    return offset + ancho;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Alineación de tipos y tamaños en bytes

### Salida del harness
```text
=== AUDITORIA SESION 12 - Registro de simbolo y offsets alineados ===
Caso                               esperado:              obtenido:              resultado
[ 1] ancho_de_tipo("int")               esperado: 4            obtenido: 4            OK
[ 2] ancho_de_tipo("double")            esperado: 8            obtenido: 8            OK
[ 3] int en cursor 0                    esperado: 4            obtenido: 4            OK
[ 4] char e int: offset de b            esperado: 4            obtenido: 4            OK
[ 5] double desde cursor 4              esperado: 8            obtenido: 8            OK
[ 6] registro NULL                      esperado: -1           obtenido: -1           OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion12.md
}
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Al revisar el código, identifiqué que era necesario validar los datos antes de realizar los cálculos. Por ejemplo, si el símbolo era NULL, el cursor tenía un valor negativo o el tipo de dato no estaba registrado, el programa debía regresar -1 para evitar errores.

También se consideró que el cursor podía no estar alineado con el tamaño del tipo de dato, por lo que agregué una operación para ajustar su posición al siguiente múltiplo correspondiente. De esta manera, el símbolo ocupa el espacio correcto en memoria.

---
**Firma digital:** (`Juanp-Alvarado`)
