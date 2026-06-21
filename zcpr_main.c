/* ZCPR CLI (LibreDOS Modernized 64-Bit Shell)
 * Main Interactive Entry Point for Windows 11 and Linux
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <portable.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define getcwd _getcwd
#define chdir _chdir
#define strcasecmp _stricmp
#else
#include <unistd.h>
#endif

/* External linkages to our ported 64-bit C17 modules */
extern volatile int CBreakCounter;
void cbreak_handler(void);

struct fcb {
    char bytes[0x25];
};

struct ExecBlock {
    word segOfEnv;
    char far *cmdLine;
    struct fcb far *fcb1;
    struct fcb far *fcb2;
};

int lowLevelExec(char far * cmd, struct ExecBlock far * bl);
int lowlevel_err_handler(int drive_or_char, int err_code, int is_write, int area_code, int allowed_actions);

/* Internal Command Handlers */
void handle_help(void) {
    printf("\nZCPR Internal Commands:\n");
    printf("  HELP        Display this help screen\n");
    printf("  VER         Display ZCPR shell version information\n");
    printf("  CLS         Clear the screen\n");
    printf("  CD [dir]    Change current directory or show current path\n");
    printf("  DIR         List files and folders in current directory\n");
    printf("  BEEP        Ring the console bell\n");
    printf("  28          Set console height to 28 lines (via ANSI escape sequence)\n");
    printf("  50          Set console height to 50 lines (via ANSI escape sequence)\n");
    printf("  MEM         Display system memory status\n");
    printf("  EXIT        Close ZCPR shell\n\n");
    printf("ZCPR Environment Configuration:\n");
    printf("  ZCPR_CPM_MODE=1  Enables CP/M and MP/M JIT emulation hook\n\n");
}

void handle_ver(void) {
    printf("ZCPR Command Line Shell v1.0.0 [Modernized 64-Bit C17 Port]\n");
    printf("Target OS: Windows 11 (MSVC x64 build)\n");
    printf("Ctrl-Break Handler Counter: %d\n", CBreakCounter);
}

