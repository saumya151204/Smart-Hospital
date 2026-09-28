#include "patient.h"
#include "department.h"
#include "file_manager.h"
#include "utils.h"
#include <time.h>

static PNode *head = NULL;
static int count = 0;

/* ---------- names for enums ---------- */
const char *priorityName(Priority p)
{
    switch (p) {
        case P_EMERGENCY:   return "Emergency";
        case P_CRITICAL:    return "Critical";
        case P_APPOINTMENT: return "Appointment";
        case P_WALKIN:      return "Walk-in";
    }
    return "?";
}

const char *statusName(Status s)
{
    switch (s) {
        case ST_WAITING:    return "Waiting";
        case ST_IN_CONSULT: return "In consult";
        case ST_COMPLETED:  return "Completed";
        case ST_CANCELLED:  return "Cancelled";
    }
    return "?";
}

/* ---------- linked list ---------- */
PNode *patient_head(void)  { return head; }
int    patient_count(void) { return count; }

static int appendNode(const Patient *p)
{
    PNode *n = (PNode *)malloc(sizeof(PNode));
    PNode *c;
    if (!n) { printError("Memory allocation failed."); return -1; }
    n->data = *p;
    n->next = NULL;
    if (!head) head = n;
    else { c = head; while (c->next) c = c->next; c->next = n; }
    count++;
    return 0;
}

PNode *patient_findById(int id)
{
    PNode *c;
    for (c = head; c; c = c->next) if (c->data.id == id) return c;
    return NULL;
}

static int nextId(void)
{
    PNode *c;
    int max = 0;
    for (c = head; c; c = c->next) if (c->data.id > max) max = c->data.id;
    return max + 1;
}

void patient_freeAll(void)
{
    PNode *c = head, *t;
    while (c) { t = c->next; free(c); c = t; }
    head = NULL;
    count = 0;
}

/* ---------- file I/O ---------- */
void patient_load(void)
{
    int n = 0, i;
    Patient *arr = fm_load(F_PATIENTS, sizeof(Patient), &n);
    for (i = 0; i < n; i++) appendNode(&arr[i]);
    free(arr);
}

int patient_save(void)
{
    Patient *arr = NULL;
    PNode *c;
    int i = 0, rc;
    if (count > 0) {
        arr = (Patient *)malloc((size_t)count * sizeof(Patient));
        if (!arr) { printError("Memory allocation failed."); return -1; }
        for (c = head; c; c = c->next) arr[i++] = c->data;
    }
    rc = fm_save(F_PATIENTS, arr, sizeof(Patient), count);
    free(arr);
    return rc;
}

/* ---------- display ---------- */
void patient_printHeader(void)
{
    printf(C_BOLD "%-5s %-20s %-4s %-2s %-11s %-16s %-12s %-11s\n" C_RESET,
           "ID", "Name", "Age", "G", "Phone", "Department", "Priority", "Status");
    printf("---------------------------------------------------------------------------------------\n");
}

void patient_printRow(const Patient *p)
{
    printf("%-5d %-20.20s %-4d %-2c %-11s %-16.16s %-12s %-11s\n",
           p->id, p->name, p->age, p->gender, p->phone,
           dept_name(p->deptId), priorityName(p->priority), statusName(p->status));
}

static void printDetails(const Patient *p)
{
    printf("Patient ID   : %d\n", p->id);
    printf("Name         : %s\n", p->name);
    printf("Age / Gender : %d / %c\n", p->age, p->gender);
    printf("Phone        : %s\n", p->phone);
    printf("Address      : %s\n", p->address);
    printf("Blood Group  : %s\n", p->bloodGroup);
    printf("Problem      : %s\n", p->problem);
    printf("Department   : %s\n", dept_name(p->deptId));
    printf("Priority     : %s\n", priorityName(p->priority));
    printf("Status       : %s\n", statusName(p->status));
    printf("Registered   : "); printDate(p->regDate); printf("\n");
}

/* ---------- input helpers ---------- */
static void getBloodGroup(char *out)
{
    static const char *valid[] = { "A+", "A-", "B+", "B-", "O+", "O-", "AB+", "AB-" };
    char buf[8];
    int i;
    while (1) {
        getString("Blood group (A+ A- B+ B- O+ O- AB+ AB-): ", buf, sizeof buf, 0);
        for (i = 0; i < 8; i++)
            if (compareIgnoreCase(buf, valid[i]) == 0) { strcpy(out, valid[i]); return; }
        printError("Invalid blood group.");
    }
}

static PNode *askPatient(void)
{
    int id = getInt("Enter Patient ID: ", 1, 999999);
    PNode *n = patient_findById(id);
    if (!n) printError("Patient not found.");
    return n;
}

