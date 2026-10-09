import gzip
import re
import os

html_path = "preview.html"
css_path = "styles.css"
js_path = "app.js"
output_header_path = "include/web_page.h"

with open(html_path, "r", encoding="utf-8") as f:
    html = f.read()

with open(css_path, "r", encoding="utf-8") as f:
    css = f.read()

with open(js_path, "r", encoding="utf-8") as f:
    js = f.read()

# Inline CSS
html = html.replace('<link rel="stylesheet" href="styles.css">', f'<style>\n{css}\n</style>')

# Inline JS
html = html.replace('<script src="app.js"></script>', f'<script>\n{js}\n</script>')

# Gzip compress
gz_data = gzip.compress(html.encode("utf-8"), compresslevel=9)

print(f"Original bundle size: {len(html)} bytes")
print(f"Compressed GZIP size: {len(gz_data)} bytes")

# Format as C byte array
lines = []
for i in range(0, len(gz_data), 16):
    chunk = gz_data[i:i+16]
    hex_str = ", ".join(f"0x{b:02x}" for b in chunk)
    lines.append(f"  {hex_str},")

header_content = f"""#pragma once
#include <Arduino.h>

// Pre-compressed GZIP bundle (HTML + CSS + JS) for instant flash streaming via send_P
// Generated size: {len(gz_data)} bytes (reduced from {len(html)} bytes)
const uint8_t INDEX_HTML_GZ[] PROGMEM = {{
""" + "\n".join(lines) + f"""
}};
const size_t INDEX_HTML_GZ_LEN = {len(gz_data)};
"""

with open(output_header_path, "w", encoding="utf-8") as f:
    f.write(header_content)

print(f"Successfully generated {output_header_path}")
