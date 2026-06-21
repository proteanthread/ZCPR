/* EGA/VGA 28-line screen mode utility
 * Loads 8x14 font in original BIOS Int 10h execution.
 *
 * =========================================================================
 * 64-bit Modernization Guidelines:
 *
 * WHAT CAN BE CHANGED:
 * - Number of columns, window title, or sizing dimensions.
 *
 * WHAT CANNOT BE CHANGED:
 * - The height target (which must remain 28 lines to match the utility's purpose).
 *
 * WHAT TO EXPECT:
 * - On modern Windows 11 and Linux terminals that support ANSI escape codes,
 *   this will resize the terminal window to 28 lines high.
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

    /* Send ANSI escape sequence to resize terminal to 28 lines, 80 columns */
    printf("\x1b[8;28;80t");
    fflush(stdout);
    return 0;
}