void handle_cls(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void handle_beep(void) {
    printf("\a");
    fflush(stdout);
}

void handle_cd(const char *arg) {
    char cwd_buf[512];
    if (!arg || strlen(arg) == 0) {
        if (getcwd(cwd_buf, sizeof(cwd_buf))) {
            printf("%s\n", cwd_buf);
        } else {
            perror("getcwd failed");
        }
    } else {
        if (chdir(arg) != 0) {
            fprintf(stderr, "Invalid directory: %s\n", arg);
        }
    }
}

void handle_dir(void) {
#ifdef _WIN32
    system("dir");
#else
    system("ls -la");
#endif
}

void handle_mem(void) {
    printf("Memory Allocation Strategy: Modern flat 64-bit virtual memory\n");
    printf("Conventional Memory Free: Virtual flat (Infinite)\n");
    printf("XMS memory swapping: Disabled (Swapping is handled by host OS virtual memory)\n");
}

void handle_28(void) {
    /* Send ANSI escape sequence to resize console to 28 lines high */
    printf("\x1b[8;28;80t");
    fflush(stdout);
    printf("[ZCPR] Set terminal layout height to 28 lines.\n");
}

void handle_50(void) {
    /* Send ANSI escape sequence to resize console to 50 lines high */
    printf("\x1b[8;50;80t");
    fflush(stdout);
    printf("[ZCPR] Set terminal layout height to 50 lines.\n");
}

/* Find external command executable path */
int resolve_external_command(const char *cmd, char *out_path, size_t max_len) {
#ifdef _WIN32
    /* Search PATH using SearchPath Windows API */
    DWORD res = SearchPathA(NULL, cmd, ".exe", (DWORD)max_len, out_path, NULL);
    if (res > 0 && res < max_len) {
        return 1;
    }
    res = SearchPathA(NULL, cmd, ".bat", (DWORD)max_len, out_path, NULL);
    if (res > 0 && res < max_len) {
        return 1;
    }
    res = SearchPathA(NULL, cmd, ".cmd", (DWORD)max_len, out_path, NULL);
    if (res > 0 && res < max_len) {
        return 1;
    }
#else
    /* Simple PATH search on POSIX */
    char *path_env = getenv("PATH");
    if (path_env) {
        char *path_copy = strdup(path_env);
        char *token = strtok(path_copy, ":");
        while (token) {
            snprintf(out_path, max_len, "%s/%s", token, cmd);
            if (access(out_path, X_OK) == 0) {
                free(path_copy);
                return 1;
            }
            token = strtok(NULL, ":");
        }
        free(path_copy);
    }
#endif
    return 0;
}

int main(int argc, char **argv) {
    char input_line[1024];
    char cwd_buf[512];

#ifdef _WIN32
    /* Enable Virtual Terminal Processing for Windows console resizing and ANSI formatting */
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif

    /* Initialize our newly ported signal capture module */
    cbreak_handler();

    printf("=====================================================================\n");
    printf("  ZCPR CLI (LibreDOS Modernized 64-Bit Command Shell)\n");
    printf("  Ported to standard C17 - Compiled natively for Windows 11\n");
    printf("=====================================================================\n");
    printf("Type HELP for details or EXIT to quit the shell.\n\n");

    const char *cpm_mode = getenv("ZCPR_CPM_MODE");
    if (cpm_mode && (strcmp(cpm_mode, "1") == 0 || strcasecmp(cpm_mode, "on") == 0)) {
        printf("[ZCPR] CP/M & MP/M JIT Emulation Mode: ENABLED\n\n");
    }

    while (1) {
        /* Print Prompt: ZCPR C:\path> */
        if (getcwd(cwd_buf, sizeof(cwd_buf))) {
            printf("ZCPR %s> ", cwd_buf);
        } else {
            printf("ZCPR> ");
        }
        fflush(stdout);

        if (!fgets(input_line, sizeof(input_line), stdin)) {
            break;
        }

        /* Strip trailing newlines */
        size_t len = strlen(input_line);
        while (len > 0 && (input_line[len - 1] == '\r' || input_line[len - 1] == '\n')) {
            input_line[len - 1] = '\0';
            len--;
        }

        /* Skip empty commands */
        if (len == 0) {
            continue;
        }

        /* Extract command and arguments */
        char cmd_name[256] = {0};
        char *args_ptr = NULL;

        /* Skip leading whitespace */
        char *start = input_line;
        while (*start == ' ' || *start == '\t') {
            start++;
        }

        char *end = start;
        while (*end != '\0' && *end != ' ' && *end != '\t') {
            end++;
        }

        size_t cmd_len = end - start;
        if (cmd_len >= sizeof(cmd_name)) {
            cmd_len = sizeof(cmd_name) - 1;
        }
        memcpy(cmd_name, start, cmd_len);
        cmd_name[cmd_len] = '\0';

        /* Uppercase command name for case-insensitive checks */
        char cmd_upper[256];
        for (size_t i = 0; i <= cmd_len; i++) {
            cmd_upper[i] = (char)toupper((unsigned char)cmd_name[i]);
        }

        args_ptr = end;
        while (*args_ptr == ' ' || *args_ptr == '\t') {
            args_ptr++;
        }

        /* Process Commands */
        if (strcmp(cmd_upper, "EXIT") == 0) {
            break;
        } else if (strcmp(cmd_upper, "HELP") == 0) {
            handle_help();
        } else if (strcmp(cmd_upper, "VER") == 0) {
            handle_ver();
        } else if (strcmp(cmd_upper, "CLS") == 0) {
            handle_cls();
        } else if (strcmp(cmd_upper, "BEEP") == 0) {
            handle_beep();
        } else if (strcmp(cmd_upper, "CD") == 0 || strcmp(cmd_upper, "CHDIR") == 0) {
            handle_cd(args_ptr);
        } else if (strcmp(cmd_upper, "DIR") == 0) {
            handle_dir();
        } else if (strcmp(cmd_upper, "MEM") == 0) {
            handle_mem();
        } else if (strcmp(cmd_upper, "28") == 0) {
            handle_28();
        } else if (strcmp(cmd_upper, "50") == 0) {
            handle_50();
        } else {
            /* Try to run as external command using resolve and lowLevelExec */
            char resolved_path[512] = {0};
            if (resolve_external_command(cmd_name, resolved_path, sizeof(resolved_path))) {
                /* Format command line structure for ExecBlock compatibility */
                struct ExecBlock block;
                block.segOfEnv = 0;

                /* Format the Pascal command line argument tail block: 
                   block.cmdLine[0] = length of arguments, block.cmdLine[1...] = arguments tail ending in '\r' */
                char cmdline_buf[256];
                size_t args_len = strlen(args_ptr);
                if (args_len > 250) args_len = 250;
                
                cmdline_buf[0] = (char)args_len;
                memcpy(cmdline_buf + 1, args_ptr, args_len);
                cmdline_buf[args_len + 1] = '\r';
                cmdline_buf[args_len + 2] = '\0';
                block.cmdLine = cmdline_buf;
                block.fcb1 = NULL;
                block.fcb2 = NULL;

                int exit_code = lowLevelExec(resolved_path, &block);
                if (exit_code == -1) {
                    fprintf(stderr, "ZCPR: Failed to execute '%s'\n", cmd_name);
                }
            } else {
                fprintf(stderr, "Bad command or filename: '%s'\n", cmd_name);
            }
        }
    }

    printf("\nExiting ZCPR shell. Goodbye!\n");
    return 0;
}
