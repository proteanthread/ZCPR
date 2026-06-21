/*   $Id$
 *
 *	Dummy ^Break signal catcher -- module
 *	Aborts any program		<-> Activated only if FreeCOM is not active
 *	Full relocable
 *
 *	On return the caller must return to DOS the following way:
 *	if(Carry)	retf			<-- Terminate
 *	if(!Carry)	retf 2			<-- Proceed / Ignore ^Break
 *
 *   $Log$
 *   Revision 1.2  2002/04/02 18:09:31  skaus
 *   add: XMS-Only Swap feature (FEATURE_XMS_SWAP) (Tom Ehlert)
 *
 *   Revision 1.1  2001/04/23 21:35:41  skaus
 *   Beta 7 changes (code split)
 *
 *   Revision 1.1.2.1  2001/01/28 21:23:55  skaus
 *   add: Kernel Swap Support
 *
 * =========================================================================
 * 64-bit Modernization Guidelines:
 *
 * WHAT CAN BE CHANGED:
 * - The signal handler registration or default return action (e.g. exit code)
 *   can be configured for testing different termination modes.
 * - Platform-specific console handler controls (like SetConsoleCtrlHandler or sigaction)
 *   can be modified to integrate with host environments.
 *
 * WHAT CANNOT BE CHANGED:
 * - The logical intent of this handler (which is to force program termination
 *   upon encountering Ctrl-Break when ZCPR/FreeCOM itself is inactive) must remain.
 *
 * WHAT TO EXPECT:
 * - On 64-bit Windows 11 and Linux, this dummy handler returns a termination flag
 *   or aborts execution by triggering a default termination sequence.
 *
 * WHAT TO DO IF SOMETHING DOESN'T WORK OR BREAKS:
 * - Ensure console signals are not blocked by the parent process.
 * - Verify environment variable settings and console permissions on host shell.
 * =========================================================================
 */

#include <stdlib.h>
#include <signal.h>
#include <portable.h>

/*
 * Returns 1 (equivalent to Carry set in 16-bit) to tell the caller to terminate the program.
 */
int dummy_cbreak_handler(void) {
    return 1; /* Terminate program */
}
