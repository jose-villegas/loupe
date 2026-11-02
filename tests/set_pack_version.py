"""Copies an asset pack with another format version in its header, for a test
that loupe refuses it: python set_pack_version.py PACK VERSION OUT."""

import struct
import sys
from pathlib import Path

# The header's version: a little-endian u32 after the four-byte magic.
VERSION_AT = 4


def main() -> int:
    pack, version, out = Path(sys.argv[1]), int(sys.argv[2]), Path(sys.argv[3])
    data = bytearray(pack.read_bytes())
    struct.pack_into("<I", data, VERSION_AT, version)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_bytes(data)
    return 0


if __name__ == "__main__":
    sys.exit(main())
