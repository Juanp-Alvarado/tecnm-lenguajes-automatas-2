# Bitácora de Requerimientos y Auditoría IA — Sesión 01
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Encuadre, Diagnóstico de Punteros y Metodología IA  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Alvarado Martinez Juan Pablo   
**Matrícula:** 24531368
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Inversión de buffer in-place (char*)  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Al intentar implementar la inversión in-place directamente, el principal tropiezo fue el manejo de literales de cadena (por ejemplo, char *s = "hola"), las cuales residen en una sección de memoria de solo lectura (text segment), provocando un fallo de segmento (Segmentation Fault) al intentar modificar sus caracteres. La solución manual requirió utilizar arreglos mutables almacenados en el stack (char s[] = "hola"). Asimismo, se debió gestionar cuidadosamente el cálculo de los punteros de inicio y fin (str y str + len - 1), asegurando que el intercambio de variables temporales no sobreescribiera datos antes de tiempo ni causara desbordamientos con longitudes impares.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Punteros, O(n) tiempo, O(1) memoria |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
[Contexto] Trabajas en un entorno C99 estándar utilizando GCC, sin librerías externas o no estándar.

[Rol] Actúa como un ingeniero de software de sistemas y compiladores experto en gestión de memoria y punteros en C.

[Especificación] Escribe una función modular en C llamada invertir que reciba un único puntero char *s para invertir una cadena de caracteres terminada en nulo "in-place" (in situ). La implementación debe calcular la longitud de manera segura mediante strlen, mantener estrictamente una complejidad temporal O(n) y una complejidad espacial auxiliar O(1). Debe contemplar de manera robusta la seguridad de punteros y casos límite (como punteros NULL).

[Output] Proporciona únicamente el código fuente modular en C junto con un breve análisis de su complejidad temporal y espacial en comentarios, omitiendo explicaciones conversacionales adicionales.
```

---

## 3. Código Devuelto por el Modelo
```c
#include <stddef.h>

/**
 * @brief Invierte un buffer de caracteres in-place utilizando aritmética de punteros.
 * @param str Puntero al inicio del buffer de caracteres.
 * @param len Longitud del buffer.
 */
void invertir_buffer(char *str, size_t len) {
    // Guarda de seguridad para punteros nulos o longitudes triviales
    if (str == NULL || len <= 1) {
        return;
    }

    char *start = str;
    char *end = str + len - 1;

    while (start < end) {
        // Intercambio de valores usando una variable temporal (In-place)
        char temp = *start;
        *start = *end;
        *end = temp;

        start++;
        end--;
    }
}

/*
 * Análisis de Complejidad:
 * - Complejidad Temporal: O(n), donde n es la mitad de la longitud del buffer (len/2), 
 *   recorriendo los elementos una sola vez de forma simétrica.
 * - Complejidad Espacial: O(1), ya que solo se emplean dos punteros auxiliares (start, end) 
 *   y una variable temporal de tipo char, requiriendo memoria adicional constante independiente de n.
 */
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Valores NULL, buffers vacios y longitudes impares

### Salida del harness
```text
[TEST 1] Verificación de puntero NULL (str = NULL, len = 5): PASSED
[TEST 2] Verificación de buffer vacío o longitud 1 (len = 0, len = 1): PASSED
[TEST 3] Verificación de longitudes pares e impares ("1234" -> "4321", "abcde" -> "edcba"): PASSED
RESULTADO: 3/3 pruebas superadas
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> En su primera iteración, la IA omitió validar explícitamente si el puntero recibido era NULL, lo cual habría provocado un fallo de segmentación (Segmentation Fault) al evaluar la operación aritmética str + len - 1 si se le pasaba un puntero nulo con una longitud mayor a cero. Para solucionarlo, integré una guarda de seguridad al inicio de la función (if (str == NULL || len <= 1) return;), asegurando que el programa maneje de forma robusta entradas inválidas sin comprometer la estabilidad del sistema.

---
**Firma digital:** Juanp-Alvarado
