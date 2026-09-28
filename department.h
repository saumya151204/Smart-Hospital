#ifndef DEPARTMENT_H
#define DEPARTMENT_H

#include "common.h"

void        dept_load(void);
int         dept_save(void);
const char *dept_name(int id);
int         dept_exists(int id);
int         dept_count(void);
void        dept_list(void);
int         dept_select(void);      /* asks user, returns valid department id */
void        dept_menu(void);

#endif
