/* $Id$
 * File:
 *                          cswap.asm -> cswap.c
 * Description:
 *                       command.com swapping
 *
 *                    Copyright (c) 2001 tom ehlert
 *                       All Rights Reserved
 *
 * =========================================================================
 * 64-bit Modernization Guidelines:
 *
 * WHAT CAN BE CHANGED:
 * - Emulated memory sizes, structures or status queries.
 *
 * WHAT CANNOT BE CHANGED:
 * - Mapped global variables (`SwapResidentSize`, `SwapTransientSize`, `canexit`)
 *   which are used throughout FreeCOM.
 *
 * WHAT TO EXPECT:
 * - On 64-bit platforms, XMS functions return failure or inactive states
 *   since physical XMS memory is not present in flat virtual address space.
 *
 * WHAT TO DO IF SOMETHING DOESN'T WORK OR BREAKS:
 * - Verify that the client calls handle `XMSisactive() == 0` correctly.
 * =========================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <portable.h>

word SwapResidentSize = 0;
word SwapTransientSize = 0;
word XMSsave[8] = {0};
word XMSrestore[8] = {0};
dword XMSdriverAdress = 0;
word mySS = 0;
word mySP = 0;

/* Stub buffers for DOS structures for compatibility */
byte dosFCB1[37] = {0};
byte dosFCB2[37] = {0};
char dosCMDNAME[384] = {0};
byte dosParamDosExec[22] = {0};

char localStack[1024] = {0};

int XMSisactive(void) {
    return 0; /* XMS swap is not active or supported on 64-bit platforms */
}

int XMSexec(void) {
    /* Swapping is obsolete under virtual memory. Returning failure. */
    return -1;
}

void xms_kill(void) {
    /* No-op under flat virtual memory space. */
}

void XMSrequest(void) {
    /* No-op under flat memory. */
}
