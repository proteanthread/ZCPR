/* EGA/VGA 50-line screen mode utility
 * Loads 8x8 font in original BIOS Int 10h execution.
 *
 * =========================================================================
 * 64-bit Modernization Guidelines:
 *
 * WHAT CAN BE CHANGED:
 * - Number of columns, window title, or sizing dimensions.
 *
 * WHAT CANNOT BE CHANGED:
 * - The height target (which must remain 50 lines to match the utility's purpose).
 *
 * WHAT TO EXPECT:
 * - On modern Windows 11 and Linux terminals that support ANSI escape codes,
 *   this will resize the terminal window to 50 lines high.
 *
 * WHAT TO DO IF SOMETHING DOESN'T WORK OR BREAKS:
 * - Check if the host terminal emulator supports ANSI escape/resize sequences.
 * =========================================================================
 */

#include <stdio.h>
#include <portable.h>

#ifdef _WIN32
#include <windows.h>
#endif

int main(void) {
#ifdef _WIN32
    /* Enable Virtual Terminal Processing for Windows console */
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif

    /* Send ANSI escape sequence to resize terminal to 50 lines, 80 columns */
    printf("\x1b[8;50;80t");
    fflush(stdout);
    return 0;
}
