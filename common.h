#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------- Constants ---------- */
#define MAX_NAME     50
#define MAX_PHONE    16
#define MAX_ADDR     100
#define MAX_TEXT     200
#define MAX_DEPTS    10
#define DATA_DIR     "data"
#define AVG_CONSULT_MIN 10   /* used for estimated waiting time */

/* ---------- Enums ---------- */
typedef enum { P_EMERGENCY = 1, P_CRITICAL, P_APPOINTMENT, P_WALKIN } Priority;
typedef enum { ST_WAITING, ST_IN_CONSULT, ST_COMPLETED, ST_CANCELLED } Status;
typedef enum { ROLE_ADMIN = 1, ROLE_DOCTOR, ROLE_RECEPTION } Role;
typedef enum { AP_BOOKED, AP_DONE, AP_CANCELLED } ApptStatus;

/* ---------- Nested struct ---------- */
typedef struct { int d, m, y; } Date;
typedef struct { int hh, mm; } Time;

/* ---------- Main records (saved in .dat files) ---------- */
typedef struct {
    int  id;
    char name[MAX_NAME];
    int  age;
    char gender;                 /* 'M', 'F', 'O' */
    char phone[MAX_PHONE];
    char address[MAX_ADDR];
    char bloodGroup[5];
    char problem[MAX_TEXT];
    int  deptId;
    Priority priority;
    Status   status;
    int  isEmergency;
    Date regDate;
    long regTime;                /* time_t as long, for sorting */
} Patient;

typedef struct {
    int  id;
    char name[MAX_NAME];
    char specialization[MAX_NAME];
    int  deptId;
    char phone[MAX_PHONE];
    int  available;              /* 1 = available, 0 = not */
    int  room;
} Doctor;

typedef struct {
    int  id;
    char name[MAX_NAME];
} Department;

typedef struct {
    int  id;
    int  patientId;
    int  doctorId;
    Date date;
    Time time;
    ApptStatus status;
} Appointment;

typedef struct {
    int  id;
    int  patientId;
    int  doctorId;
    char symptoms[MAX_TEXT];
    char diagnosis[MAX_TEXT];
    char prescription[MAX_TEXT];
    char notes[MAX_TEXT];
    Date followUp;
    Date consultDate;
} Consultation;

typedef struct {
    char username[MAX_NAME];
    unsigned long passHash;      /* hashed, never plain text */
    Role role;
    int  linkedId;               /* doctor id if role == DOCTOR */
} User;

#endif
