/* Phase 3 main: login + role-based dashboards (skeleton).
   Queue/appointments/consultation/reports menus are placeholders,
   filled in during later phases. */
#include "common.h"
#include "utils.h"
#include "file_manager.h"
#include "department.h"
#include "patient.h"
#include "doctor.h"
#include "auth.h"

static void notReady(const char *feature)
{
    printf(C_YELLOW "\n[%s] Coming in a later phase.\n" C_RESET, feature);
}

static void adminDashboard(const User *u)
{
    int ch;
    do {
        clearScreen();
        printHeader("ADMIN DASHBOARD");
        printf("Logged in as: %s\n\n", u->username);
        printf("1. Patient Management\n2. Doctor Management\n3. Department Management\n");
        printf("4. Queue Management\n5. Appointment Management\n6. Staff / User Management\n");
        printf("7. Reports & Statistics\n0. Logout\n\n");
        ch = getInt("Enter choice: ", 0, 7);
        switch (ch) {
            case 1: patient_menu(); break;
            case 2: doctor_menu();  break;
            case 3: dept_menu();    break;
            case 4: notReady("Queue Management");    pauseScreen(); break;
            case 5: notReady("Appointment Management"); pauseScreen(); break;
            case 6: auth_userMenu(); break;
            case 7: notReady("Reports & Statistics"); pauseScreen(); break;
            default: break;
        }
    } while (ch != 0);
}

static void doctorDashboard(const User *u)
{
    int ch;
    do {
        clearScreen();
        printHeader("DOCTOR DASHBOARD");
        printf("Logged in as: %s (Doctor ID: %d)\n\n", u->username, u->linkedId);
        printf("1. Today's Appointments\n2. Waiting / Emergency Queue\n3. Patient History\n");
        printf("4. Consultation (Diagnosis/Prescription)\n5. Completed Patients\n0. Logout\n\n");
        ch = getInt("Enter choice: ", 0, 5);
        switch (ch) {
            case 1: notReady("Today's Appointments"); pauseScreen(); break;
            case 2: notReady("Queue"); pauseScreen(); break;
            case 3: notReady("Patient History"); pauseScreen(); break;
            case 4: notReady("Consultation"); pauseScreen(); break;
            case 5: notReady("Completed Patients"); pauseScreen(); break;
            default: break;
        }
    } while (ch != 0);
}

static void receptionDashboard(const User *u)
{
    int ch;
    do {
        clearScreen();
        printHeader("RECEPTIONIST DASHBOARD");
        printf("Logged in as: %s\n\n", u->username);
        printf("1. Register Patient\n2. Book Appointment\n3. Add Patient to Queue\n");
        printf("4. View Waiting Queue\n5. Search Patient\n6. Check Patient Status\n0. Logout\n\n");
        ch = getInt("Enter choice: ", 0, 6);
        switch (ch) {
            case 1: patient_menu(); break;           /* Add Patient is inside here for now */
            case 2: notReady("Book Appointment"); pauseScreen(); break;
            case 3: notReady("Add to Queue"); pauseScreen(); break;
            case 4: notReady("Waiting Queue"); pauseScreen(); break;
            case 5: patient_menu(); break;            /* Search Patient is inside here for now */
            case 6: notReady("Check Patient Status"); pauseScreen(); break;
            default: break;
        }
    } while (ch != 0);
}

int main(void)
{
    const User *u;

    initTerminal();
    fm_init();
    dept_load();
    patient_load();
    doctor_load();
    auth_load();

    u = auth_login();
    if (u) {
        switch (u->role) {
            case ROLE_ADMIN:     adminDashboard(u); break;
            case ROLE_DOCTOR:    doctorDashboard(u); break;
            case ROLE_RECEPTION: receptionDashboard(u); break;
        }
    }

    patient_save();
    doctor_save();
    patient_freeAll();
    doctor_freeAll();
    auth_freeAll();
    printf("Goodbye!\n");
    return 0;
}