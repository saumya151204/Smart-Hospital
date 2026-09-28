#ifndef UTILS_H
#define UTILS_H

#include "common.h"

/* ANSI colors */
#define C_RESET  "\033[0m"
#define C_RED    "\033[31m"
#define C_GREEN  "\033[32m"
#define C_YELLOW "\033[33m"
#define C_CYAN   "\033[36m"
#define C_BOLD   "\033[1m"

void initTerminal(void);            /* enables ANSI colors on Windows */
void clearScreen(void);
void pauseScreen(void);
void printHeader(const char *title);
void printError(const char *msg);
void printSuccess(const char *msg);

/* Input helpers (validated) */
int  getInt(const char *msg, int min, int max);
void getString(const char *msg, char *out, int size, int allowEmpty);
void getPhone(const char *msg, char *out);
char getGender(const char *msg);
int  confirm(const char *msg);      /* 1 = yes, 0 = no */

/* Date / time */
Date todayDate(void);
void printDate(Date d);
void printCurrentDateTime(void);

/* Security */
unsigned long hashPassword(const char *s);   /* djb2 */
void getPassword(const char *msg, char *out, int size);

#endif
