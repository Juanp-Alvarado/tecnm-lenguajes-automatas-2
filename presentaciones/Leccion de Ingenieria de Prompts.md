# Lección de Prompt Engineering: El Método CREO para Programadores

En la ingeniería de software moderna, interactuar con Inteligencia Artificial no es cuestión de hacer preguntas casuales, sino de **diseñar instrucciones precisas**. Para lograr resultados profesionales en programación —evitando código genérico, errores de sintaxis o respuestas incompletas— utilizaremos el marco **CREO**:

*   **C - Contexto:** El entorno, lenguaje, versión, restricciones técnicas o el problema de fondo.
*   **R - Rol:** La perspectiva o nivel de experiencia que la IA debe adoptar (ej. *Arquitecto de Software Senior*, *Experto en C/C++*, *Auditor de Seguridad*).
*   **Expectativa:** La tarea exacta que debe realizar (ej. refactorizar, depurar, diseñar una estructura de datos).
*   **Salida (Output):** El formato exacto en el que deseas recibir la respuesta (ej. bloque de código limpio, tabla comparativa, explicación paso a paso sin rodeos).

---

## Nivel 1 (Básico): Generación de Código y Explicación

En este nivel inicial, aplicamos CREO para crear funciones limpias y entender su funcionamiento básico.

### Caso de Uso: Escribir una función en C para validar strings.

#### Prompt Estructurado con CREO:
> **[Contexto]** Estoy desarrollando un analizador léxico en C11 para un compilador educativo. Necesito validar identificadores según las reglas estándar de C (deben empezar con letra o guion bajo y contener solo alfanuméricos).
> **[Rol]** Actúa como un Ingeniero de Software experto en C y buenas prácticas de codificación defensiva.
> **[Expectativa]** Escribe una función pura en C llamada `is_valid_identifier` que reciba un `const char *str` y devuelva `1` si es válido o `0` en caso contrario. Incluye manejo de punteros nulos.
> **[Salida]** Proporciona únicamente el bloque de código comentado en C y, debajo, una breve explicación de 3 líneas sobre cómo maneja los casos límite.

#### Resultado Esperado del Prompt:
Un código limpio, directo, acotado a lo que pediste y con el formato de salida estipulado.

---

## Nivel 2 (Intermedio): Depuración y Optimización (*Debugging*)

Aquí el reto no es crear desde cero, sino resolver un problema complejo dentro de un sistema existente (como detectar fugas de memoria o mejorar la complejidad algorítmica).

### Caso de Uso: Optimizar una lista enlazada a $O(1)$ en C.

#### Prompt Estructurado con CREO:
> **[Contexto]** Tengo una lista enlazada simple en C utilizada para almacenar tokens. Actualmente la función de inserción `token_list_append` recorre toda la lista hasta el final, lo que genera una complejidad $O(n)$ inaceptable para archivos fuente grandes.
> **[Rol]** Actúa como un Especialista en Rendimiento y Estructuras de Datos en sistemas Unix/Linux.
> **[Expectativa]** Modifica la estructura de control de la lista para incorporar un puntero al último elemento (`tail`), logrando que las inserciones sean de complejidad $O(1)$. Reescribe la función de inserción y la de creación.
> **[Salida]** Devuelve las funciones modificadas en C, seguidas de un bloque de comandos `bash` que muestre cómo compilar con GCC y validar con `valgrind` que no existan fugas de memoria.

---

## Nivel 3 (Avanzado): Diseño de Arquitectura y Sistemas

En este nivel superior, utilizas la IA para diseñar sistemas completos con múltiples componentes, gestión de ámbitos y patrones de diseño estrictos (como tablas de símbolos jerárquicas o ASTs).

### Caso de Uso: Implementar una Tabla de Símbolos con Resolución de Ámbitos en C++.

#### Prompt Estructurado con CREO:
> **[Contexto]** En la fase de análisis semántico de mi compilador necesito gestionar variables anidadas con control de ocultamiento (*shadowing*). Cada bloque `{ ... }` genera un nuevo ámbito hijo que apunta a su ámbito padre.
> **[Rol]** Actúa como un Arquitecto de Compiladores Senior especializado en C++ moderno (C++17).
> **[Expectativa]** Diseña una clase `Scope` utilizando `std::shared_ptr` para el encadenamiento de ámbitos y `std::vector` de buckets para las colisiones de la tabla hash interna. Debe incluir métodos de inserción con detección de redeclaración y búsqueda recursiva ascendente.
> **[Salida]** Presenta el archivo de cabecera (`symbol_table.hpp`) completo, documentado, y un ejemplo ejecutable en `main()` que demuestre una prueba de *shadowing* entre un ámbito global y uno local.

---

## Ejercicio Práctico para los Alumnos

Diseña tu propio prompt utilizando el formato **CREO** para resolver el siguiente escenario:

*   **Escenario:** Necesitas que la IA revise un archivo en C++ buscando posibles divisiones por cero en expresiones matemáticas complejas antes de evaluar el AST.
*   **Tu tarea:** Escribe el prompt detallando el **C**ontexto, el **R**ol que asumirá la IA, la **E**xpectativa exacta y el formato de **O**utput que requieres.
