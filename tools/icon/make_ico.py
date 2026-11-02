"""Writes assets/loupe.ico from assets/loupe.png: every size Windows asks
for, for loupe.exe's icon, its shortcuts and its .apak files. Run it after
changing the PNG and commit both.

usage: python tools/icon/make_ico.py
"""

from pathlib import Path

from PIL import Image

ASSETS = Path(__file__).resolve().parents[2] / "assets"
SIZES = [(16, 16), (24, 24), (32, 32), (48, 48), (64, 64), (128, 128), (256, 256)]

Image.open(ASSETS / "loupe.png").save(ASSETS / "loupe.ico", sizes=SIZES)