/* ---------- CRUD ---------- */
static void addPatient(void)
{
    Patient p;
    PNode *c;
    clearScreen();
    printHeader("ADD PATIENT");
    memset(&p, 0, sizeof p);

    getString("Name: ", p.name, MAX_NAME, 0);
    p.age    = getInt("Age (0-120): ", 0, 120);
    p.gender = getGender("Gender (M/F/O): ");
    getPhone("Phone (10 digits): ", p.phone);

    for (c = head; c; c = c->next)
        if (strcmp(c->data.phone, p.phone) == 0 &&
            compareIgnoreCase(c->data.name, p.name) == 0) {
            printf(C_RED "[ERROR] Duplicate: this patient already exists (ID %d).\n" C_RESET, c->data.id);
            return;
        }

    getString("Address: ", p.address, MAX_ADDR, 0);
    getBloodGroup(p.bloodGroup);
    getString("Problem / Disease: ", p.problem, MAX_TEXT, 0);
    p.deptId      = dept_select();
    p.isEmergency = confirm("Is this an emergency case?");
    p.priority    = p.isEmergency ? P_EMERGENCY : P_WALKIN;
    p.status      = ST_WAITING;
    p.id          = nextId();
    p.regDate     = todayDate();
    p.regTime     = (long)time(NULL);

    if (appendNode(&p) == 0 && patient_save() == 0)
        printf(C_GREEN "[OK] Patient registered. Patient ID = %d\n" C_RESET, p.id);
}

static void viewAll(void)
{
    PNode *c;
    clearScreen();
    printHeader("ALL PATIENTS");
    if (!head) { printf("No patients found.\n"); return; }
    patient_printHeader();
    for (c = head; c; c = c->next) patient_printRow(&c->data);
    printf("\nTotal: %d\n", count);
}

/* array of pointers, used for binary search and sorting */
static Patient **buildArray(void)
{
    Patient **arr;
    PNode *c;
    int i = 0;
    if (count == 0) return NULL;
    arr = (Patient **)malloc((size_t)count * sizeof(Patient *));
    if (!arr) return NULL;
    for (c = head; c; c = c->next) arr[i++] = &c->data;
    return arr;
}

static Patient *binarySearchId(int id)   /* list is in ID order (auto-increment) */
{
    Patient **arr = buildArray();
    Patient *found = NULL;
    int lo = 0, hi = count - 1, mid;
    if (!arr) return NULL;
    while (lo <= hi) {
        mid = (lo + hi) / 2;
        if (arr[mid]->id == id)      { found = arr[mid]; break; }
        else if (arr[mid]->id < id)  lo = mid + 1;
        else                         hi = mid - 1;
    }
    free(arr);
    return found;
}

static void searchPatient(void)
{
    int ch, id, dept, found = 0;
    char key[MAX_NAME];
    PNode *c;
    Patient *p;

    clearScreen();
    printHeader("SEARCH PATIENT");
    printf("1. By ID (binary search)\n2. By Name (linear search)\n3. By Phone\n4. By Department\n\n");
    ch = getInt("Enter choice: ", 1, 4);

    switch (ch) {
        case 1:
            id = getInt("Enter Patient ID: ", 1, 999999);
            p = binarySearchId(id);
            if (p) { printf("\n"); printDetails(p); } else printError("Patient not found.");
            return;
        case 2:
            getString("Enter name (or part of it): ", key, MAX_NAME, 0);
            break;
        case 3:
            getPhone("Enter phone: ", key);
            break;
        default:
            dept = dept_select();
            break;
    }

    printf("\n");
    patient_printHeader();
    for (c = head; c; c = c->next) {
        int match = 0;
        if (ch == 2) match = containsIgnoreCase(c->data.name, key);
        else if (ch == 3) match = (strcmp(c->data.phone, key) == 0);
        else match = (c->data.deptId == dept);
        if (match) { patient_printRow(&c->data); found++; }
    }
    if (!found) printError("No matching patient.");
    else printf("\n%d record(s) found.\n", found);
}

