#!/usr/bin/env python3
"""
Plantilla única de las presentaciones del curso.

Ocho láminas, en este orden:

  1. Portada: tema, pregunta guía y objetivo de la sesión.
  2. Conceptos básicos en lenguaje llano (con analogías), ANTES de los tecnicismos.
  3. ¿Dónde estamos y a dónde vamos? Mapa de las 4 unidades y los 4 hitos.
  4. Fundamentos teóricos (contenido del bloque).
  5. Arquitectura de memoria (contenido del bloque).
  6. Casos borde y antipatrones (contenido del bloque).
  7. Lo que vas a construir hoy: entregable, criterios y salida esperada del harness.
  8. Dinámica de la clase y protocolo de entrega.

La lámina 7 es la que responde "¿cómo se verá el resultado al final?".
"""

from __future__ import annotations

import html
import re

from lab_utils import expected_output


def fmt(texto: str) -> str:
    """Escapa HTML y convierte `texto` entre acentos graves en <code>."""
    escapado = html.escape(texto, quote=False)
    return re.sub(r"`([^`]+)`", r"<code>\1</code>", escapado)


def _tarjeta(titulo: str, cuerpo: str, detalle: str, color: str) -> str:
    return f"""
        <div class="card" onclick="toggleCard(this)">
          <div class="card-header"><span style="color: {color};">{fmt(titulo)}</span><span class="expand-hint">[+]</span></div>
          <p>{fmt(cuerpo)}</p>
          <div class="card-details">{fmt(detalle)}</div>
        </div>"""


def _resaltar(codigo: str) -> str:
    """Resaltado mínimo: comentarios, cadenas, palabras clave y números."""
    texto = html.escape(codigo, quote=False)
    guardados: list[str] = []

    def guardar(m):
        guardados.append(m.group(0))
        return f"@@{chr(65 + len(guardados) - 1)}@@"

    texto = re.sub(r"/\*.*?\*/|//[^\n]*|#[^\n]*", guardar, texto, flags=re.S)
    texto = re.sub(r'"(?:\\.|[^"\\])*"', guardar, texto)
    texto = re.sub(
        r"\b(int|char|void|long|unsigned|const|struct|typedef|enum|static|"
        r"return|if|else|while|for|switch|case|break|default|size_t)\b",
        r'<span class="kw">\1</span>',
        texto,
    )
    texto = re.sub(r"\b(\d+)\b", r'<span class="num">\1</span>', texto)

    def restaurar(m):
        token = guardados[ord(m.group(1)) - 65]
        clase = "com" if token.startswith(("/*", "//", "#")) else "str"
        return f'<span class="{clase}">{token}</span>'

    return re.sub(r"@@([A-Z])@@", restaurar, texto)


def _bloques_stub(stub: str) -> list[str]:
    """Separa el esqueleto en bloques de función (de la firma a la llave final)."""
    lineas = stub.splitlines()
    bloques: list[str] = []
    i = 0
    while i < len(lineas):
        if re.match(r"^[A-Za-z_].*\(.*\)\s*\{?\s*$", lineas[i]) and not lineas[i].startswith("#"):
            j = i
            while j < len(lineas) and lineas[j].strip() != "}":
                j += 1
            if j < len(lineas):
                bloques.append("\n".join(lineas[i:j + 1]))
                i = j + 1
                continue
        i += 1
    return bloques


