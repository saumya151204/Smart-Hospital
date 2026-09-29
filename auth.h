#ifndef AUTH_H
#define AUTH_H

#include "common.h"

typedef struct UNode {
    User data;
    struct UNode *next;
} UNode;

void  auth_load(void);
int   auth_save(void);
void  auth_freeAll(void);

/* Returns pointer to logged-in user's data, or NULL if login failed.
   Locks out after 3 wrong attempts (returns NULL). */
const User *auth_login(void);

int   auth_addUser(const char *username, const char *password, Role role, int linkedId);
int   auth_changePassword(const char *username);
void  auth_userMenu(void);   /* Admin: manage staff/users */

#endif