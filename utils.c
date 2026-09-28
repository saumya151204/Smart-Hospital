#include "utils.h"
#include <ctype.h>
#include <time.h>

#ifdef _WIN32
  #include <windows.h>
  #include <conio.h>
#else
  #include <termios.h>
  #include <unistd.h>
#endif

void initTerminal(void)
{
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode))
        SetConsoleMode(h, mode | 0x0004); /* ENABLE_VIRTUAL_TERMINAL_PROCESSING */
#endif
}

void clearScreen(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen(void)
{
    char buf[8];
    printf("\nPress Enter to continue...");
    fgets(buf, sizeof buf, stdin);
}

void printHeader(const char *title)
{
    printf(C_CYAN C_BOLD);
    printf("==================================================\n");
    printf("  %s\n", title);
    printf("==================================================\n");
    printf(C_RESET);
}

void printError(const char *msg)   { printf(C_RED "[ERROR] %s\n" C_RESET, msg); }
void printSuccess(const char *msg) { printf(C_GREEN "[OK] %s\n" C_RESET, msg); }

/* ---------- Input ---------- */
static void trimNewline(char *s)
{
    size_t n = strlen(s);
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r')) s[--n] = '\0';
}

int getInt(const char *msg, int min, int max)
{
    char buf[32];
    char *end;
    long v;
    while (1) {
        printf("%s", msg);
        if (!fgets(buf, sizeof buf, stdin)) { clearerr(stdin); continue; }
        v = strtol(buf, &end, 10);
        if (end != buf && (*end == '\n' || *end == '\0') && v >= min && v <= max)
            return (int)v;
        printf(C_RED "Invalid input. Enter a number between %d and %d.\n" C_RESET, min, max);
    }
}

void getString(const char *msg, char *out, int size, int allowEmpty)
{
    while (1) {
        printf("%s", msg);
        if (!fgets(out, size, stdin)) { clearerr(stdin); continue; }
        if (strchr(out, '\n') == NULL) {            /* input too long: flush rest */
            int c; while ((c = getchar()) != '\n' && c != EOF);
        }
        trimNewline(out);
        if (out[0] != '\0' || allowEmpty) return;
        printf(C_RED "Input cannot be empty.\n" C_RESET);
    }
}

void getPhone(const char *msg, char *out)
{
    while (1) {
        int ok = 1, i;
        getString(msg, out, MAX_PHONE, 0);
        if (strlen(out) != 10) ok = 0;
        for (i = 0; ok && out[i]; i++)
            if (!isdigit((unsigned char)out[i])) ok = 0;
        if (ok) return;
        printf(C_RED "Phone must be exactly 10 digits.\n" C_RESET);
    }
}

char getGender(const char *msg)
{
    char buf[8];
    while (1) {
        getString(msg, buf, sizeof buf, 0);
        char g = (char)toupper((unsigned char)buf[0]);
        if (buf[1] == '\0' && (g == 'M' || g == 'F' || g == 'O')) return g;
        printf(C_RED "Enter M, F or O.\n" C_RESET);
    }
}

int confirm(const char *msg)
{
    char buf[8];
    while (1) {
        printf("%s (y/n): ", msg);
        if (!fgets(buf, sizeof buf, stdin)) { clearerr(stdin); continue; }
        if (buf[0] == 'y' || buf[0] == 'Y') return 1;
        if (buf[0] == 'n' || buf[0] == 'N') return 0;
    }
}

/* ---------- Date / time ---------- */
Date todayDate(void)
{
    time_t t = time(NULL);
    struct tm *lt = localtime(&t);
    Date d = { lt->tm_mday, lt->tm_mon + 1, lt->tm_year + 1900 };
    return d;
}

void printDate(Date d) { printf("%02d-%02d-%04d", d.d, d.m, d.y); }

void printCurrentDateTime(void)
{
    time_t t = time(NULL);
    char buf[64];
    strftime(buf, sizeof buf, "%d-%m-%Y  %H:%M:%S", localtime(&t));
    printf("%s", buf);
}

/* ---------- Security ---------- */
unsigned long hashPassword(const char *s)      /* djb2 */
{
    unsigned long h = 5381;
    int c;
    while ((c = (unsigned char)*s++)) h = ((h << 5) + h) + c;
    return h;
}

void getPassword(const char *msg, char *out, int size)
{
    int i = 0, c;
    printf("%s", msg);
    fflush(stdout);
#ifdef _WIN32
    while ((c = _getch()) != '\r' && c != '\n') {
        if (c == '\b') { if (i > 0) { i--; printf("\b \b"); } }
        else if (i < size - 1 && c >= 32) { out[i++] = (char)c; printf("*"); }
    }
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt; newt.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    while ((c = getchar()) != '\n' && c != EOF) {
        if (c == 127 || c == '\b') { if (i > 0) { i--; printf("\b \b"); } }
        else if (i < size - 1 && c >= 32) { out[i++] = (char)c; printf("*"); }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    out[i] = '\0';
    printf("\n");
}
