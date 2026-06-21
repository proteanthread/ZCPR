/*   $Id$
 *
 *	XMS swap Ctrl-Break handler stub.
 *
 * =========================================================================
 * 64-bit Modernization Guidelines:
 *
 * WHAT CAN BE CHANGED:
 * - Return codes or actions inside these stubs.
 *
 * WHAT CANNOT BE CHANGED:
 * - Signature mapping for compatibility with context tables.
 *
 * WHAT TO EXPECT:
 * - Direct return of success or ignore since swapping is virtualized.
 *
 * WHAT TO DO IF SOMETHING DOESN'T WORK OR BREAKS:
 * - Verify the status of the general break handler.
 * =========================================================================
 */

#include <portable.h>

int xms_brk_handler(void) {
    /* No-op under flat memory environments. */
    return 0;
}
