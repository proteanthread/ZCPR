/*
    This file is part of SUPPL - the supplemental library for DOS
    Copyright (C) 1996-2000 Steffen Kaiser

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* Portable include file for MSVC (flat/64-bit compiler environments) */

#ifdef _MSC_VER

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

#ifndef EZERO
#define EZERO 0
#endif

#define far
#define farcode
#define fardata
#define near
#define huge
#define interrupt
#define pascal
#define cdecl

/* byte/word/dword type mapping for 64-bit clean C17 */
typedef uint8_t byte;
typedef uint16_t word;
typedef unsigned int dword;
typedef int FLAG;
typedef int FLAG8;

#ifndef _CLIB_
#define peekb(segm,ofs) (*(volatile byte*)(uintptr_t)((((uintptr_t)(segm)) << 4) + (ofs)))
#define peekw(segm,ofs) (*(volatile word*)(uintptr_t)((((uintptr_t)(segm)) << 4) + (ofs)))
#define pokeb(segm,ofs,value) (*(volatile byte*)(uintptr_t)((((uintptr_t)(segm)) << 4) + (ofs)) = (byte)(value))
#define pokew(segm,ofs,value) (*(volatile word*)(uintptr_t)((((uintptr_t)(segm)) << 4) + (ofs)) = (word)(value))
#endif

#define _osversion 0x061E /* DOS 6.30 simulation */

typedef void (*isr)(void);
#define set_isrfct(num,fct) ((void)0)
#define get_isr(num,var) ((var) = NULL)
#define set_isr(num,var) ((void)0)
#define ASMINTERRUPT

/* calling an interrupt REGPACK definition */
typedef struct {
    unsigned int r_ax;
    unsigned int r_bx;
    unsigned int r_cx;
    unsigned int r_dx;
    unsigned int r_bp;
    unsigned int r_si;
    unsigned int r_di;
    unsigned int r_ds;
    unsigned int r_es;
    unsigned int r_flags;
} IREGS;

/* forward declaration of the C replacement for intr.asm */
void intr(int intno, void *regs);
#define intrpt(num,regs) intr((num), (regs))

#define _CS 0

/* Disk and file function stubs/simulations */
static inline int getdisk(void) { return 2; /* Drive C: */ }
static inline int setdisk(int drive) { (void)drive; return 0; }
static inline int _dos_getftime(int fd, unsigned *date, unsigned *time) { (void)fd; *date = 0; *time = 0; return 0; }
static inline int _dos_setftime(int fd, unsigned date, unsigned time) { (void)fd; (void)date; (void)time; return 0; }
static inline void far *_dos_getvect(int intno) { (void)intno; return NULL; }
static inline void _dos_setvect(int intno, void far *vect) { (void)intno; (void)vect; }
static inline void far *getdta(void) { return NULL; }
static inline void setdta(void far *dta) { (void)dta; }

#endif /* _MSC_VER */
