# Bitácora de Requerimientos y Auditoría IA — Sesión 14
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Declaración vs Uso de Variables  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo  
**Matrícula:** 24531368 
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Detección de variables no declaradas  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
>Al codificar de forma manual la lógica de la tabla de símbolos para declarar y usar, la principal dificultad radica en el manejo riguroso de apuntadores nulos (NULL) tanto para la estructura Tabla como para las cadenas de caracteres (nombre), así como el control estricto de los límites del arreglo estático definido por TABLA_MAX. Asimismo, es crucial asegurar que una redeclaración no sobrescriba ni incremente indebidamente el contador total, retornando 0 cuando la variable ya existe y 1 únicamente cuando la inserción es exitosa. 

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Invariante de definición previa |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
### Contrato C-R-E-O

* **C**ontexto: Compilador en C. Fase de análisis semántico para validar la regla de declaración previa usando una tabla de símbolos básica.
* **R**ol: Experto en C y diseño de compiladores.
* **E**specificación: Implementar en `ia_sesion14_c.c` las funciones definidas en `api_sesion14.h`:
  - `declarar()`: Retorna `-1` si recibe nulos o cadena vacía `""`. Retorna `0` si el nombre ya existe (usar `strcmp`) o si la tabla se llena (`TABLA_MAX`). Inserta y retorna `1` incrementando `total` en caso contrario.
  - `usar()`: Retorna `1` si el nombre exacto ya está en la tabla, de lo contrario retorna `0`.
  - Código libre de advertencias (`-Wall -Wextra -Werror`), sin función `main()`.
* **O**utput: Código fuente en C listo para compilar con el harness.
```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion14.h"
#include <stdio.h>
#include <string.h>

int declarar(Tabla *tabla, const char *nombre) {
    if (!tabla || !nombre || *nombre == '\0') {
        return -1;
    }

    // Revisar si ya está en la tabla
    int i;
    for (i = 0; i < tabla->total; i++) {
        if (tabla->nombres[i] && strcmp(tabla->nombres[i], nombre) == 0) {
            return 0; // ya existe
        }
    }

    // Chequear capacidad máxima
    if (tabla->total >= TABLA_MAX) {
        return 0;
    }

    int pos = tabla->total;
    tabla->nombres[pos] = nombre;
    tabla->total = pos + 1;

    return 1;
}

int usar(const Tabla *tabla, const char *nombre) {
    int resultado = 0;
    
    if (tabla == NULL || nombre == NULL) {
        return 0;
    }

    int idx = 0;
    while (idx < tabla->total) {
        if (tabla->nombres[idx] != NULL) {
            if (strcmp(tabla->nombres[idx], nombre) == 0) {
                resultado = 1;
                break; 
            }
        }
        idx++;
    }

    return resultado;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Uso antes de declarar y redeclaración

### Salida del harness
```text
=== AUDITORIA SESION 14 - Declaracion previa y redeclaracion ===
Caso                               esperado:              obtenido:              resultado
[ 1] usar sin declarar                  esperado: 0            obtenido: 0            OK
[ 2] usar despues de declarar           esperado: 1            obtenido: 1            OK
[ 3] redeclarar el mismo nombre         esperado: 0            obtenido: 0            OK
[ 4] total tras redeclarar              esperado: 1            obtenido: 1            OK
[ 5] prefijo no es el mismo nombre      esperado: 0            obtenido: 0            OK
[ 6] declarar nombre vacio              esperado: -1           obtenido: -1           OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion14.md
```

- [x] Caso base y límites superados sin *Crash*.
- [x] No existen accesos fuera de límites ni *Memory Leaks*.
- [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> La versión inicial generada por el modelo omitía verificar explícitamente si el nombre pasado como argumento era una cadena vacía (nombre[0] == '\0') en la función usar, lo cual podría causar comportamientos indefinidos si se evaluaban identificadores inválidos. Se ajustó la guarda inicial para validar tanto punteros nulos como cadenas vacías (nombre[0] == '\0'), garantizando robustez ante entradas malformadas en el analizador léxico/sintáctico.

---
**Firma digital:** Juanp-Alvarado
