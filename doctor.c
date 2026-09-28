#include "doctor.h"
#include "department.h"
#include "file_manager.h"
#include "utils.h"

static DNode *head = NULL;
static int count = 0;

DNode *doctor_head(void)  { return head; }
int    doctor_count(void) { return count; }

static int appendNode(const Doctor *d)
{
    DNode *n = (DNode *)malloc(sizeof(DNode));
    DNode *c;
    if (!n) { printError("Memory allocation failed."); return -1; }
    n->data = *d;
    n->next = NULL;
    if (!head) head = n;
    else { c = head; while (c->next) c = c->next; c->next = n; }
    count++;
    return 0;
}

DNode *doctor_findById(int id)
{
    DNode *c;
    for (c = head; c; c = c->next) if (c->data.id == id) return c;
    return NULL;
}

static int nextId(void)
{
    DNode *c;
    int max = 0;
    for (c = head; c; c = c->next) if (c->data.id > max) max = c->data.id;
    return max + 1;
}

void doctor_freeAll(void)
{
    DNode *c = head, *t;
    while (c) { t = c->next; free(c); c = t; }
    head = NULL;
    count = 0;
}

void doctor_load(void)
{
    int n = 0, i;
    Doctor *arr = fm_load(F_DOCTORS, sizeof(Doctor), &n);
    for (i = 0; i < n; i++) appendNode(&arr[i]);
    free(arr);
}

int doctor_save(void)
{
    Doctor *arr = NULL;
    DNode *c;
    int i = 0, rc;
    if (count > 0) {
        arr = (Doctor *)malloc((size_t)count * sizeof(Doctor));
        if (!arr) { printError("Memory allocation failed."); return -1; }
        for (c = head; c; c = c->next) arr[i++] = c->data;
    }
    rc = fm_save(F_DOCTORS, arr, sizeof(Doctor), count);
    free(arr);
    return rc;
}

void doctor_printHeader(void)
{
    printf(C_BOLD "%-5s %-20s %-18s %-16s %-11s %-5s %-9s\n" C_RESET,
           "ID", "Name", "Specialization", "Department", "Phone", "Room", "Available");
    printf("-----------------------------------------------------------------------------------------\n");
}

void doctor_printRow(const Doctor *d)
{
    printf("%-5d %-20.20s %-18.18s %-16.16s %-11s %-5d %-9s\n",
           d->id, d->name, d->specialization, dept_name(d->deptId),
           d->phone, d->room, d->available ? "Yes" : "No");
}

static void printDetails(const Doctor *d)
{
    printf("Doctor ID      : %d\n", d->id);
    printf("Name           : %s\n", d->name);
    printf("Specialization : %s\n", d->specialization);
    printf("Department     : %s\n", dept_name(d->deptId));
    printf("Phone          : %s\n", d->phone);
    printf("Room           : %d\n", d->room);
    printf("Available      : %s\n", d->available ? "Yes" : "No");
}

static DNode *askDoctor(void)
{
    int id = getInt("Enter Doctor ID: ", 1, 999999);
    DNode *n = doctor_findById(id);
    if (!n) printError("Doctor not found.");
    return n;
}

static void addDoctor(void)
{
    Doctor d;
    DNode *c;
    clearScreen();
    printHeader("ADD DOCTOR");
    memset(&d, 0, sizeof d);

    getString("Name: ", d.name, MAX_NAME, 0);
    getString("Specialization: ", d.specialization, MAX_NAME, 0);
    getPhone("Phone (10 digits): ", d.phone);

    for (c = head; c; c = c->next)
        if (strcmp(c->data.phone, d.phone) == 0) {
            printf(C_RED "[ERROR] A doctor with this phone already exists (ID %d).\n" C_RESET, c->data.id);
            return;
        }

    d.deptId    = dept_select();
    d.room      = getInt("Room number (1-999): ", 1, 999);
    d.available = 1;
    d.id        = nextId();

    if (appendNode(&d) == 0 && doctor_save() == 0)
        printf(C_GREEN "[OK] Doctor added. Doctor ID = %d\n" C_RESET, d.id);
}

