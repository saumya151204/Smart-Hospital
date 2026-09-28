#ifndef DOCTOR_H
#define DOCTOR_H

#include "common.h"

typedef struct DNode {
    Doctor data;
    struct DNode *next;
} DNode;

void   doctor_load(void);
int    doctor_save(void);
void   doctor_freeAll(void);
DNode *doctor_head(void);
int    doctor_count(void);
DNode *doctor_findById(int id);

void   doctor_printHeader(void);
void   doctor_printRow(const Doctor *d);

void   doctor_menu(void);

#endif
