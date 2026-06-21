/*
 * icmd_inc.h - Installable command definitions for 64-bit clean standard C17.
 *
 */

#ifndef ICMD_INC_H_
#define ICMD_INC_H_

typedef struct {
    const char *name;
    int (*check)(const char *cmdname);
    int (*perform)(const char *cmdname, const char *cmdline);
} installable_command_t;

#endif /* ICMD_INC_H_ */
