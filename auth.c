#include "auth.h"
#include "file_manager.h"
#include "utils.h"

static UNode *head = NULL;
static int count = 0;

static int appendNode(const User *u)
{
    UNode *n = (UNode *)malloc(sizeof(UNode));
    UNode *c;
    if (!n) { printError("Memory allocation failed."); return -1; }
    n->data = *u;
    n->next = NULL;
    if (!head) head = n;
    else { c = head; while (c->next) c = c->next; c->next = n; }
    count++;
    return 0;
}

static UNode *findUser(const char *username)
{
    UNode *c;
    for (c = head; c; c = c->next)
        if (compareIgnoreCase(c->data.username, username) == 0) return c;
    return NULL;
}

void auth_freeAll(void)
{
    UNode *c = head, *t;
    while (c) { t = c->next; free(c); c = t; }
    head = NULL;
    count = 0;
}

int auth_save(void)
{
    User *arr = NULL;
    UNode *c;
    int i = 0, rc;
    if (count > 0) {
        arr = (User *)malloc((size_t)count * sizeof(User));
        if (!arr) { printError("Memory allocation failed."); return -1; }
        for (c = head; c; c = c->next) arr[i++] = c->data;
    }
    rc = fm_save(F_USERS, arr, sizeof(User), count);
    free(arr);
    return rc;
}

void auth_load(void)
{
    int n = 0, i;
    User *arr = fm_load(F_USERS, sizeof(User), &n);
    for (i = 0; i < n; i++) appendNode(&arr[i]);
    free(arr);

    if (count == 0) {                     /* first run: create default admin */
        User u;
        memset(&u, 0, sizeof u);
        strcpy(u.username, "admin");
        u.passHash = hashPassword("admin123");
        u.role = ROLE_ADMIN;
        u.linkedId = 0;
        appendNode(&u);
        auth_save();
        printf(C_YELLOW "First run: default admin account created.\n");
        printf("  Username: admin\n  Password: admin123\n");
        printf("  Please change this password after logging in.\n" C_RESET);
        pauseScreen();
    }
}

int auth_addUser(const char *username, const char *password, Role role, int linkedId)
{
    User u;
    if (findUser(username)) { printError("Username already exists."); return -1; }
    memset(&u, 0, sizeof u);
    strncpy(u.username, username, MAX_NAME - 1);
    u.passHash = hashPassword(password);
    u.role = role;
    u.linkedId = linkedId;
    if (appendNode(&u) != 0) return -1;
    return auth_save();
}

int auth_changePassword(const char *username)
{
    UNode *n = findUser(username);
    char pass1[MAX_NAME], pass2[MAX_NAME];
    if (!n) { printError("User not found."); return -1; }
    getPassword("New password: ", pass1, sizeof pass1);
    getPassword("Confirm password: ", pass2, sizeof pass2);
    if (strcmp(pass1, pass2) != 0) { printError("Passwords do not match."); return -1; }
    n->data.passHash = hashPassword(pass1);
    if (auth_save() == 0) { printSuccess("Password changed."); return 0; }
    return -1;
}

const User *auth_login(void)
{
    char username[MAX_NAME], password[MAX_NAME];
    int attempts;
    UNode *n;

    for (attempts = 0; attempts < 3; attempts++) {
        clearScreen();
        printHeader("SMART HOSPITAL - LOGIN");
        getString("Username: ", username, MAX_NAME, 0);
        getPassword("Password: ", password, sizeof password);

        n = findUser(username);
        if (n && n->data.passHash == hashPassword(password)) {
            printSuccess("Login successful.");
            pauseScreen();
            return &n->data;
        }
        printf(C_RED "\nInvalid username or password. Attempts left: %d\n" C_RESET, 2 - attempts);
        pauseScreen();
    }
    printError("Too many failed attempts. Exiting.");
    return NULL;
}

static const char *roleName(Role r)
{
    switch (r) {
        case ROLE_ADMIN:     return "Admin";
        case ROLE_DOCTOR:    return "Doctor";
        case ROLE_RECEPTION: return "Receptionist";
    }
    return "?";
}

static void listUsers(void)
{
    UNode *c;
    printf(C_BOLD "%-20s %-14s\n" C_RESET, "Username", "Role");
    for (c = head; c; c = c->next) printf("%-20s %-14s\n", c->data.username, roleName(c->data.role));
}

static void addUserFlow(void)
{
    char username[MAX_NAME], pass1[MAX_NAME], pass2[MAX_NAME];
    int roleCh, linkedId = 0;
    getString("New username: ", username, MAX_NAME, 0);
    if (findUser(username)) { printError("Username already exists."); return; }
    getPassword("Password: ", pass1, sizeof pass1);
    getPassword("Confirm password: ", pass2, sizeof pass2);
    if (strcmp(pass1, pass2) != 0) { printError("Passwords do not match."); return; }
    printf("1. Admin\n2. Doctor\n3. Receptionist\n");
    roleCh = getInt("Role: ", 1, 3);
    if (roleCh == 2) linkedId = getInt("Linked Doctor ID: ", 1, 999999);
    if (auth_addUser(username, pass1, (Role)roleCh, linkedId) == 0)
        printSuccess("User created.");
}

static void deleteUserFlow(void)
{
    char username[MAX_NAME];
    UNode *prev = NULL, *c;
    getString("Username to delete: ", username, MAX_NAME, 0);
    if (compareIgnoreCase(username, "admin") == 0) {
        printError("Cannot delete the default admin account.");
        return;
    }
    for (c = head; c && compareIgnoreCase(c->data.username, username) != 0; prev = c, c = c->next) ;
    if (!c) { printError("User not found."); return; }
    if (!confirm("Really delete this user?")) { printf("Cancelled.\n"); return; }
    if (prev) prev->next = c->next; else head = c->next;
    free(c);
    count--;
    if (auth_save() == 0) printSuccess("User deleted.");
}

void auth_userMenu(void)
{
    int ch;
    do {
        clearScreen();
        printHeader("STAFF / USER MANAGEMENT");
        printf("1. View Users\n2. Add User\n3. Delete User\n0. Back\n\n");
        ch = getInt("Enter choice: ", 0, 3);
        switch (ch) {
            case 1: listUsers();     pauseScreen(); break;
            case 2: addUserFlow();   pauseScreen(); break;
            case 3: deleteUserFlow();pauseScreen(); break;
            default: break;
        }
    } while (ch != 0);
}