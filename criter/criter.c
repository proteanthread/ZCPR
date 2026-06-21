/* $Id$
 *
 *		Critical Error handler -- module
 *
 *	This handler does:
 *	0) The current context is passed into this function within ES:BX
 *		The original ES & BX are restored by CRITER.
 *	1) Probe for Autofail <-> return FAIL for all criterrs
 *	2) Display error message:
 *	block devices:
 *		Error "reading from"|"writing to" drive ?: XXX area: error
 *	character devices:
 *		Error "reading from"|"writing to" device XXXXXXX: error
 *	3) Display user action possebilities:
 *		(A)bort (I)gnore (R)etry (F)ail? _
 *	4) Get user input & return to DOS (via IRET)
 *
 *	== What to do if neither I,R,nor F are allowed??
 *	== Simply return??
 *
 *	Organization of the strings:
 *	+ There may be upto 256 strings, numbered from 0 through N
 *	+ The actual "error strings" must be the last ones and their
 *	  order must be the same as the code in DI
 *	+ All strings are packed together as follows:
 *	  (N+1) words	near pointer to character sequence of string #X
 *	  character sequences of strings, no obvious order among them required
 *	  This block is created by an external tool, like the messages for
 *	  FreeCom itself and simply attached to the criterr handler code.
 *
 * =========================================================================
 * 64-bit Modernization Guidelines:
 *
 * WHAT CAN BE CHANGED:
 * - Error prompt format, colors, console formatting, and string mappings.
 * - Integration with standard streams (e.g. stderr/stdout).
 * - Mapped responses to signal behaviors.
 *
 * WHAT CANNOT BE CHANGED:
 * - The logic of autofail check and repeat-check arrays (to support ZCPR's
 *   internal API calls like path parsing and directory traversals).
 * - Function interface signatures and global variables (`criter_repeat_checkarea`).
 *
 * WHAT TO EXPECT:
 * - When a critical error is flagged, the shell prompts the user directly on standard error
 *   and captures standard input keys, unless autofail or repeat check is active.
 *
 * WHAT TO DO IF SOMETHING DOESN'T WORK OR BREAKS:
 * - Ensure stdin/stderr streams are not redirected or closed.
 * - Verify the status of the `repCheck` configuration.
 * =========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <portable.h>

#define HIDE_CRITER_DRIVES 26
#define FAIL 3
#define RETRY 1
#define ABORT 2
#define IGNORE 0

int repCheck = -1; /* disabled by default */
signed char criter_repeat_checkarea[HIDE_CRITER_DRIVES] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

/* Autofail variant of Critical Error Handler */
int autofail_err_handler(void) {
    return FAIL;
}

/* Helper to map error numbers to error strings */
const char* get_criter_error_string(int err_code) {
    switch (err_code) {
        case 0: return "write-protection violation attempted";
        case 1: return "unknown unit for driver";
        case 2: return "drive not ready";
        case 3: return "unknown command given to driver";
        case 4: return "data error (bad CRC)";
        case 5: return "bad device driver request structure length";
        case 6: return "seek error";
        case 7: return "unknown media type";
        case 8: return "sector not found";
        case 9: return "printer out of paper";
        case 10: return "write fault";
        case 11: return "read fault";
        case 12: return "general failure";
        case 13: return "(DOS 3.0+) sharing violation";
        case 14: return "(DOS 3.0+) lock violation";
        case 15: return "invalid disk change";
        case 16: return "(DOS 3.0+) FCB unavailable";
        case 17: return "(DOS 3.0+) sharing buffer overflow";
        case 18: return "(DOS 4.0+) code page mismatch";
        case 19: return "(DOS 4.0+) out of input";
        case 20: return "(DOS 4.0+) insufficient disk space";
        default: return "Unknown error code";
    }
}

/* Low level Critical Error Handler */
int lowlevel_err_handler(int drive_or_char, int err_code, int is_write, int area_code, int allowed_actions) {
    /* Mapped actions bitmasks: Ignore = 32, Retry = 16, Fail = 8 */
    int allow_ignore = allowed_actions & 32;
    int allow_retry = allowed_actions & 16;
    int allow_fail = allowed_actions & 8;
    int allow_abort = 1; /* Abort is always allowed */

    /* Check repeat check autofailer cache */
    if (repCheck != -1 && drive_or_char >= 0 && drive_or_char < HIDE_CRITER_DRIVES) {
        if (criter_repeat_checkarea[drive_or_char] != -1) {
            criter_repeat_checkarea[drive_or_char]++;
            return FAIL; /* Silently return FAIL */
        }
    }

    /* Print Error Message */
    const char *action_str = is_write ? "writing to" : "reading from";
    const char *err_msg = get_criter_error_string(err_code);

    if (drive_or_char >= 0 && drive_or_char < 26) {
        /* Block Device */
        const char *areas[] = { "DOS", "FAT", "root", "data" };
        const char *area_str = (area_code >= 0 && area_code < 4) ? areas[area_code] : "unknown";
        fprintf(stderr, "\nError %s drive %c: %s area: %s\n", action_str, 'A' + drive_or_char, area_str, err_msg);
    } else {
        /* Character Device */
        fprintf(stderr, "\nError %s device: %s\n", action_str, err_msg);
    }

    /* Prompt User */
    while (1) {
        fprintf(stderr, "(");
        if (allow_abort) fprintf(stderr, "A");
        if (allow_ignore) fprintf(stderr, "I");
        if (allow_retry) fprintf(stderr, "R");
        if (allow_fail) fprintf(stderr, "F");
        fprintf(stderr, ")bort, Ignore, Retry, Fail? ");

        int ch = getchar();
        if (ch == EOF) {
            return FAIL;
        }
        /* Convert to upper case */
        if (ch >= 'a' && ch <= 'z') ch -= 32;

        if (ch == 'A' && allow_abort) {
            if (drive_or_char >= 0 && drive_or_char < HIDE_CRITER_DRIVES) {
                criter_repeat_checkarea[drive_or_char] = 0; /* Enable Autofail next times */
            }
            return ABORT;
        }
        if (ch == 'I' && allow_ignore) return IGNORE;
        if (ch == 'R' && allow_retry) return RETRY;
        if (ch == 'F' && allow_fail) {
            if (drive_or_char >= 0 && drive_or_char < HIDE_CRITER_DRIVES) {
                criter_repeat_checkarea[drive_or_char] = 0; /* Enable Autofail next times */
            }
            return FAIL;
        }
        fprintf(stderr, "\nInvalid key.\n");
    }
}
