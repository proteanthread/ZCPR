/*   $Id$
 *
 *	^Break signal catcher and initialization function.
 *	Bases on CB_CATCH.ASM
 *	It is activated only, if FreeCOM is active.
 *	--> FreeCOM polls the ^Break state actively, so the ^C press is
 *		recorded, but ignored otherwise
 *
 *	There are several possibility to communicate with FreeCOM, because
 *	FreeCOM actively polls the ^Break status, this implementation
 *	optimizes for the polling rather this handler.
 *
 *   $Log$
 *   Revision 1.3  2004/02/01 13:52:18  skaus
 *   add/upd: CVS $id$ keywords to/of files
 *
 *   Revision 1.2  2002/04/02 18:09:31  skaus
 *   add: XMS-Only Swap feature (FEATURE_XMS_SWAP) (Tom Ehlert)
 *
 *   Revision 1.1  2001/04/12 00:09:06  skaus
 *   chg: New structure
 *
 * =========================================================================
 * 64-bit Modernization Guidelines:
 *
 * WHAT CAN BE CHANGED:
 * - Signal handling hooks (e.g. mapping SIGINT or SIGBREAK).
 * - Action taken on signal capture (logging, console output).
 *
 * WHAT CANNOT BE CHANGED:
 * - The global flag `CBreakCounter` must be updated on signal capture.
 *   FreeCOM actively polls this counter to determine break states.
 *
 * WHAT TO EXPECT:
 * - On 64-bit Windows 11 and Linux, console interrupts (Ctrl-C) are caught
 *   and increment the break counter without terminating the process immediately.
 *
 * WHAT TO DO IF SOMETHING DOESN'T WORK OR BREAKS:
 * - Ensure other parts of the application do not overwrite the signal handler.
 * - Under Windows, check that console event routing is properly configured.
 * =========================================================================
 */

#include <signal.h>
#include <portable.h>

volatile int CBreakCounter = 0;

#ifdef _WIN32
#include <windows.h>
BOOL WINAPI win_ctrl_handler(DWORD ctrl_type) {
    if (ctrl_type == CTRL_C_EVENT || ctrl_type == CTRL_BREAK_EVENT) {
        CBreakCounter++;
        return TRUE; /* Handled */
    }
    return FALSE;
}
#else
void posix_sigint_handler(int sig) {
    if (sig == SIGINT) {
        CBreakCounter++;
    }
}
#endif

void cbreak_handler(void) {
#ifdef _WIN32
    SetConsoleCtrlHandler(win_ctrl_handler, TRUE);
#else
    signal(SIGINT, posix_sigint_handler);
#endif
}
