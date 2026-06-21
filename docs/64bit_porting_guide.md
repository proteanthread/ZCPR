# ZCPR/FreeCOM 64-Bit C17 Porting and Modernization Guide

This document details the architectural changes, porting decisions, and design patterns introduced during the migration of ZCPR/FreeCOM's 16-bit real-mode x86 assembly to standard C17 (`.C` and `.H`). 

## 1. Architectural Strategy

The legacy codebase relied on 16-bit real-mode assembly instructions, segment registers, and DOS/BIOS software interrupts to handle execution, screen adjustments, context swapping, and signal hooking. On modern 64-bit operating systems (Windows 10/11 and Linux), these low-level interfaces are obsolete or inaccessible. 

The modernization ports these files into portable standard C17:
- **Flat Memory Space**: Removed segmentation register references (`CS`, `DS`, `SS`, `ES`) and memory model modifiers (`far`, `near`, `huge`, `pascal`, `cdecl`) for flat address compilation on modern platforms.
- **Portability**: Code uses standard library calls (`<stdio.h>`, `<stdlib.h>`, `<stdint.h>`, `<signal.h>`) mapped to standard Windows APIs or POSIX wrappers.
- **File Retention**: All original `.ASM`, `.inc`, and `.id` source files are preserved intact.

---

## 2. Component Migration Details

### 2.1 Critical Error Handler (`criter.c`)
- **Original**: `criter.asm` hooked Int 24h to capture hardware/device exceptions.
- **Modernization**: Written in standard C17. Prompts user choices (`(A)bort, (I)gnore, (R)etry, (F)ail?`) on stderr and stdin. Includes the caching/auto-failing algorithm for block devices (`criter_repeat_checkarea`) which enables silent failures when scanning directories or commands in PATH.

### 2.2 Program Execution (`lowexec.c`)
- **Original**: `lowexec.asm` triggered DOS `int 21h` with `AX = 4b00h` (EXEC).
- **Modernization**: Implemented via portable C library standard APIs:
  - On Windows: Uses `_spawnvp` with `_P_WAIT` (or `CreateProcess` fallback).
  - On Linux: Uses `fork` and `execvp` child task setups.
  - Command line parameter buffers are parsed from legacy DOS Pascal-like structures.

### 2.3 Signal Capture (`cb_catch.c` & `dmy_cbrk.c`)
- **Original**: `cb_catch.asm` hooked DOS interrupts to count Ctrl-Break signals.
- **Modernization**:
  - Registers OS-level console control handlers (`SetConsoleCtrlHandler` on Windows) or POSIX signal lines (`signal(SIGINT, ...)`).
  - Increments a volatile global `CBreakCounter` polled by the main shell event loop.

### 2.4 Console Window Sizing (`28.c` & `50.c`)
- **Original**: `28.asm` and `50.asm` made BIOS Int 10h calls to set 8x14 or 8x8 font height.
- **Modernization**: resizes the shell console window using standard ANSI/VT escape sequences:
  - `\x1b[8;28;80t` (for 28 lines high, 80 columns wide)
  - `\x1b[8;50;80t` (for 50 lines high, 80 columns wide)
  - Enables `ENABLE_VIRTUAL_TERMINAL_PROCESSING` on Windows console stdout descriptors before sending the escape sequences.

### 2.5 Software Interrupts (`intr.c`)
- **Original**: `intr.asm` modified code bytes dynamically to trigger arbitrary 16-bit software interrupts (`int 0` self-patch).
- **Modernization**: Implements safe C function mappings for the standard register structure `IREGS`/`REGPACK`, intercepting common system calls (e.g. `getdisk`, `setdisk`, DOS version queries) and mapping them to standard compiler functions or return values.

### 2.6 Memory Swapping (`cswap.c`)
- **Original**: `cswap.asm` swapped memory pages to disk or XMS blocks before executing child programs.
- **Modernization**: Memory swapping is handled automatically by modern OS virtual memory systems. Swap wrappers are stubbed to directly run processes without manual context saves.

### 2.7 Installable Commands (`icmd_inc.h`)
- **Original**: `icmd_inc.inc` intercepted multiplexer hooks.
- **Modernization**: Converted into standard C registration descriptors where commands register `check` and `perform` callbacks.

---

## 3. CP/M and MP/M JIT Emulation Hook

To leave room for eventual JIT execution of CP/M and MP/M written programs:
- Program executors (`lowexec.c`, `kssf.c`, `vspawn.c`) inspect the environment variable `ZCPR_CPM_MODE` on startup.
- If `ZCPR_CPM_MODE` is set to `"1"` or `"on"`, the execution routines bypass standard native process spawning and route control directly to the JIT emulator hook callback `invoke_cpm_jit_emulator`.
- To enable or disable JIT mode on modern host shells, simply adjust the environment variable:
  - Windows: `set ZCPR_CPM_MODE=1`
  - Linux: `export ZCPR_CPM_MODE=1`
