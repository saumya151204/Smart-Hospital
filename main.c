/* Phase 2 main: temporary menu. Replaced by login + dashboards in Phase 3. */
#include "common.h"
#include "utils.h"
#include "file_manager.h"
#include "department.h"
#include "patient.h"
#include "doctor.h"

int main(void)
{
    int ch;

    initTerminal();
    fm_init();
    dept_load();
    patient_load();
    doctor_load();

    do {
        clearScreen();
        printHeader("SMART HOSPITAL - PHASE 2");
        printf("Date/Time: "); printCurrentDateTime(); printf("\n\n");
        printf("1. Patient Management\n2. Doctor Management\n3. Department Management\n0. Exit\n\n");
        ch = getInt("Enter choice: ", 0, 3);
        switch (ch) {
            case 1: patient_menu(); break;
            case 2: doctor_menu();  break;
            case 3: dept_menu();    break;
            case 0:
                if (!confirm("Exit program?")) ch = -1;
                break;
        }
    } while (ch != 0);

    patient_save();
    doctor_save();
    patient_freeAll();
    doctor_freeAll();
    printf("Goodbye!\n");
    return 0;
}
