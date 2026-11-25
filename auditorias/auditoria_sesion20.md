# Bitácora de Requerimientos y Auditoría IA — Sesión 20
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Hito 2: Analizador Semántico y Scopes  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo  
**Matrícula:** 24531368  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Integración de Tabla de Símbolos y AST  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Al realizar el ejercicio tuve dificultad para entender cómo se relacionan los ámbitos con la tabla de símbolos y el árbol de sintaxis abstracta (AST). Un problema importante fue identificar en qué nivel se encuentra una variable, ya que puede estar declarada en el ámbito actual o en alguno de sus padres.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Contrato de Validación Semántica Integral |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
### Método C-R-E-O para Resolución de Ámbitos

- **Contexto**: Resolución de nombres en un árbol de ámbitos (scopes).
- **Rol**: Especialista en construcción de analizadores semánticos en C.
- **Especificación**: 
  - `profundidad_de(actual, nombre)`: Retorna la distancia (0, 1, 2...) al ámbito donde se declara por primera vez el `nombre` subiendo por `padre`. Si no existe, retorna `-1`. Si hay parámetros nulos, retorna `-2`.
  - `contar_visibles(actual)`: Retorna el número de identificadores únicos visibles desde el ámbito `actual` considerando que los ámbitos internos ocultan (shadowing) a los externos. Si `actual` es nulo, retorna `-2`.
- **Output**: Archivo C libre de advertencias y listo para compilar con `-Wall -Wextra -Werror`.
```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion20.h"
#include <string.h>
#include <stddef.h>

int profundidad_de(const Ambito *actual, const char *nombre) {
    if (actual == NULL || nombre == NULL) {
        return -2;
    }

    const Ambito *curr = actual;
    int nivel = 0;
    while (curr != NULL) {
        for (int i = 0; i < curr->total; i++) {
            if (curr->simbolos[i] != NULL && strcmp(curr->simbolos[i], nombre) == 0) {
                return nivel;
            }
        }
        curr = curr->padre;
        nivel++;
    }
    return -1;
}

int contar_visibles(const Ambito *actual) {
    if (actual == NULL) {
        return -2;
    }

    const char *unicos[AMBITO_MAX * AMBITO_MAX];
    int total_unicos = 0;

    const Ambito *curr = actual;
    while (curr != NULL) {
        for (int i = 0; i < curr->total; i++) {
            const char *simbolo = curr->simbolos[i];
            if (simbolo == NULL) {
                continue;
            }

            int ya_existe = 0;
            for (int j = 0; j < total_unicos; j++) {
                if (strcmp(unicos[j], simbolo) == 0) {
                    ya_existe = 1;
                    break;
                }
            }

            if (!ya_existe) {
                unicos[total_unicos++] = simbolo;
            }
        }
        curr = curr->padre;
    }

    return total_unicos;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Casos complejos de shadowing y ámbito

### Salida del harness
```text
=== AUDITORIA SESION 20 - Resolucion de nombres y visibilidad ===
Caso                               esperado:              obtenido:              resultado
[ 1] nombre en el ambito actual         esperado: 0            obtenido: 0            OK
[ 2] nombre dos niveles arriba          esperado: 2            obtenido: 2            OK
[ 3] shadowing gana el interno          esperado: 0            obtenido: 0            OK
[ 4] nombre no visible                  esperado: -1           obtenido: -1           OK
[ 5] visibles sin repetir               esperado: 3            obtenido: 3            OK
[ 6] ambito NULL                        esperado: -2           obtenido: -2           OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion20.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Al revisar el código me di cuenta de que era importante considerar los símbolos que tienen el mismo nombre en diferentes ámbitos. Si no se revisan correctamente, podrían contarse varias veces.

Para solucionar esto, en la función contar_visibles() agregué una revisión que compara cada símbolo con los que ya fueron guardados. Si el nombre ya existe, no se vuelve a contar.

---
**Firma digital:** Juanp-Alvarado
