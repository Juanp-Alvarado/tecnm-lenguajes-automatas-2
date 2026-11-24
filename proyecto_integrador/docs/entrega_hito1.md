# Entrega · Hito 1

**Equipo:**
- Juan Pablo Alvarado Martinez
- Diego Reyes Martinez
- Lesli Tamara Canul Llanes
- Pablo Uriel Desiderio Lopez
- Ricardo Hernandez Naranjo

**Repositorio:** https://github.com/Juanp-Alvarado/tecnm-lenguajes-automatas-2
**Tag:** `v0.1-hito1`
**Fecha:** 05/11/2026

---

## 1. Qué funciona

Hito 1: léxico, sintaxis y AST de expresiones.

```bash
cd proyecto_integrador
make clean && make
./lenguajito tests/validos/01_suma.lj --tokens
./lenguajito tests/validos/01_suma.lj --ast
./lenguajito tests/validos/01_suma.lj --check
./lenguajito tests/invalidos/01_no_declarada.lj --check
./lenguajito tests/invalidos/05_sintaxis.lj --check
```

- `make` compila sin warnings con `-std=c11 -Wall -Wextra -Werror`.
- `--tokens` imprime `linea:columna TIPO lexema`, un token por línea. Para `print(1 + 2 * 3);`:

```text
1:1 KW print
1:6 DELIM (
1:7 NUM 1
1:9 OP +
1:11 NUM 2
1:13 OP *
1:15 NUM 3
1:16 DELIM )
1:17 DELIM ;
2:1 EOF
```

- `--ast` respeta la precedencia (la multiplicación queda dentro de la suma):

```text
(programa (imprimir (+ (num 1) (* (num 2) (num 3)))))
```

- `--check` acepta el programa válido: no imprime nada y termina con código 0.
- `--check` reporta con archivo, línea y columna los errores de los programas inválidos:

```text
tests/invalidos/01_no_declarada.lj:1:7: error E01: variable no declarada 'x'
tests/invalidos/05_sintaxis.lj:1:9: error S01: se esperaba una expresion
```

## 2. Qué no funciona (y por qué)

- [ ] `--check` no detecta la redeclaración de variables (`tests/invalidos/02_redeclaracion.lj`): el programa se acepta sin reportar el `E02` esperado en la línea 2. Requiere la tabla de símbolos con ámbitos, que corresponde al Hito 2.
- [ ] `--check` no detecta la división entre cero constante (`tests/invalidos/04_div_cero.lj`): el programa se acepta sin reportar `E04` (esperado en la línea 1, columna 12, según `docs/gramatica.md`). Pendiente; la guía no asigna este diagnóstico a un hito concreto, por lo que se confirmará con el docente.
- [ ] `--run` (ejecución) no está en el alcance del Hito 1; se hace en el Hito 2.
- [ ] Tabla de símbolos con ámbitos anidados y shadowing: pendiente (Hito 2).
- [ ] Reporte de varios errores en una sola pasada: pendiente (Hito 2).

## 3. Pruebas

Salida de `make test-hito1`:

```text
python3 tests/run.py --hito1

Hito 1: 4/4 pruebas superadas
```

Pruebas propias en `tests/propias/` (7 casos + runner `run_propias.py`; la carpeta `tests/` del curso no se modificó):

| Archivo | Qué cubre |
| :--- | :--- |
| `01_precedencia` | Precedencia de operadores (`*` antes que `+`) |
| `02_comparacion` | Operadores de comparación en el AST |
| `03_sin_expresion` | Error de sintaxis cuando falta una expresión (`.err`) |
| `04_asociatividad` | Asociatividad de operadores del mismo nivel |
| `05_unario` | Operadores unarios |
| `06_parentesis` | Agrupación con paréntesis |
| `07_sin_init` | Declaración sin inicializador |

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
| Redactar este documento de entrega con la plantilla del curso | Asistente de documentación técnica (Claude) | Llenar la plantilla a partir del repositorio, sin inventar datos que no pudiera verificar | Borrador con las secciones 1 a 6; dejó marcados como pendientes los datos que no pudo ver | Se aceptó la estructura y la alerta de que el tag apuntaba a un commit viejo. Se completaron las secciones 1 a 4 con salidas reales del equipo. Se verificó el reparto contra los autores de los commits en lugar de asignarlo sin evidencia. |
| ⚠️ COMPLETAR | | | | |

## 6. Demo

- [ ] Preparada para 3 minutos: `--tokens`, `--ast` y `--check` sobre un programa válido y uno inválido.
- [ ] Cualquier integrante puede explicar cualquier parte.
