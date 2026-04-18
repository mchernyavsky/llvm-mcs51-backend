from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from elftools.elf.elffile import ELFFile


@dataclass(frozen=True)
class FlatImage:
    image: bytes
    done_address: int
    function_address: int


def _find_function_symbol(elf: ELFFile, function: str):
    symtab = elf.get_section_by_name(".symtab")
    if symtab is None:
        raise SystemExit(f"{function}: missing .symtab in LLVM object")

    names = {function, f"_{function}"}
    for symbol in symtab.iter_symbols():
        if symbol.name in names:
            return symbol
    raise SystemExit(f"{function}: function symbol not found in LLVM object")


def _load_text_section(elf: ELFFile, function: str) -> tuple[bytes, int]:
    text = elf.get_section_by_name(".text")
    if text is None:
        raise SystemExit(f"{function}: missing .text section in LLVM object")
    text_index = elf.get_section_index(".text")

    for reloc_name in (".rel.text", ".rela.text"):
        reloc = elf.get_section_by_name(reloc_name)
        if reloc is not None and reloc.num_relocations() != 0:
            raise SystemExit(f"{function}: relocations in {reloc_name} are not supported yet")

    symbol = _find_function_symbol(elf, function)
    if symbol["st_shndx"] != text_index:
        raise SystemExit(f"{function}: target symbol is not defined in .text")

    section_data = text.data()
    offset = int(symbol["st_value"])
    if offset < 0 or offset > len(section_data):
        raise SystemExit(f"{function}: invalid function offset {offset} in LLVM object")

    return section_data, offset


def _harness_size(arg_count: int) -> int:
    if arg_count > 2:
        raise SystemExit("only up to two arguments are supported by the test harness")
    return arg_count * 2 + 7


def _build_harness(args: list[int], function_address: int) -> tuple[bytes, int]:
    rom = bytearray()
    for reg_num, value in zip((7, 6), args):
        if not 0 <= value <= 0xFF:
            raise SystemExit(f"argument {value} is out of range for i8")
        rom.extend((0x78 + reg_num, value))

    rom.extend((0x12, (function_address >> 8) & 0xFF, function_address & 0xFF))
    rom.extend((0xF5, 0x90))

    done_address = len(rom)
    rom.extend((0x80, 0xFE))
    return bytes(rom), done_address


def build_flat_image(object_file: Path, function: str, args: list[int]) -> FlatImage:
    with object_file.open("rb") as fh:
        elf = ELFFile(fh)
        text_data, text_offset = _load_text_section(elf, function)

    function_address = _harness_size(len(args)) + text_offset
    harness, done_address = _build_harness(args, function_address)
    return FlatImage(image=harness + text_data, done_address=done_address, function_address=function_address)


def write_intel_hex(path: Path, image: bytes, *, record_size: int = 16) -> None:
    lines: list[str] = []
    for offset in range(0, len(image), record_size):
        chunk = image[offset : offset + record_size]
        total = len(chunk) + ((offset >> 8) & 0xFF) + (offset & 0xFF)
        fields = [f":{len(chunk):02X}{offset:04X}00"]
        for byte in chunk:
            total += byte
            fields.append(f"{byte:02X}")
        checksum = (-total) & 0xFF
        fields.append(f"{checksum:02X}")
        lines.append("".join(fields))
    lines.append(":00000001FF")
    path.write_text("\n".join(lines) + "\n")