static void viewAll(int onlyAvailable)
{
    DNode *c;
    int shown = 0;
    clearScreen();
    printHeader(onlyAvailable ? "AVAILABLE DOCTORS" : "ALL DOCTORS");
    doctor_printHeader();
    for (c = head; c; c = c->next)
        if (!onlyAvailable || c->data.available) { doctor_printRow(&c->data); shown++; }
    if (!shown) printf("No doctors found.\n");
    else printf("\nTotal: %d\n", shown);
}

static void searchDoctor(void)
{
    int ch, id, dept = 0, found = 0;
    char key[MAX_NAME];
    DNode *c;

    clearScreen();
    printHeader("SEARCH DOCTOR");
    printf("1. By ID\n2. By Name\n3. By Department\n\n");
    ch = getInt("Enter choice: ", 1, 3);

    if (ch == 1) {
        id = getInt("Enter Doctor ID: ", 1, 999999);
        c = doctor_findById(id);
        if (c) { printf("\n"); printDetails(&c->data); } else printError("Doctor not found.");
        return;
    }
    if (ch == 2) getString("Enter name (or part of it): ", key, MAX_NAME, 0);
    else dept = dept_select();

    printf("\n");
    doctor_printHeader();
    for (c = head; c; c = c->next) {
        int match = (ch == 2) ? containsIgnoreCase(c->data.name, key)
                              : (c->data.deptId == dept);
        if (match) { doctor_printRow(&c->data); found++; }
    }
    if (!found) printError("No matching doctor.");
}

static void updateDoctor(void)
{
    DNode *n;
    int ch;
    clearScreen();
    printHeader("UPDATE DOCTOR");
    n = askDoctor();
    if (!n) return;

    do {
        clearScreen();
        printHeader("UPDATE DOCTOR");
        printDetails(&n->data);
        printf("\n1.Name 2.Specialization 3.Department 4.Phone 5.Room 6.Toggle Availability 0.Save & Back\n\n");
        ch = getInt("Field to edit: ", 0, 6);
        switch (ch) {
            case 1: getString("New name: ", n->data.name, MAX_NAME, 0); break;
            case 2: getString("New specialization: ", n->data.specialization, MAX_NAME, 0); break;
            case 3: n->data.deptId = dept_select(); break;
            case 4: getPhone("New phone: ", n->data.phone); break;
            case 5: n->data.room = getInt("New room (1-999): ", 1, 999); break;
            case 6: n->data.available = !n->data.available; break;
            default: break;
        }
    } while (ch != 0);

    if (doctor_save() == 0) printSuccess("Doctor updated.");
}

static void deleteDoctor(void)
{
    DNode *prev = NULL, *c;
    int id;
    clearScreen();
    printHeader("DELETE DOCTOR");
    id = getInt("Enter Doctor ID: ", 1, 999999);
    for (c = head; c && c->data.id != id; prev = c, c = c->next) ;
    if (!c) { printError("Doctor not found."); return; }

    printf("\n");
    printDetails(&c->data);
    printf("\n");
    if (!confirm("Really delete this doctor?")) { printf("Cancelled.\n"); return; }

    if (prev) prev->next = c->next; else head = c->next;
    free(c);
    count--;
    if (doctor_save() == 0) printSuccess("Doctor deleted.");
}

void doctor_menu(void)
{
    int ch;
    do {
        clearScreen();
        printHeader("DOCTOR MANAGEMENT");
        printf("1. Add Doctor\n2. View All Doctors\n3. View Available Doctors\n");
        printf("4. Search Doctor\n5. Update Doctor\n6. Delete Doctor\n0. Back\n\n");
        ch = getInt("Enter choice: ", 0, 6);
        switch (ch) {
            case 1: addDoctor();     pauseScreen(); break;
            case 2: viewAll(0);      pauseScreen(); break;
            case 3: viewAll(1);      pauseScreen(); break;
            case 4: searchDoctor();  pauseScreen(); break;
            case 5: updateDoctor();  pauseScreen(); break;
            case 6: deleteDoctor();  pauseScreen(); break;
            default: break;
        }
    } while (ch != 0);
}
