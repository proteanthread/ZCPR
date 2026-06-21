/* File:
 *                         intr.asm
 * Description:
 *       Assembly implementation of calling an interrupt
 *
 * =========================================================================
 * 64-bit Modernization Guidelines:
 *
 * WHAT CAN BE CHANGED:
 * - Register mapping or simulated behaviors for standard DOS interrupts (like int 21h).
 * - Carry flag / status return mapping.
 *
 * WHAT CANNOT BE CHANGED:
 * - The function name and signature (`intr(int intno, void *regs)`).
 * - Mapping structure pointer to IREGS to maintain layout contracts.
 *
 * WHAT TO EXPECT:
 * - Simulated register values for standard queries, and no-op / success states for others.
 *
 * WHAT TO DO IF SOMETHING DOESN'T WORK OR BREAKS:
 * - Check if the calling code expects a specific register return value or carry status
 *   and add it to the switch case.
 * =========================================================================
 */

#include <portable.h>
#include <stdio.h>

void intr(int intno, void *regs) {
    IREGS *r = (IREGS *)regs;
    
    /* Clear carry flag by default to indicate success (bit 0 of flags) */
    r->r_flags &= ~1;

    if (intno == 0x21) {
        unsigned char ah = (r->r_ax >> 8) & 0xFF;
        switch (ah) {
            case 0x19: /* Get Current Drive (0 = A, 1 = B, 2 = C...) */
                r->r_ax = (r->r_ax & 0xFF00) | 2; /* Default to C: */
                break;
            case 0x0E: /* Select Drive */
                r->r_ax = (r->r_ax & 0xFF00) | 26; /* Mapped to 26 logical drives */
                break;
            case 0x30: /* Get DOS Version */
                r->r_ax = 0x061E; /* DOS 6.30 (AH = major = 30, AL = minor = 6) */
                r->r_bx = 0xFF00; /* OEM number */
                r->r_cx = 0x0000; /* serial number */
                break;
            default:
                /* Stub other int 21h requests */
                break;
        }
    } else if (intno == 0x10) {
        /* BIOS video interrupt stubs */
    } else if (intno == 0x2F) {
        /* Multiplexer stubs */
    }
}
