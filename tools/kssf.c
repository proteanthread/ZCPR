/* Kernel Swap Support Faker wrapper -> C17
 * Useage: kssf freecom [{ arguments }]
 *
 * =========================================================================
 * 64-bit Modernization Guidelines:
 *
 * WHAT CAN BE CHANGED:
 * - Shell path searching or command forwarding rules.
 * - Integration logic for CP/M JIT mode.
 *
 * WHAT CANNOT BE CHANGED:
 * - Command-line parameter format structure.
 *
 * WHAT TO EXPECT:
 * - Synchronous spawn of the specified shell executable with options passed along.
 *
 * WHAT TO DO IF SOMETHING DOESN'T WORK OR BREAKS:
 * - Ensure target shell path exists.
 * =========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <portable.h>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Useage: KSSF freecom [{ arguments }]\n");
        fprintf(stderr, "  freecom: absolute filename of FreeCOM/ZCPR\n");
        fprintf(stderr, "  arguments: options passed to the shell\n");
        return 3;
    }

    const char *shell_path = argv[1];
    
    /* JIT mode check */
    const char *cpm_mode = getenv("ZCPR_CPM_MODE");
    if (cpm_mode && (strcmp(cpm_mode, "1") == 0 || strcasecmp(cpm_mode, "on") == 0)) {
        printf("[KSSF] JIT emulation mode detected. Execution environment active.\n");
    }

    /* Allocate argument array for child process */
    char **child_args = malloc(sizeof(char*) * argc);
    if (!child_args) {
        return -1;
    }

    /* Skip kssf and pass freecom + remaining arguments */
    for (int i = 1; i < argc; i++) {
        child_args[i - 1] = argv[i];
    }
    child_args[argc - 1] = NULL;

    int exit_status = -1;

#ifdef _WIN32
    intptr_t result = _spawnvp(_P_WAIT, shell_path, (const char *const *)child_args);
    if (result != -1) {
        exit_status = (int)result;
    } else {
        fprintf(stderr, "KSSF: Failed to execute shell '%s'\n", shell_path);
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
