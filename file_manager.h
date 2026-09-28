#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <stddef.h>

#define F_PATIENTS      "data/patients.dat"
#define F_DOCTORS       "data/doctors.dat"
#define F_DEPARTMENTS   "data/departments.dat"
#define F_APPOINTMENTS  "data/appointments.dat"
#define F_CONSULTATIONS "data/consultations.dat"
#define F_USERS         "data/users.dat"

/* Creates the data/ folder if missing. Returns 0 on success. */
int fm_init(void);

/* Writes 'count' records of 'size' bytes. Returns 0 on success, -1 on error. */
int fm_save(const char *file, const void *data, size_t size, int count);

/* Reads all records. Returns malloc'd array (caller must free) and sets *count.
   Returns NULL with *count = 0 if the file does not exist or is empty. */
void *fm_load(const char *file, size_t size, int *count);

#endif
