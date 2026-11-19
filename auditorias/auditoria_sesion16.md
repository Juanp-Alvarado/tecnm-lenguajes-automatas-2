# Bitácora de Requerimientos y Auditoría IA — Sesión 16
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Firmas de Función y Parámetros  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo  
**Matrícula:** 24531368  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Registro de prototipos y aridad  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Al realizar la función manualmente, tuve dificultad para organizar la búsqueda dentro del catálogo de funciones y comparar correctamente el nombre con la cantidad de argumentos. También tuve que considerar los posibles valores nulos para evitar errores al utilizar los apuntadores. Otro detalle fue controlar el recorrido del arreglo para no acceder a posiciones fuera de sus límites. Comprendí que no solamente se debe verificar si una función existe, sino también comprobar que la cantidad de parámetros coincida con la registrada.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Coincidencia estricta de parámetros |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
Contexto: Estoy construyendo la fase de análisis semántico (comprobación de tipos y firmas) de un compilador en C para la materia de Lenguajes y Autómatas II. Necesito una función que valide si una llamada a una función cumple con la firma registrada en el catálogo.

Rol: Actúa como un Ingeniero de Software experto en C y Teoría de Compiladores, enfocado en código seguro, limpio y que cumple estrictamente con el estándar C11 sin generar advertencias (-Wall -Wextra -Werror).

Especificación: Implementa la función con la siguiente firma: int verificar_llamada(const Funcion *catalogo, int total, const char *nombre, int nargs);

Donde Funcion está definida como:

typedef struct Funcion {
    const char *nombre;
    int aridad;
} Funcion;
Reglas de negocio:

Si catalogo es NULL, nombre es NULL o nargs es negativo, la función debe retornar -1 inmediatamente.
Debe buscar de forma exacta el nombre dentro del arreglo catalogo (de tamaño total).
Si encuentra la función por su nombre:
Retorna 1 si el número de argumentos nargs coincide exactamente con la aridad registrada (aridad).
Retorna 0 si la aridad no coincide.
Si recorre todo el catálogo y el nombre no existe, debe retornar -1.
Evita cualquier tipo de desbordamiento, punteros nulos o comportamiento indefinido.
Output: Devuelve únicamente el código de la función en C, listo para guardarse en ia_sesion16_c.c, incluyendo la cabecera "api_sesion16.h" y las librerías necesarias de la biblioteca estándar (como <string.h> para comparar cadenas). No incluyas una función main.
```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion16.h"
#include 
#include 

int verificar_llamada(const Funcion *catalogo, int total, const char *nombre, int nargs) {
    if (catalogo == NULL || nombre == NULL || nargs < 0) {
        return -1;
    }

    for (int i = 0; i < total; i++) {
        if (catalogo[i].nombre != NULL && strcmp(catalogo[i].nombre, nombre) == 0) {
            if (catalogo[i].aridad == nargs) {
                return 1;
            } else {
                return 0;
            }
        }
    }

    return -1;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Llamadas con número incorrecto de args

### Salida del harness
```text
=== AUDITORIA SESION 16 - Registro de prototipos y aridad ===
Caso                               esperado:              obtenido:              resultado
[ 1] suma con 3 argumentos              esperado: 1            obtenido: 1            OK
[ 2] suma con 2 argumentos              esperado: 0            obtenido: 0            OK
[ 3] suma con 4 argumentos              esperado: 0            obtenido: 0            OK
[ 4] funcion no registrada              esperado: -1           obtenido: -1           OK
[ 5] main sin argumentos                esperado: 1            obtenido: 1            OK
[ 6] nombre NULL                        esperado: -1           obtenido: -1           OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion16.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Al revisar el código generado por la IA, identifiqué que no validaba explícitamente si el valor de total era menor o igual a cero. Esto podía provocar que la función recorriera incorrectamente el catálogo o que no realizara ninguna búsqueda cuando no había elementos registrados. También consideré importante comprobar que los nombres de las entradas no fueran NULL antes de utilizar strcmp(). Para evitar estos problemas, se debe validar que el tamaño del catálogo sea positivo y conservar la comprobación de apuntadores nulos. Después de revisar la implementación y ejecutar el harness, se comprobó que los seis casos de prueba fueron superados.

---
**Firma digital:** Juanp-Alvarado
