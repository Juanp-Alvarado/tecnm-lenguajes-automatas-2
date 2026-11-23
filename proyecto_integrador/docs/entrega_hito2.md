# Entrega · Hito 1

**Equipo:**
- Juan Pablo Alvarado Martinez
- Diego Reyes Martinez
- Lesli Tamara Canul Llanes
- Pablo Uriel Desiderio Lopez
- Ricardo Hernandez Naranjo

**Repositorio:** https://github.com/Juanp-Alvarado/tecnm-lenguajes-automatas-2
**Tag:** `v0.2-hito2`
**Fecha:** 05/11/2026

---

## 1. Qué funciona

Hito 1: léxico, sintaxis y AST de expresiones.

```bash
make clean && make
make test-hito1
make test-hito2
./lenguajito tests/validos/01_suma.lj --tokens
./lenguajito tests/validos/01_suma.lj --ast
./lenguajito tests/validos/01_suma.lj --check
./lenguajito tests/validos/01_suma.lj --run
./lenguajito tests/validos/01_suma.lj --tac
```

make compila todos los src/*.c y src/*.h con -std=c11 -Wall -Wextra -Werror -O2 generando el binario lenguajito sin advertencias.

--tokens (modo_tokens en compilador.c + lexer.c) imprime linea:columna TIPO lexema por token y linea:columna EOF al final, reportando errores léxicos S01.

--ast (parser.c + ast.c) construye el árbol sintáctico con precedencia de operadores, lo imprime en una línea con ast_imprimir y libera la memoria con ast_liberar.

--check (parser.c + semantica.c) valida sintaxis (S01) y análisis semántico con ámbitos anidados (push_scope / pop_scope), detectando E01 (variable no declarada o leída sin valor), E02 (redeclaración en el mismo ámbito) y E04 (división entre cero constante).

--run (interprete.c, conectado en compilador.c) ejecuta el AST tras validar sintaxis y semántica, manejando ámbitos (push_scope / pop_scope), shadowing, evaluación de expresiones y sentencias (if, else, while, print).

--tac (tac.c, conectado en compilador.c) genera código de tres direcciones con temporales _t0, _t1, ..., etiquetas L1, L2, ..., saltos (goto, ifFalso) y plegado de constantes (plegar).
```

## 2. Qué no funciona (y por qué)

[ ] Reporte ilimitado o multi-pasada más allá del tope: tanto parser.c como semantica.c limitan el diagnóstico a un máximo de 20 errores (MAX_ERRORES = 20 / p->errores < 20).

[ ] Modos posteriores como --asm (y el alcance formal/pruebas de --tac según el cronograma del curso): main.c reconoce --asm como modo válido, pero compilador.c lo marca como no implementado en este hito.

## 3. Pruebas

Salida de `make test-hito1`:

```text
python3 tests/run.py --hito2

Hito 2: 14/14 pruebas superadas
```

Pruebas propias en `tests/propias/` (7 casos + runner `run_propias.py`; la carpeta `tests/` del curso no se modificó):

tests/propias/01_precedencia: precedencia de operadores (* antes que +).

tests/propias/02_comparacion: operadores de comparación en el AST.

tests/propias/03_sin_expresion: error de sintaxis cuando falta una expresión (.err).

tests/propias/04_asociatividad: asociatividad de operadores del mismo nivel.

tests/propias/05_unario: operadores unarios.

tests/propias/06_parentesis: agrupación con paréntesis.

tests/propias/07_sin_init: declaración sin inicializador.

## 4. Reparto de trabajo

| Integrante | Archivos / etapas | Commits principales |
| :--- | :--- | :--- |
| Juan Pablo Alvarado Martinez (`Juanp-Alvarado`) | Lexer (`lexer.c`) y `compilador.c` para `--tokens`; integración de PRs; `avances.md` | `6b1e2c6`, `9cc52be`, `b147b0f`, `046e1eb`, `21209cc`, `324c0df` |
| Lesli Tamara Canul Llanes (`LeslieTamara14`) | Parser y AST: `parser.c/.h`, `ast.c/.h`; ajustes al lexer; cuaderno de pruebas del parser | `61cf2ea`, `da8846d`, `28a838e`, `5f6439b`, `a278ed0`, `28fdc09` |
| Pablo Uriel Desiderio Lopez (`pablouriellopez68-sketch`) | Modo `--check`; runner y 7 pruebas propias | `da1c23e`, `fca69e6` |
| Diego Reyes Martinez (`ReyesDiego963`) | `--tokens` con el formato de la especificación, `--check` y análisis semántico (E01) | `ab91cfa` |
| Ricardo Hernandez Naranjo (`henari2806-ai`) | Auditorias y Avances | `a31c74a` (plantilla de entrega) |

## 5. Bitácora C-R-E-O (IA)

| Contexto | Rol | Especificación | Output | ¿Qué aceptaron, corrigieron o rechazaron? |
| :--- | :--- | :--- | :--- | :--- |
| Compilación y ejecución de pruebas del compilador en WSL | Asistente técnico de desarrollo en C11 y Make | Explicar el `Makefile` y diagnosticar fallos al compilar o correr `make test-hito1` | Desglose del `Makefile` e indicación de ejecutar los comandos dentro de `proyecto_integrador/` | Se aceptó la explicación del `Makefile` y se aplicó el cambio de directorio a `proyecto_integrador/`, logrando compilar y pasar las pruebas del Hito 1. |
| Preparación para la defensa de la demo del Hito 2 | Tutor técnico de compiladores en C11 | Explicar estructuras, funciones y responsabilidades de `parser`, `ast`, `semantica`, `interprete`, `tac`, `compilador` y `main`, además de estructurar la bitácora | Análisis módulo por módulo, propuestas de reparto para Hito 2 y formato de bitácora | Se aceptaron las explicaciones técnicas (`push_scope`/`pop_scope`, `E01`/`E02`/`E04`, `--run` y plegado de constantes) y se verificó la ejecución de las 14/14 pruebas del Hito 2. |

## 6. Demo

- [ ] Preparada para 3 minutos: `--tokens`, `--ast` y `--check` sobre un programa válido y uno inválido.
- [ ] Cualquier integrante puede explicar cualquier parte.
