/* Dummy drivers to be included into FreeCOM itself
 *
 * =========================================================================
 * 64-bit Modernization Guidelines:
 *
 * WHAT CAN BE CHANGED:
 * - Return values or logs inside these stubs.
 *
 * WHAT CANNOT BE CHANGED:
 * - Function signatures and names must match the expectation of other modules.
 *
 * WHAT TO EXPECT:
 * - Simple constant returns (e.g. 3 for critical error fail) on 64-bit platforms.
 *
 * WHAT TO DO IF SOMETHING DOESN'T WORK OR BREAKS:
 * - Verify linking symbols are visible by other objects.
 * =========================================================================
 */

#include <portable.h>

int dummy_criter_handler(void) {
    return 3; /* Always fail */
}
