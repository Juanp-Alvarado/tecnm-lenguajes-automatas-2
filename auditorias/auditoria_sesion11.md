# Bitácora de Requerimientos y Auditoría IA — Sesión 11
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Arquitectura de la Tabla de Símbolos  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo  
**Matrícula:** 24531368  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Tabla Hash con encadenamiento  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Al realizar el código manualmente, tuve dificultad para organizar los símbolos dentro de la tabla hash y controlar las colisiones cuando dos nombres generaban la misma posición. También fue necesario tener cuidado con el manejo de memoria dinámica al utilizar malloc y con los apuntadores de las listas enlazadas para evitar errores al insertar nuevos elementos.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Función de dispersión y colisiones O(1) |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C-R-E-O

C (Contexto): Desarrollamos una Tabla de Símbolos para un compilador en C, utilizando una Tabla Hash con encadenamiento para manejar colisiones.

R (Rol): Actúa como programador experto en C, aplicando buenas prácticas de seguridad, memoria y estándar C11.

E (Especificación):

Implementar hash_djb2() con el algoritmo DJB2, validando cadenas NULL.
Implementar insertar_simbolo() validando parámetros.
Calcular el índice mediante módulo.
Evitar símbolos duplicados usando strcmp.
Crear nodos con malloc, copiar el nombre de forma segura y asignar tipo y offset.
Insertar al final de la lista enlazada.
Retornar 1 si inserta, 0 si ya existe y -1 si ocurre un error.
O (Output): Entregar únicamente el código C comentado, listo para ia_sesion11_c.c, sin incluir main().
```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion11.h"
#include 
#include 
#include 

/*
 * djb2: Algoritmo clasico de hash propuesto por Dan Bernstein.
 * Empieza en 5381 y multiplica por 33 en cada byte (hash * 33 + c).
 */
unsigned int hash_djb2(const char *clave) {
    if (clave == NULL) {
        return 0;
    }
    unsigned int hash = 5381;
    int c;
    while ((c = (unsigned char)*clave++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}


int insertar_simbolo(Simbolo *tabla[], unsigned int cubetas, const char *nombre, int tipo, int offset) {
    if (tabla == NULL || nombre == NULL || cubetas == 0) {
        return -1;
    }

    /* Calcular el indice hash aplicando el modulo segun el numero de cubetas */
    unsigned int hash_val = hash_djb2(nombre);
    unsigned int indice = hash_val % cubetas;

    /* Recorrer la cadena en la cubeta correspondiente para buscar si ya existe */
    Simbolo *actual = tabla[indice];
    Simbolo *anterior = NULL;
    while (actual != NULL) {
        if (strcmp(actual->nombre, nombre) == 0) {
            return 0; /* El simbolo ya existe, no se duplica */
        }
        anterior = actual;
        actual = actual->sig;
    }

    /* Crear el nuevo nodo de Simbolo */
    Simbolo *nuevo = (Simbolo *)malloc(sizeof(Simbolo));
    if (nuevo == NULL) {
        return -1; /* Error de asignacion de memoria */
    }

    /* Inicializar la estructura del nuevo simbolo */
    strncpy(nuevo->nombre, nombre, sizeof(nuevo->nombre) - 1);
    nuevo->nombre[sizeof(nuevo->nombre) - 1] = '\0';
    nuevo->tipo = tipo;
    nuevo->offset = offset;
    nuevo->sig = NULL;

    /* Insertar al final de la cadena de la lista enlazada para mantener el orden */
    if (anterior == NULL) {
        tabla[indice] = nuevo; /* Primer elemento en la cubeta */
    } else {
        anterior->sig = nuevo; /* Encolar despues del ultimo elemento */
    }

    return 1; /* Insercion exitosa */
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Manejo de claves duplicadas y colisiones

### Salida del harness
```text
=== AUDITORIA SESION 11 - Tabla hash con encadenamiento ===
Caso                               esperado:              obtenido:              resultado
[ 1] hash_djb2("")                      esperado: 5381         obtenido: 5381         OK
[ 2] hash_djb2("abc")                   esperado: 193485963    obtenido: 193485963    OK
[ 3] insertar x dos veces               esperado: 0            obtenido: 0            OK
[ 4] dos claves, una cubeta             esperado: 2            obtenido: 2            OK
[ 5] offset de beta en cadena           esperado: 4            obtenido: 4            OK
[ 6] insertar en tabla NULL             esperado: -1           obtenido: -1           OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion11.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Al revisar el código, identifiqué que era importante controlar la longitud del nombre para evitar que se escribieran datos fuera del espacio disponible en la estructura. Para solucionarlo utilicé strncpy, limitando la cantidad de caracteres copiados y agregando el carácter nulo al final. También se agregó la validación de memoria para comprobar que malloc haya reservado correctamente el espacio.

Otro aspecto que se debe considerar es que la tabla de símbolos necesita liberar los nodos de memoria cuando termine de utilizarse, ya que la función de inserción solamente reserva memoria y no se encarga de liberarla.

---
**Firma digital:** Juanp-Alvarado