def _lab_codigo(lab: dict) -> str:
    """Contrato del laboratorio + el esqueleto de la función que deben escribir."""
    lineas = [f"/* {lab['api']['header']} */"]
    lineas += list(lab["api"].get("tipos", []))
    decls = lab["api"]["decls"]
    lineas.append(decls[0])
    if len(decls) > 1:
        lineas.append(f"/* + {len(decls) - 1} firma(s) más en la cabecera */")
    lineas.append("")
    lineas.append("/* tu archivo, por ahora */")

    nombres = []
    for decl in lab["api"]["decls"]:
        m = re.search(r"(\w+)\s*\(", decl)
        if m:
            nombres.append(m.group(1))

    bloques = _bloques_stub(lab["stub"])
    elegido = None
    for bloque in bloques:
        if "TODO" in bloque and any(re.search(rf"\b{nom}\s*\(", bloque) for nom in nombres):
            elegido = bloque
            break
    if elegido is None:
        for bloque in bloques:
            if "TODO" in bloque:
                elegido = bloque
                break
    if elegido is None:
        elegido = bloques[0] if bloques else lab["api"]["decls"][0]

    # Si el bloque usa un tipo definido en el propio esqueleto, muéstralo.
    for typedef in lab["stub"].splitlines():
        if typedef.startswith("typedef") and typedef.rstrip().endswith(";"):
            m = re.search(r"}\s*(\w+);", typedef)
            if m and re.search(rf"\b{m.group(1)}\b", elegido):
                lineas.append(typedef)

    stub_lineas = elegido.splitlines()
    if len(stub_lineas) > 6:
        stub_lineas = stub_lineas[:6] + ["/* ... */"]
    lineas += stub_lineas
    return "\n".join(lineas)


