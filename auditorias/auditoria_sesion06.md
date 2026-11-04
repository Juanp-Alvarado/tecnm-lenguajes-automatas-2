# Bitácora de Requerimientos y Auditoría IA — Sesión 06
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Sistema de Tipos Primitivos  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo  
**Matrícula:** 24531368 
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Chequeo de compatibilidad de tipos  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Durante el desarrollo manual identifiqué que la principal dificultad fue establecer correctamente las condiciones para determinar cuándo una conversión entre tipos de datos era segura. También fue necesario diferenciar el tipo de origen del tipo de destino para evitar aceptar asignaciones que pudieran provocar pérdida de información.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Matriz de compatibilidad estática |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
**C – Contexto:** Desarrollar un programa en lenguaje C que evalúe la compatibilidad entre diferentes tipos de datos.

**R – Rol:** Actúa como programador especializado en C y programación estructurada.

**E – Especificaciones:** Utilizar caracteres, enteros y decimales; permitir conversiones seguras, rechazar las que puedan perder información e identificar cada tipo de dato por su nombre.

**O – Objetivo:** Crear un sistema sencillo que valide las conversiones de datos y determine cuáles son compatibles, utilizando funciones y estructuras de control en C.
```

---

## 3. Código Devuelto por el Modelo
```c
%%writefile ia_sesion06_c.c
/* ia_sesion06_c.c - Fase 2: El Cientifico
 *
 * Pega aqui el codigo que te devolvio la IA, con la MISMA firma
 * que declara api_sesion06.h. No puede traer main().
 */
#include "api_sesion06.h"

int compatible(Tipo destino, Tipo origen)
{
    // TIPO_ERROR nunca es compatible con nada
    if (destino == TIPO_ERROR || origen == TIPO_ERROR)
    {
        return 0;
    }

    // Un tipo es compatible consigo mismo
    if (destino == origen)
    {
        return 1;
    }

    // Jerarquía: CHAR < INT < FLOAT. Solo se permite asignar a un tipo de igual o mayor rango
    if (destino == TIPO_INT && origen == TIPO_CHAR)
    {
        return 1;
    }

    if (destino == TIPO_FLOAT && (origen == TIPO_CHAR || origen == TIPO_INT))
    {
        return 1;
    }

    // Cualquier otra asignación (e.g. tipo más ancho a más angosto) se rechaza
    return 0;
}

const char *nombre_tipo(Tipo t)
{
    switch (t)
    {
        case TIPO_ERROR:
            return "error";

        case TIPO_CHAR:
            return "char";

        case TIPO_INT:
            return "int";

        case TIPO_FLOAT:
            return "float";

        default:
            return "desconocido";
    }
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Rechazo de asignaciones incompatibles

### Salida del harness
```text
=== AUDITORIA SESION 06 - Tabla de compatibilidad de tipos ===
Caso                               esperado:              obtenido:              resultado
[ 1] compatible(INT,INT)                esperado: 1            obtenido: 1            OK
[ 2] compatible(FLOAT,INT)              esperado: 1            obtenido: 1            OK
[ 3] compatible(INT,FLOAT)              esperado: 0            obtenido: 0            OK
[ 4] compatible(INT,ERROR)              esperado: 0            obtenido: 0            OK
[ 5] nombre_tipo(INT)                   esperado: int          obtenido: INT          FALLA
[ 6] nombre_tipo(ERROR)                 esperado: error        obtenido: ERROR        FALLA

RESULTADO: 4/6 pruebas superadas
Aun hay 2 caso(s) sin resolver. Revisa tu implementacion.
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Durante la revisión del código identifiqué que era necesario contemplar las entradas inválidas, ya que no basta con comprobar únicamente los tipos definidos en la enumeración. También se debe verificar que los valores recibidos pertenezcan a las categorías permitidas. Para mejorar la solución, se debe validar primero el tipo de destino y origen antes de comparar si son iguales. Esto evita aceptar valores no reconocidos por error.

---
**Firma digital:** [Usuario de GitHub]
