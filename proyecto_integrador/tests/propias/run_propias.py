#!/usr/bin/env python3
"""Ejecuta las pruebas propias de Lenguajito.

Colocar en: proyecto_integrador/tests/propias/run_propias.py

Uso (desde proyecto_integrador/):
    python3 tests/propias/run_propias.py           # todos los casos
    python3 tests/propias/run_propias.py 01_       # solo los que empiecen asi

Convencion de archivos (un caso = un .lj + su archivo esperado):

    caso.lj + caso.out      -> se corre con --run    y se compara stdout exacto
    caso.lj + caso.ast      -> se corre con --ast    y se compara ignorando
                               espacios al inicio/final
    caso.lj + caso.tokens   -> se corre con --tokens y se compara linea por linea
    caso.lj + caso.err      -> se corre con --check; el .err tiene "LINEA CODIGO"
                               (por ejemplo: 2 E01) y se exige salida 1 y el
                               diagnostico en esa linea

Si un .lj no tiene ningun archivo esperado, se reporta como caso incompleto.
"""
from __future__ import annotations

import re
import subprocess
import sys
from pathlib import Path

RAIZ = Path(__file__).resolve().parent          # tests/propias/
BIN = RAIZ.parent.parent / "lenguajito"         # proyecto_integrador/lenguajito

VERDE = "\033[32m"
ROJO = "\033[31m"
GRIS = "\033[90m"
FIN = "\033[0m"


def ejecutar(archivo: Path, modo: str) -> subprocess.CompletedProcess:
    """Llama al compilador igual que lo hace tests/run.py: BIN ARCHIVO MODO."""
    return subprocess.run(
        [str(BIN), str(archivo), modo],
        capture_output=True,
        text=True,
    )


def revisar_salida(prog: Path, errores: list[str]) -> None:
    """caso.out: la salida de --run debe ser identica, byte a byte."""
    esperado = prog.with_suffix(".out").read_text(encoding="utf-8")
    r = ejecutar(prog, "--run")
    if r.returncode != 0:
        errores.append(f"{prog.name}: --run termino con {r.returncode}\n"
                       f"  stderr: {r.stderr.strip()!r}")
    elif r.stdout != esperado:
        errores.append(
            f"{prog.name}: salida distinta\n"
            f"  esperado: {esperado!r}\n  obtenido: {r.stdout!r}"
        )


def revisar_ast(prog: Path, errores: list[str]) -> None:
    """caso.ast: el AST en una sola linea, comparado sin espacios sobrantes."""
    esperado = prog.with_suffix(".ast").read_text(encoding="utf-8").strip()
    r = ejecutar(prog, "--ast")
    if r.returncode != 0:
        errores.append(f"{prog.name}: --ast termino con {r.returncode}\n"
                       f"  stderr: {r.stderr.strip()!r}")
    elif r.stdout.strip() != esperado:
        errores.append(
            f"{prog.name}: AST distinto\n"
            f"  esperado: {esperado}\n  obtenido: {r.stdout.strip()}"
        )


def revisar_tokens(prog: Path, errores: list[str]) -> None:
    """caso.tokens: una linea por token, formato LINEA:COLUMNA TIPO lexema."""
    esperado = prog.with_suffix(".tokens").read_text(encoding="utf-8").split("\n")
    r = ejecutar(prog, "--tokens")
    if r.returncode != 0:
        errores.append(f"{prog.name}: --tokens termino con {r.returncode}\n"
                       f"  stderr: {r.stderr.strip()!r}")
        return
    obtenido = r.stdout.split("\n")
    esperado = [l for l in esperado if l.strip()]
    obtenido = [l for l in obtenido if l.strip()]
    if len(esperado) != len(obtenido):
        errores.append(
            f"{prog.name}: se esperaban {len(esperado)} tokens y salieron {len(obtenido)}"
        )
        return
    for i, (esp, obt) in enumerate(zip(esperado, obtenido), start=1):
        if esp.split() != obt.split():
            errores.append(
                f"{prog.name}: token {i} distinto\n"
                f"  esperado: {esp}\n  obtenido: {obt}"
            )
            return


def revisar_error(prog: Path, errores: list[str]) -> None:
    """caso.err: contiene 'LINEA CODIGO'; --check debe salir 1 y reportarlo."""
    contenido = prog.with_suffix(".err").read_text(encoding="utf-8").split()
    if len(contenido) != 2:
        errores.append(f"{prog.name}: el .err debe tener 'LINEA CODIGO', por ejemplo '2 E01'")
        return
    linea, codigo = contenido
    r = ejecutar(prog, "--check")
    patron = rf":{linea}:\d+: error {codigo}:"
    if r.returncode != 1:
        errores.append(f"{prog.name}: --check debio salir con 1, salio con {r.returncode}")
    elif not re.search(patron, r.stdout):
        errores.append(
            f"{prog.name}: se esperaba '{codigo}' en la linea {linea}\n"
            f"  obtenido en stdout: {r.stdout!r}\n"
            f"  (recuerda: los diagnosticos van a stdout, no a stderr)"
        )


REVISIONES = [
    (".out", revisar_salida),
    (".ast", revisar_ast),
    (".tokens", revisar_tokens),
    (".err", revisar_error),
]


def main() -> int:
    if not BIN.exists():
        print(f"{ROJO}X{FIN} No existe {BIN}. Ejecuta `make` primero.")
        return 1

    filtro = sys.argv[1] if len(sys.argv) > 1 else ""
    casos = sorted(p for p in RAIZ.glob("*.lj") if p.name.startswith(filtro))

    if not casos:
        print(f"No hay casos .lj en {RAIZ}"
              + (f" que empiecen con '{filtro}'" if filtro else ""))
        return 0

    errores: list[str] = []
    total = 0

    for prog in casos:
        esperados = [(ext, fn) for ext, fn in REVISIONES
                     if prog.with_suffix(ext).exists()]
        if not esperados:
            print(f"{GRIS}-{FIN} {prog.name}: sin archivo esperado (.out/.ast/.tokens/.err)")
            continue
        for _, revision in esperados:
            total += 1
            antes = len(errores)
            revision(prog, errores)
            marca = f"{ROJO}X{FIN}" if len(errores) > antes else f"{VERDE}OK{FIN}"
            print(f"{marca} {prog.name} [{_[1:]}]")

    if errores:
        print()
        for error in errores:
            print(f"{ROJO}X{FIN} {error}")

    print(f"\nPruebas propias: {total - len(errores)}/{total} superadas")
    return 0 if not errores else 1


if __name__ == "__main__":
    sys.exit(main())