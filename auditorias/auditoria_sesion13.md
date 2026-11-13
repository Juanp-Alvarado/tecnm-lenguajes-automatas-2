# Bitácora de Requerimientos y Auditoría IA — Sesión 13
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Manejo de Ámbitos (Scope Global/Local)  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo 
**Matrícula:** 24531368  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Pila de tablas de símbolos (Stack Scopes)  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Al realizar el código manualmente, una de las dificultades fue controlar la búsqueda de variables entre los diferentes ámbitos. Era necesario revisar primero el ámbito actual y después los ámbitos anteriores para encontrar una variable. También se debía evitar registrar nombres repetidos y comprobar que existiera espacio disponible en el arreglo. Otro detalle fue manejar correctamente los apuntadores para recorrer los ámbitos sin provocar errores en el programa.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Ocultamiento de variables (Shadowing) |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text

 * CONTRATO C-R-E-O:
 * Contexto: Desarrollo de un compilador (Manejo de Ámbitos, Scope Global vs Local) en C.
 * Rol: Científico / Experto en compiladores y lenguaje C.
 * Especificación:
     - Implementar `int declarar(Ambito *ambito, const char *nombre)`:
      - Si `ambito` o `nombre` es NULL, o `nombre` es una cadena vacía, retornar -1.
      - Si el ámbito ya está lleno (`total >= AMBITO_MAX`), retornar -1.
     - Buscar si `nombre` ya existe únicamente en el ámbito actual. Si ya existe, retornar 0.
      - Si no existe y hay espacio, guardarlo en el ámbito actual e incrementar `total`. Retornar 1.
     - Implementar `const char *buscar(const Ambito *ambito, const char *nombre)`:
      - Si `ambito` o `nombre` es NULL, o `nombre` es vacío, retornar NULL.
     - Buscar `nombre` en el ámbito actual. Si se encuentra, retornar `ambito->nombre`.
      - Si no se encuentra en el actual, subir jerárquicamente a través de `ambito->padre` buscando en cada nivel superior.
      - Si se llega a la raíz y no se encuentra, retornar NULL.
 * Output: Código limpio en C, sin incluir la función `main()` y que compile con -Wall -Wextra -Werror.
 
```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion13.h"
#include <string.h>
#include <stddef.h>

int declarar(Ambito *ambito, const char *nombre) {
    if (ambito == NULL || nombre == NULL || nombre[0] == '\0') {
        return -1;
    }
    
    // Verificar si ya existe en el ambito actual
    for (int i = 0; i < ambito->total; i++) {
        if (strcmp(ambito->nombres[i], nombre) == 0) {
            return 0;
        }
    }
    
    // Verificar si el ambito esta lleno
    if (ambito->total >= AMBITO_MAX) {
        return -1;
    }
    
    // Insertar el nuevo nombre
    ambito->nombres[ambito->total] = nombre;
    ambito->total++;
    return 1;
}

const char *buscar(const Ambito *ambito, const char *nombre) {
    if (ambito == NULL || nombre == NULL || nombre[0] == '\0') {
        return NULL;
    }
    
    const Ambito *actual = ambito;
    while (actual != NULL) {
        for (int i = 0; i < actual->total; i++) {
            if (strcmp(actual->nombres[i], nombre) == 0) {
                return actual->nombre;
            }
        }
        actual = actual->padre;
    }
    
    return NULL;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Búsqueda jerárquica de identificadores

### Salida del harness
```text
=== AUDITORIA SESION 13 - Pila de ambitos y busqueda jerarquica ===
Caso                               esperado:              obtenido:              resultado
[ 1] declarar(&g, "x")                  esperado: 1            obtenido: 1            OK
[ 2] buscar en el mismo ambito          esperado: global       obtenido: global       OK
[ 3] buscar subiendo al padre           esperado: global       obtenido: global       OK
[ 4] shadowing en el hijo               esperado: bloque       obtenido: bloque       OK
[ 5] tres niveles hasta la raiz         esperado: raiz         obtenido: raiz         OK
[ 6] nombre no declarado                esperado: (nulo)       obtenido: (nulo)       OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion13.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Durante la revisión del código identifiqué que era importante comprobar los apuntadores antes de utilizarlos, ya que podían recibirse valores NULL. También era necesario verificar que el ámbito no estuviera lleno antes de guardar un nuevo identificador. Para solucionarlo se agregaron condiciones que validan los datos de entrada y revisan la capacidad del arreglo.

Otro aspecto importante fue comprobar que la búsqueda no se limitara al ámbito actual, sino que también recorriera los ámbitos padres. Con esto se permite encontrar variables globales desde un ámbito local y respetar el funcionamiento del Shadowing, dando prioridad a la variable declarada en el ámbito más cercano.

---
**Firma digital:** Juanp-Alvarado
