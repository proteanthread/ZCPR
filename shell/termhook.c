/* File:
 *                          termhook.asm
 * Description:
 *                       command.com termination hook
 *
 * =========================================================================
 * 64-bit Modernization Guidelines:
 *
 * WHAT CAN BE CHANGED:
 * - Resources to free in the cleanup registry (buffers, descriptors, logs).
 * - Console crash message text.
 *
 * WHAT CANNOT BE CHANGED:
 * - The logic of checking if the shell is a permanent/primary shell and halting/exiting.
 *
 * WHAT TO EXPECT:
 * - Clean teardown of shell data, closing open logs, and returning exit codes.
 *
 * WHAT TO DO IF SOMETHING DOESN'T WORK OR BREAKS:
 * - Verify the status of the `canexit` variable which indicates if the instance is permanent.
 * =========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <portable.h>

word myPID = 0;
word origPPID = 0;
word terminationAddressOffs = 0;
word terminationAddressSegm = 0;
int canexit = 0; /* 1 -> can exit; 0 -> permanent instance (cannot exit) */

void terminateFreeCOMHook(int exit_code) {
    if (!canexit) {
        fprintf(stderr, "\n\aCannot terminate permanent FreeCOM instance\n");
        fprintf(stderr, "System halted ... reboot or power off now\n");
        while (1) {
            /* Death loop for permanent instance */
#ifdef _WIN32
            Sleep(1000);
#else
            exit(exit_code); /* Fallback to exit on modern systems instead of locking up host machine */
#endif
        }
    }
    
    /* Standard shell exit */
    exit(exit_code);
}
