/* $Id$
 *  LOWEXEC.ASM -> LOWEXEC.C
 *
 *  Comments:
 *
 *  ??/??/95 (Steffen Kaiser)
 *    started.
 *
 *  12/??/95 (Svante Frey)
 *    reorganized the code
 *
 *  01/06/96 (Tim Norman)
 *    added this history
 *
 *  08/07/96 (Steffen Kaiser)
 *    made argument handling independent of memory model
 *
 * =========================================================================
 * 64-bit Modernization Guidelines:
 *
 * WHAT CAN BE CHANGED:
 * - Spawning mechanisms, argument parsing rules, and environment propagation.
 * - Integration logic for the CP/M and MP/M JIT emulation engine.
 *
 * WHAT CANNOT BE CHANGED:
 * - Function signature `int lowLevelExec(char far * cmd, struct ExecBlock far * bl)`.
 * - The returned exit codes, which other parts of the shell expect.
 *
 * WHAT TO EXPECT:
 * - On 64-bit Windows 11 and Linux, this executes the child process synchronously
 *   and propagates stdout/stderr/stdin descriptors.
 * - If `ZCPR_CPM_MODE` environment variable is set to "1" or "on", it triggers the JIT hook.
 *
 * WHAT TO DO IF SOMETHING DOESN'T WORK OR BREAKS:
 * - Verify path variables or check if the binary has execute permissions.
 * - Under Linux, ensure path uses correct forward slashes.
 * =========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <portable.h>

#ifdef _WIN32
#include <process.h>
#include <windows.h>
#define strcasecmp _stricmp
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

/* forward structure declarations matching exec.c */
struct fcb {
    char bytes[0x25];
};

struct ExecBlock {
    word segOfEnv;
    char far *cmdLine;
    struct fcb far *fcb1;
    struct fcb far *fcb2;
};

/* CP/M JIT emulator entry hook */
int invoke_cpm_jit_emulator(const char *cmd, struct ExecBlock *bl) {
    fprintf(stderr, "[ZCPR-JIT] CP/M / MP/M JIT Emulation Hook: Executing '%s'\n", cmd);
    /* Future CP/M / MP/M emulator implementation goes here.
       Currently it returns a stubbed exit success status. */
    (void)bl;
    return 0;
}

/* Parse legacy DOS cmdline parameter block (Pascal-like string) into argv array */
static void parse_args(char *cmdline_raw, char *args_out[], int max_args, int *argc_out) {
    int count = 0;
    char *ptr = cmdline_raw;

    /* Skip leading whitespace */
    while (*ptr == ' ' || *ptr == '\t' || *ptr == '\r' || *ptr == '\n') {
        ptr++;
    }

    while (*ptr != '\0' && count < max_args - 1) {
        args_out[count++] = ptr;
        
        /* Find next token space or end of string, taking care of quotes */
        int in_quote = 0;
        while (*ptr != '\0') {
            if (*ptr == '"') {
                in_quote = !in_quote;
            } else if ((*ptr == ' ' || *ptr == '\t' || *ptr == '\r' || *ptr == '\n') && !in_quote) {
                *ptr = '\0';
                ptr++;
                break;
            }
            ptr++;
        }
        
        /* Skip trailing spaces */
        while (*ptr == ' ' || *ptr == '\t' || *ptr == '\r' || *ptr == '\n') {
            ptr++;
        }
    }
    args_out[count] = NULL;
    *argc_out = count;
}

int lowLevelExec(char far * cmd, struct ExecBlock far * bl) {
    /* 1. CP/M and MP/M mode hook check */
    const char *cpm_mode = getenv("ZCPR_CPM_MODE");
    if (cpm_mode && (strcmp(cpm_mode, "1") == 0 || strcasecmp(cpm_mode, "on") == 0)) {
        return invoke_cpm_jit_emulator(cmd, bl);
    }

    /* 2. Process command line argument buffer */
    /* bl->cmdLine points to string buffer, where cmdLine[0] is length, cmdLine[1...] is raw cmdline ending with '\r' */
    int raw_len = (unsigned char)bl->cmdLine[0];
    char *cmdline_copy = malloc(raw_len + 1);
    if (!cmdline_copy) {
        return -1;
    }
    memcpy(cmdline_copy, bl->cmdLine + 1, raw_len);
    cmdline_copy[raw_len] = '\0';

    /* Strip trailing carriage return if any */
    for (int i = raw_len - 1; i >= 0; i--) {
        if (cmdline_copy[i] == '\r' || cmdline_copy[i] == '\n') {
            cmdline_copy[i] = '\0';
        } else {
            break;
        }
    }

    /* Max 64 arguments */
    char *args[65];
    int child_argc = 0;
    args[0] = (char *)cmd; /* first argument is command */
    
    parse_args(cmdline_copy, args + 1, 64, &child_argc);

    int exit_status = -1;

#ifdef _WIN32
    /* Spawn process synchronously on Windows */
    intptr_t result = _spawnvp(_P_WAIT, cmd, (const char *const *)args);
    if (result != -1) {
        exit_status = (int)result;
    } else {
        /* Failed to spawn, try with command interpreter fallback */
        char win_cmd[MAX_PATH];
        snprintf(win_cmd, sizeof(win_cmd), "cmd.exe /c %s %s", cmd, cmdline_copy);
        exit_status = system(win_cmd);
    }
#else
    /* Fork and execute on Linux */
    pid_t pid = fork();
    if (pid == 0) {
        /* Child */
        execvp(cmd, args);
        /* If execvp fails, exit with error code */
        exit(127);
    } else if (pid > 0) {
        /* Parent */
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            exit_status = WEXITSTATUS(status);
        }
    }
#endif

    free(cmdline_copy);
    return exit_status;
}
