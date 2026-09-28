#ifndef PATIENT_H
#define PATIENT_H

#include "common.h"

typedef struct PNode {
    Patient data;
    struct PNode *next;
} PNode;

void   patient_load(void);
int    patient_save(void);
void   patient_freeAll(void);
PNode *patient_head(void);
int    patient_count(void);
PNode *patient_findById(int id);

void   patient_printHeader(void);
void   patient_printRow(const Patient *p);
const char *priorityName(Priority p);
const char *statusName(Status s);

void   patient_menu(void);

#endif
