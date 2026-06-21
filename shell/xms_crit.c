/*   $Id$
 *
 *	XMS swap Critical Error handler stub.
 *
 * =========================================================================
 * 64-bit Modernization Guidelines:
 *
 * WHAT CAN BE CHANGED:
 * - Redirection to standard critical error handlers.
 *
 * WHAT CANNOT BE CHANGED:
 * - Interface signature.
 *
 * WHAT TO EXPECT:
 * - Simple forwarding of parameters or returning default fail codes.
 *
 * WHAT TO DO IF SOMETHING DOESN'T WORK OR BREAKS:
 * - Double check the return values mapping to standard critical error codes.
 * =========================================================================
 */

#include <portable.h>

int xms_crit_handler(int err_code, const char *device) {
    (void)err_code;
    (void)device;
    /* XMS Swap is disabled; return Fail directly. */
    return 3;
}
