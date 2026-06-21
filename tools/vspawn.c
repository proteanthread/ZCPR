/* Kernel Swap Support Faker #2 -> C17
 * Useage: VSPAWN [{ arguments }]
 *
 * =========================================================================
 * 64-bit Modernization Guidelines:
 *
 * WHAT CAN BE CHANGED:
 * - Extension formatting rules (.SWP vs other targets).
 * - Signal handling in JIT mode.
 *
 * WHAT CANNOT BE CHANGED:
 * - The logic of searching for own file name to execute the companion .SWP.
 *
 * WHAT TO EXPECT:
 * - Looks up its own binary path, substitutes the extension with ".SWP",
 *   and spawns it synchronously.
 *
 * WHAT TO DO IF SOMETHING DOESN'T WORK OR BREAKS:
 * - Ensure a matching .SWP executable exists in the same directory.
 * =========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <portable.h>

#ifdef _WIN32
#include <process.h>
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

int main(int argc, char **argv) {
    char shell_path[512] = {0};

    /* Get own path from argv[0] */
    if (argc > 0 && argv[0]) {
        strncpy(shell_path, argv[0], sizeof(shell_path) - 5);
        char *ext = strrchr(shell_path, '.');
        if (ext) {
            strcpy(ext, ".SWP");
        } else {
            strcat(shell_path, ".SWP");
        }
    } else {
        strcpy(shell_path, "COMMAND.SWP");
    }

    /* JIT mode check */
    const char *cpm_mode = getenv("ZCPR_CPM_MODE");
    if (cpm_mode && (strcmp(cpm_mode, "1") == 0 || strcasecmp(cpm_mode, "on") == 0)) {
        printf("[VSPAWN] JIT emulation mode detected. Execution environment active.\n");
    }

    /* Build args: the first arg is the shell_path, followed by standard arguments */
    char **child_args = malloc(sizeof(char*) * (argc + 1));
    if (!child_args) {
        return -1;
    }

    child_args[0] = shell_path;
    for (int i = 1; i < argc; i++) {
        child_args[i] = argv[i];
    }
    child_args[argc] = NULL;

    int exit_status = -1;

#ifdef _WIN32
    intptr_t result = _spawnvp(_P_WAIT, shell_path, (const char *const *)child_args);
    if (result != -1) {
        exit_status = (int)result;
    } else {
        /* On Windows, try executing COMMAND.SWP if present, otherwise log error */
        fprintf(stderr, "VSPAWN: Failed to execute shell '%s'\n", shell_path);
    }
#else
    pid_t pid = fork();
    if (pid == 0) {
        execvp(shell_path, child_args);
        exit(127);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            exit_status = WEXITSTATUS(status);
        }
    }
#endif

    free(child_args);
    return exit_status;
}