static void updatePatient(void)
{
    PNode *n;
    int ch;
    clearScreen();
    printHeader("UPDATE PATIENT");
    n = askPatient();
    if (!n) return;

    do {
        clearScreen();
        printHeader("UPDATE PATIENT");
        printDetails(&n->data);
        printf("\n1.Name 2.Age 3.Gender 4.Phone 5.Address 6.Blood Group\n");
        printf("7.Problem 8.Department 9.Emergency status 0.Save & Back\n\n");
        ch = getInt("Field to edit: ", 0, 9);
        switch (ch) {
            case 1: getString("New name: ", n->data.name, MAX_NAME, 0); break;
            case 2: n->data.age = getInt("New age: ", 0, 120); break;
            case 3: n->data.gender = getGender("New gender (M/F/O): "); break;
            case 4: getPhone("New phone: ", n->data.phone); break;
            case 5: getString("New address: ", n->data.address, MAX_ADDR, 0); break;
            case 6: getBloodGroup(n->data.bloodGroup); break;
            case 7: getString("New problem: ", n->data.problem, MAX_TEXT, 0); break;
            case 8: n->data.deptId = dept_select(); break;
            case 9:
                n->data.isEmergency = confirm("Mark as emergency?");
                n->data.priority = n->data.isEmergency ? P_EMERGENCY : P_WALKIN;
                break;
            default: break;
        }
    } while (ch != 0);

    if (patient_save() == 0) printSuccess("Patient updated.");
}

static void deletePatient(void)
{
    PNode *prev = NULL, *c;
    int id;
    clearScreen();
    printHeader("DELETE PATIENT");
    id = getInt("Enter Patient ID: ", 1, 999999);
    for (c = head; c && c->data.id != id; prev = c, c = c->next) ;
    if (!c) { printError("Patient not found."); return; }

    printf("\n");
    printDetails(&c->data);
    printf("\n");
    if (!confirm("Really delete this patient?")) { printf("Cancelled.\n"); return; }

    if (prev) prev->next = c->next; else head = c->next;
    free(c);
    count--;
    if (patient_save() == 0) printSuccess("Patient deleted.");
}

/* ---------- sorting ---------- */
/* key: 1=ID 2=Name 3=Age 4=Priority 5=Registration time. Returns >0 if a goes after b */
static int cmpBy(int key, const Patient *a, const Patient *b)
{
    switch (key) {
        case 1: return a->id - b->id;
        case 2: return compareIgnoreCase(a->name, b->name);
        case 3: return a->age - b->age;
        case 4:
            if (a->priority != b->priority) return (int)a->priority - (int)b->priority;
            return (a->regTime > b->regTime) - (a->regTime < b->regTime);
        default: return (a->regTime > b->regTime) - (a->regTime < b->regTime);
    }
}

static void bubbleSort(Patient **a, int n, int key)
{
    int i, j;
    Patient *t;
    for (i = 0; i < n - 1; i++)
        for (j = 0; j < n - 1 - i; j++)
            if (cmpBy(key, a[j], a[j + 1]) > 0) { t = a[j]; a[j] = a[j + 1]; a[j + 1] = t; }
}

static void selectionSort(Patient **a, int n, int key)
{
    int i, j, m;
    Patient *t;
    for (i = 0; i < n - 1; i++) {
        m = i;
        for (j = i + 1; j < n; j++) if (cmpBy(key, a[j], a[m]) < 0) m = j;
        t = a[i]; a[i] = a[m]; a[m] = t;
    }
}

static void sortView(void)
{
    Patient **arr;
    int key, i;
    clearScreen();
    printHeader("SORT PATIENTS");
    if (count == 0) { printf("No patients found.\n"); return; }
    printf("1. ID\n2. Name (A-Z)\n3. Age\n4. Priority\n5. Registration Time\n\n");
    key = getInt("Sort by: ", 1, 5);

    arr = buildArray();
    if (!arr) { printError("Memory allocation failed."); return; }
    if (key <= 3) bubbleSort(arr, count, key);      /* bubble sort for ID/name/age */
    else          selectionSort(arr, count, key);   /* selection sort for priority/time */

    printf("\n");
    patient_printHeader();
    for (i = 0; i < count; i++) patient_printRow(arr[i]);
    free(arr);                                      /* original list is untouched */
}

/* ---------- menu ---------- */
void patient_menu(void)
{
    int ch;
    do {
        clearScreen();
        printHeader("PATIENT MANAGEMENT");
        printf("1. Add Patient\n2. View All Patients\n3. Search Patient\n");
        printf("4. Update Patient\n5. Delete Patient\n6. Sort & View\n0. Back\n\n");
        ch = getInt("Enter choice: ", 0, 6);
        switch (ch) {
            case 1: addPatient();    pauseScreen(); break;
            case 2: viewAll();       pauseScreen(); break;
            case 3: searchPatient(); pauseScreen(); break;
            case 4: updatePatient(); pauseScreen(); break;
            case 5: deletePatient(); pauseScreen(); break;
            case 6: sortView();      pauseScreen(); break;
            default: break;
        }
    } while (ch != 0);
}