def render(num: int, data: dict, unidades: list[dict], total: int = 40,
           siguiente_tema: str | None = None) -> str:
    n = f"{num:02d}"
    tema = data["tema"]
    unidad = data["unidad"]
    tp = data["teoria_principal"]
    am = data["arquitectura_memoria"]
    cb = data["casos_borde"]
    ctx = data["contexto"]
    lab = data["lab"]

    # --- Lámina 2: conceptos básicos -------------------------------------
    tarjetas_conceptos = ""
    for i, (termino, definicion, analogia) in enumerate(ctx["conceptos"], start=1):
        tarjetas_conceptos += f"""
        <div class="card static">
          <div class="card-header"><span style="color: var(--accent);">{i:02d}. {fmt(termino)}</span></div>
          <p>{fmt(definicion)}</p>
          <p class="analogia"><strong>Analogía:</strong> {fmt(analogia)}</p>
        </div>"""

    # --- Lámina 3: mapa de unidades --------------------------------------
    filas_mapa = ""
    for u in unidades:
        clases = "tl-item"
        if u["nombre"] == unidad:
            clases += " current"
        elif u["hasta"] < num:
            clases += " done"
        marca = "Aquí estás" if u["nombre"] == unidad else u["hito"]
        filas_mapa += f"""
        <div class="{clases}">
          <div class="tl-unit">{u['rango']}</div>
          <div class="tl-title">{fmt(u['nombre'])}</div>
          <div class="tl-hito">{marca}</div>
        </div>"""

    if siguiente_tema and num < total:
        despues = f"{fmt(siguiente_tema)} (sesión {num + 1:02d})"
    else:
        despues = "Cierre del curso: demostración del compilador completo."

    # --- Láminas 4-6: contenido teórico ----------------------------------
    cards_s2 = "".join(
        _tarjeta(t, d, det, "var(--accent)") for t, d, det in tp["tarjetas"]
    )
    cards_s3 = "".join(
        _tarjeta(t, d, det, "var(--accent-silver)") for t, d, det in am["tarjetas"]
    )
    items_s4 = "".join(f"<li>{fmt(item)}</li>" for item in cb["items"])

    # --- Lámina 7: código del laboratorio y del proyecto ------------------
    lab_code = _resaltar(_lab_codigo(lab))
    proy_code = _resaltar(data["codigo_proyecto"])
    proy_nota = fmt(data["codigo_nota"])

    # --- Lámina 8: laboratorio -------------------------------------------
    criterios = "".join(f"<li>{fmt(c)}</li>" for c in lab["criterios"])
    salida = html.escape(expected_output(num, lab).rstrip("\n"), quote=False)
    reglas = "".join(f"<li>{fmt(r)}</li>" for r in lab.get("reglas", []))

    return f"""<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Sesión {n}: {html.escape(tema)} | Lenguajes y Autómatas II</title>
  <style>
    :root {{
      --bg: #f4f8fb;
      --card-bg: rgba(255, 255, 255, 0.88);
      --card-bg-hover: rgba(255, 255, 255, 0.98);
      --border: #d0dbe5;
      --border-focus: #0969da;
      --accent: #0969da;
      --accent-green: #1a7f37;
      --accent-silver: #627282;
      --accent-orange: #d29922;
      --text: #334155;
      --text-bright: #0f172a;
      --font-mono: 'Fira Code', monospace;
    }}
    * {{ box-sizing: border-box; margin: 0; padding: 0; }}
    body {{
      background-color: var(--bg);
      color: var(--text);
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
      height: 100vh;
      overflow: hidden;
      display: flex;
      flex-direction: column;
    }}
    header {{
      padding: 12px 28px;
      background: rgba(255, 255, 255, 0.85);
      backdrop-filter: blur(8px);
      border-bottom: 1px solid var(--border);
      display: flex;
      justify-content: space-between;
      align-items: center;
    }}
    .brand {{ font-weight: 700; color: var(--text-bright); font-size: 0.95rem; }}
    .tag {{
      background-color: #ddf4ff;
      color: var(--accent);
      padding: 4px 12px;
      border-radius: 12px;
      font-size: 0.75rem;
      font-family: var(--font-mono);
      font-weight: 600;
      border: 1px solid #b6e3ff;
    }}
    main {{ flex: 1; display: flex; position: relative; overflow: hidden; }}
    .slide {{
      position: absolute;
      inset: 0;
      padding: 26px 56px;
      display: flex;
      flex-direction: column;
      justify-content: center;
      opacity: 0;
      visibility: hidden;
      transform: translateX(30px);
      transition: all 0.3s ease;
      overflow-y: auto;
    }}
    .slide.active {{ opacity: 1; visibility: visible; transform: translateX(0); }}
    h1 {{ font-size: 2rem; color: var(--text-bright); margin-bottom: 10px; }}
    h2 {{ font-size: 1.45rem; color: var(--text-bright); margin-bottom: 14px; }}
    p.lead {{ font-size: 1.02rem; line-height: 1.5; max-width: 1000px; margin-bottom: 16px; color: #475569; }}
    .pregunta {{
      font-size: 1.2rem;
      line-height: 1.45;
      color: var(--text-bright);
      font-weight: 600;
      border-left: 4px solid var(--accent);
      padding: 8px 0 8px 16px;
      margin-bottom: 18px;
      max-width: 980px;
    }}
    .grid-3 {{ display: grid; grid-template-columns: repeat(3, 1fr); gap: 16px; }}
    .grid-2 {{ display: grid; grid-template-columns: repeat(2, 1fr); gap: 16px; }}
    .grid-lab {{ display: grid; grid-template-columns: 0.82fr 1.18fr; gap: 16px; }}
    .card {{
      background: var(--card-bg);
      border: 1px solid var(--border);
      border-radius: 10px;
      padding: 16px;
      cursor: pointer;
      transition: all 0.25s ease;
      display: flex;
      flex-direction: column;
    }}
    .card:hover {{ background: var(--card-bg-hover); border-color: var(--border-focus); transform: translateY(-2px); }}
    .card.static {{ cursor: default; }}
    .card.static:hover {{ background: var(--card-bg); border-color: var(--border); transform: none; }}
    .card.expanded {{ border-color: var(--accent); background: #ffffff; }}
    .card-header {{ font-weight: 600; color: var(--text-bright); margin-bottom: 8px; display: flex; justify-content: space-between; }}
    .expand-hint {{ font-size: 0.75rem; color: var(--accent); font-family: var(--font-mono); }}
    .card p, .card li {{ font-size: 0.86rem; line-height: 1.45; color: var(--text); }}
    .card ul {{ padding-left: 18px; }}
    .card li {{ margin-bottom: 4px; }}
    .card-details {{ max-height: 0; overflow: hidden; opacity: 0; transition: all 0.3s ease; font-size: 0.8rem; color: #475569; }}
    .card.expanded .card-details {{ max-height: 260px; opacity: 1; margin-top: 10px; padding-top: 10px; border-top: 1px dashed var(--border); }}
    .analogia {{
      margin-top: 10px;
      padding-left: 10px;
      border-left: 3px solid var(--accent-orange);
      font-size: 0.82rem !important;
      color: #475569 !important;
    }}
    .interactive-box {{
      margin-top: 16px;
      padding: 13px 18px;
      background: #ffffff;
      border: 1px solid var(--border);
      border-left: 4px solid var(--accent);
      border-radius: 6px;
      font-size: 0.86rem;
      line-height: 1.5;
    }}
    .interactive-box code, .card code, li code, p code {{
      background: #eef2f7;
      padding: 1px 5px;
      border-radius: 4px;
      font-family: var(--font-mono);
      font-size: 0.86em;
    }}
    .timeline {{ display: grid; grid-template-columns: repeat(4, 1fr); gap: 10px; margin-bottom: 4px; }}
    .tl-item {{ border: 1px solid var(--border); border-radius: 8px; padding: 10px 12px; background: #ffffff; }}
    .tl-item.current {{ border-color: var(--accent); box-shadow: 0 0 0 2px #ddf4ff; }}
    .tl-item.done {{ opacity: 0.6; }}
    .tl-unit {{ font-size: 0.68rem; font-family: var(--font-mono); color: var(--accent-silver); }}
    .tl-title {{ font-size: 0.8rem; font-weight: 600; color: var(--text-bright); margin: 3px 0; line-height: 1.25; }}
    .tl-hito {{ font-size: 0.68rem; color: var(--accent-orange); font-family: var(--font-mono); }}
    .terminal {{ align-self: start; background: #0d1117; border: 1px solid #30363d; border-radius: 10px; overflow: hidden; }}
    .terminal-bar {{ background: #161b22; color: #8b949e; font-family: var(--font-mono); font-size: 0.68rem; padding: 6px 12px; border-bottom: 1px solid #30363d; }}
    .terminal pre {{ margin: 0; padding: 12px 14px; color: #7ee787; font-family: var(--font-mono); font-size: 0.62rem; line-height: 1.45; white-space: pre; overflow-x: auto; }}
    .code-box {{ align-self: start; background: #0d1117; border: 1px solid #30363d; border-radius: 10px; overflow: hidden; }}
    .code-head {{ background: #161b22; color: #8b949e; font-family: var(--font-mono); font-size: 0.66rem; padding: 6px 12px; border-bottom: 1px solid #30363d; }}
    pre.code {{ margin: 0; padding: 10px 12px; color: #e6edf3; font-family: var(--font-mono); font-size: 0.58rem; line-height: 1.45; white-space: pre-wrap; overflow-wrap: anywhere; }}
    pre.code .kw {{ color: #79c0ff; }}
    pre.code .num {{ color: #d2a8ff; }}
    pre.code .str {{ color: #ffa657; }}
    pre.code .com {{ color: #7ee787; font-style: italic; }}
    .code-note {{ margin: 0; padding: 8px 12px; border-top: 1px solid #30363d; color: #8b949e; font-size: 0.7rem; line-height: 1.35; }}
    footer {{
      padding: 12px 28px;
      background: rgba(255, 255, 255, 0.85);
      border-top: 1px solid var(--border);
      display: flex;
      justify-content: space-between;
      align-items: center;
    }}
    .progress {{ font-size: 0.85rem; font-family: var(--font-mono); color: var(--accent); font-weight: 600; }}
    .controls {{ display: flex; gap: 10px; }}
    button {{
      background: #ffffff;
      color: var(--text-bright);
      border: 1px solid var(--border);
      padding: 7px 18px;
      border-radius: 6px;
      cursor: pointer;
      font-weight: 600;
    }}
    button:hover {{ background: #f1f5f9; }}
    button:disabled {{ opacity: 0.4; cursor: not-allowed; }}
  </style>
</head>
<body>
  <header>
    <div class="brand">TecNM Cancún &bull; SCD-1016</div>
    <div class="tag">SESIÓN {n} / {total} &bull; {html.escape(tema)}</div>
  </header>

  <main>
    <!-- Lámina 1: de qué trata la sesión -->
    <section class="slide active">
      <div class="tag" style="width: fit-content; margin-bottom: 10px;">{fmt(unidad.upper())}</div>
      <h1>{fmt(tema)}</h1>
      <p class="pregunta">{fmt(ctx["pregunta"])}</p>
      <div class="grid-2">
        <div class="card static">
          <div class="card-header"><span style="color: var(--accent);">Objetivo de hoy</span></div>
          <p>{fmt(ctx["objetivo"])}</p>
        </div>
        <div class="card static">
          <div class="card-header"><span style="color: var(--accent-green);">Al terminar tendrás</span></div>
          <p>{fmt(ctx["resultado"])}</p>
        </div>
      </div>
    </section>

    <!-- Lámina 2: conceptos básicos antes de los tecnicismos -->
    <section class="slide">
      <h2>Antes de los tecnicismos: los conceptos básicos</h2>
      <p class="lead">Si te quedas con estas ideas, el resto de la sesión se entiende solo.</p>
      <div class="grid-2">
        {tarjetas_conceptos}
      </div>
    </section>

    <!-- Lámina 3: mapa del curso -->
    <section class="slide">
      <h2>¿Dónde estamos y a dónde vamos?</h2>
      <p class="lead">{fmt(ctx["por_que"])}</p>
      <div class="timeline">
        {filas_mapa}
      </div>
      <div class="interactive-box">
        <strong>Hoy:</strong> {fmt(ctx["resultado"])}
        <br><strong>Siguiente:</strong> {despues}
      </div>
    </section>

    <!-- Lámina 4: fundamentos teóricos -->
    <section class="slide">
      <h2>00–20 min: Fundamentos teóricos — {fmt(tp["titulo"])}</h2>
      <p class="lead">{fmt(tp["lead"])}</p>
      <div class="grid-3">
        {cards_s2}
      </div>
    </section>

    <!-- Lámina 5: arquitectura de memoria -->
    <section class="slide">
      <h2>Arquitectura de memoria y estructuras de datos: {fmt(am["titulo"])}</h2>
      <p class="lead">{fmt(am["descripcion"])}</p>
      <div class="grid-2">
        {cards_s3}
      </div>
    </section>

    <!-- Lámina 6: casos borde -->
    <section class="slide">
      <h2>{fmt(cb["titulo"])}</h2>
      <p class="lead">Puntos críticos de falla que debes proteger en tu código y auditar en las respuestas del LLM:</p>
      <div class="card static" style="padding: 22px;">
        <ul style="padding-left: 20px; line-height: 1.9;">
          {items_s4}
        </ul>
      </div>
    </section>

    <!-- Lámina 7: el código del laboratorio y del proyecto -->
    <section class="slide">
      <h2>Así se ve el código</h2>
      <p class="lead">A la izquierda, el contrato y el esqueleto que vas a completar. A la derecha, dónde aparece ese concepto en Lenguajito.</p>
      <div class="grid-2">
        <div class="code-box">
          <div class="code-head">en el laboratorio · sesión {n}</div>
          <pre class="code">{lab_code}</pre>
        </div>
        <div class="code-box">
          <div class="code-head">en el proyecto · Lenguajito</div>
          <pre class="code">{proy_code}</pre>
          <p class="code-note">{proy_nota}</p>
        </div>
      </div>
      <div class="interactive-box">
        <strong>No es la solución:</strong> la firma es el contrato y el cuerpo está marcado con <code>TODO</code>. El código de la derecha es del proyecto, no del laboratorio: si copias cualquiera de los dos tal cual, el <em>harness</em> sigue en rojo.
      </div>
    </section>

    <!-- Lámina 8: lo que vas a construir hoy -->
    <section class="slide">
      <h2>Lo que vas a construir hoy</h2>
      <p class="lead">{fmt(lab["explicacion"])}</p>
      <div class="grid-lab">
        <div class="card static">
          <div class="card-header"><span style="color: var(--accent);">Entregable</span></div>
          <p style="margin-bottom: 10px;">{fmt(lab["entregable"])}</p>
          <div class="card-header" style="margin-top: 6px;"><span style="color: var(--accent-silver);">Reglas del reto</span></div>
          <ul>{reglas}</ul>
          <div class="card-header" style="margin-top: 8px;"><span style="color: var(--accent-green);">Se acepta cuando</span></div>
          <ul>{criterios}</ul>
        </div>
        <div class="terminal">
          <div class="terminal-bar">salida esperada del harness — sesión {n}</div>
          <pre>{salida}</pre>
        </div>
      </div>
      <div class="interactive-box">
        <strong>Mientras no implementes, esta salida será roja:</strong> el harness imprime <code>FALLA</code> en cada caso pendiente y termina con código de salida 1. No hay «verde» gratis.
      </div>
    </section>

    <!-- Lámina 9: dinámica y entrega -->
    <section class="slide">
      <h2>Dinámica de la clase y protocolo de entrega</h2>
      <div class="grid-2">
        <div class="card static">
          <div class="card-header"><span>Estructura de la clase (60 min)</span></div>
          <ul>
            <li><strong>00–20 min:</strong> Conceptos, teoría e invariantes.</li>
            <li><strong>20–35 min:</strong> Fase 1: El Artesano (código manual, sin IA).</li>
            <li><strong>35–45 min:</strong> Fase 2: El Científico (prompt C-R-E-O).</li>
            <li><strong>45–55 min:</strong> Fase 3: El Auditor (harness en verde).</li>
            <li><strong>55–60 min:</strong> Sincronización y commit en GitHub.</li>
          </ul>
        </div>
        <div class="card static">
          <div class="card-header"><span>Evidencia evaluada</span></div>
          <p>Tu archivo <code>auditorias/auditoria_sesion{n}.md</code> debe contener la matriz C-R-E-O y el reporte crítico.</p>
          <p style="margin-top: 8px;">Adjunta también la salida del harness: es la prueba de que tu implementación pasó los {len(lab["casos"])} casos.</p>
          <div class="card-details" style="max-height: none; opacity: 1; border-top: none; margin-top: 10px; padding-top: 0;">
            Cada entrega diaria acumula para el 40% de laboratorios y alimenta tu Proyecto Integrador.
          </div>
        </div>
      </div>
      <div class="interactive-box">
        <strong>Docente Titular:</strong> MCC. Iván Márquez Larios (<code>ijmarquezl</code>) &bull; TecNM Cancún
      </div>
    </section>
  </main>

  <footer>
    <div class="progress" id="slideIndicator">SLIDE 1 / 9</div>
    <div class="controls">
      <button id="prevBtn" onclick="prevSlide()" disabled>&larr; Anterior</button>
      <button id="nextBtn" onclick="nextSlide()">Siguiente &rarr;</button>
    </div>
  </footer>

  <script>
    let currentSlide = 0;
    const slides = document.querySelectorAll('.slide');
    const indicator = document.getElementById('slideIndicator');
    const prevBtn = document.getElementById('prevBtn');
    const nextBtn = document.getElementById('nextBtn');
    function updateSlides() {{
      slides.forEach((slide, index) => slide.classList.toggle('active', index === currentSlide));
      indicator.textContent = `SLIDE ${{currentSlide + 1}} / ${{slides.length}}`;
      prevBtn.disabled = currentSlide === 0;
      nextBtn.disabled = currentSlide === slides.length - 1;
    }}
    function nextSlide() {{ if (currentSlide < slides.length - 1) {{ currentSlide++; updateSlides(); }} }}
    function prevSlide() {{ if (currentSlide > 0) {{ currentSlide--; updateSlides(); }} }}
    function toggleCard(card) {{
      if (card.classList.contains('static')) return;
      card.classList.toggle('expanded');
      const hint = card.querySelector('.expand-hint');
      if (hint) hint.textContent = card.classList.contains('expanded') ? '[-]' : '[+]';
    }}
    document.addEventListener('keydown', (e) => {{
      if (e.key === 'ArrowRight' || e.key === ' ') nextSlide();
      if (e.key === 'ArrowLeft') prevSlide();
    }});
  </script>
</body>
</html>
"""
