/* Display the command line
 * THE LAST CHARACTER MUST BE A DOLLAR SIGN!!!
 * e.g.:
 * C> disp_cmd=abcd$
 *
 * =========================================================================
 * 64-bit Modernization Guidelines:
 *
 * WHAT CAN BE CHANGED:
 * - Formatting of the printed command line (e.g. adding newlines or quotes).
 * - Output stream destination (e.g. stdout vs stderr).
 *
 * WHAT CANNOT BE CHANGED:
 * - The program must parse the arguments matching the standard C argc/argv structure
 *   and handle legacy DOS '$'-terminated strings if they are present.
 *
 * WHAT TO EXPECT:
 * - On modern 64-bit Windows 11 and Linux, this utility outputs the command line
 *   to stdout and exits with a success code.
 *
 * WHAT TO DO IF SOMETHING DOESN'T WORK OR BREAKS:
 * - Verify stdout buffering or check if the command line argument is passed correctly
 *   from the spawning shell.
 * =========================================================================
 */

#include <stdio.h>
#include <string.h>
#include <portable.h>

int main(int argc, char **argv) {
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            char *arg = argv[i];
            /* Handle DOS $-terminated string compatibility if present */
            size_t len = strlen(arg);
            if (len > 0 && arg[len - 1] == '$') {
                arg[len - 1] = '\0';
            }
            printf("%s%s", arg, (i < argc - 1) ? " " : "");
        }
        printf("\n");
    }
    return 0;
}
