# Bitácora de Requerimientos y Auditoría IA — Sesión 03
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Google Colab, Mágicos y Flujo de Entrega  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo 
**Matrícula:** 24531368  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Manipulación de archivos y llamadas OS  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> 

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Salida estándar y códigos de retorno ABI |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
[Contexto] Trabajas en un entorno C99 estándar utilizando GCC, implementando operaciones de E/S de archivos y ejecución de comandos del sistema operativo sin librerías externas o no estándar.
[Rol] Ingeniero de software de sistemas experto en programación en C, manejo de archivos y llamadas al sistema.
[Especificación] Implementa las funciones declaradas en el contrato api_sesion03.h para el archivo ia_sesion03_c.c: escribir_texto (apertura en modo escritura "w", escritura de texto, cierre seguro y retorno de -1 o 0), contar_lineas (apertura en modo lectura "r", conteo carácter por carácter manejando la última línea sin salto de línea \n, y retorno del total de líneas o -1) y ejecutar (ejecución de comandos mediante system(), validación de errores y extracción del código de salida real usando WEXITSTATUS o -1 en caso de fallo). El código debe ser robusto ante casos límite como fallos de apertura de archivos.
```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion03.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>   
#include <sys/wait.h> 

int escribir_texto(const char *ruta, const char *texto) {
    FILE *archivo = fopen(ruta, "w");
    if (archivo == NULL) {
        return -1;
    }
    fputs(texto, archivo);
    fclose(archivo);
    return 0;
}

int contar_lineas(const char *ruta) {
    FILE *archivo = fopen(ruta, "r");
    if (archivo == NULL) {
        return -1;
    }

    int lineas = 0; 
    int character;
    int ultimo_caracter = EOF; 

    while ((character = fgetc(archivo)) != EOF) {
        ultimo_caracter = character;
        if (character == '\n') {
            lineas++;
        }
    }
    fclose(archivo);

    if (ultimo_caracter != EOF && ultimo_caracter != '\n') {
        lineas++;
    }

    return lineas;
}

int ejecutar(const char *comando) {
    int estado = system(comando);
    if (estado == -1) {
        return -1;
    }
    
    return WEXITSTATUS(estado);
}

```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Detección de errores de compilación GCC

### Salida del harness
```text
=== AUDITORIA SESION 03 - Archivos y codigos de salida ===
Caso                               esperado:              obtenido:              resultado
[ 1] escribir_texto(...)==0             esperado: 0            obtenido: 0            OK
[ 2] contar_lineas(3 lineas)            esperado: 3            obtenido: 3            OK
[ 3] contar_lineas(sin salto final)     esperado: 2            obtenido: 2            OK
[ 4] contar_lineas(inexistente)         esperado: -1           obtenido: -1           OK
[ 5] ejecutar("true")                   esperado: 0            obtenido: 0            OK
[ 6] ejecutar("false")                  esperado: 1            obtenido: 1            OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion03.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> El caso borde omitido fue que la función básica de contar_lineas solo contaba los saltos de línea (\n), ignorando la última línea si el archivo finalizaba sin uno. Para solucionarlo, se implementó una variable auxiliar (ultimo_caracter) durante la lectura carácter por carácter. Al terminar el ciclo, se añadió una condición: si el archivo no estaba vacío y el último carácter no era \n, se incrementó manualmente el contador (lineas++), garantizando un conteo exacto en cualquier escenario.

---
**Firma digital:** Juanp-Alvarado
