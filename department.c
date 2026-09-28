#include "department.h"
#include "file_manager.h"
#include "utils.h"

static Department depts[MAX_DEPTS];
static int count = 0;

static const char *defaults[] = {
    "General Medicine", "Cardiology", "Orthopedics", "Pediatrics", "ENT", "Emergency"
};

int dept_save(void)
{
    return fm_save(F_DEPARTMENTS, depts, sizeof(Department), count);
}

void dept_load(void)
{
    int n = 0, i;
    Department *arr = fm_load(F_DEPARTMENTS, sizeof(Department), &n);
    if (n > MAX_DEPTS) n = MAX_DEPTS;
    for (i = 0; i < n; i++) depts[i] = arr[i];
    count = n;
    free(arr);

    if (count == 0) {                       /* first run: create defaults */
        for (i = 0; i < 6; i++) {
            depts[i].id = i + 1;
            strncpy(depts[i].name, defaults[i], MAX_NAME - 1);
            depts[i].name[MAX_NAME - 1] = '\0';
        }
        count = 6;
        dept_save();
    }
}

int dept_count(void) { return count; }

int dept_exists(int id)
{
    int i;
    for (i = 0; i < count; i++) if (depts[i].id == id) return 1;
    return 0;
}

const char *dept_name(int id)
{
    int i;
    for (i = 0; i < count; i++) if (depts[i].id == id) return depts[i].name;
    return "Unknown";
}

void dept_list(void)
{
    int i;
    printf(C_BOLD "%-5s %-25s\n" C_RESET, "ID", "Department");
    for (i = 0; i < count; i++) printf("%-5d %-25s\n", depts[i].id, depts[i].name);
}

int dept_select(void)
{
    int id;
    dept_list();
    while (1) {
        id = getInt("Select Department ID: ", 1, 9999);
        if (dept_exists(id)) return id;
        printError("Department does not exist.");
    }
}

static void addDept(void)
{
    char name[MAX_NAME];
    int i;
    if (count >= MAX_DEPTS) { printError("Department limit reached."); return; }
    getString("New department name: ", name, MAX_NAME, 0);
    for (i = 0; i < count; i++)
        if (compareIgnoreCase(depts[i].name, name) == 0) {
            printError("Department already exists."); return;
        }
    depts[count].id = depts[count - 1].id + 1;
    strcpy(depts[count].name, name);
    count++;
    dept_save();
    printSuccess("Department added.");
}

void dept_menu(void)
{
    int ch;
    do {
        clearScreen();
        printHeader("DEPARTMENT MANAGEMENT");
        printf("1. View Departments\n2. Add Department\n0. Back\n\n");
        ch = getInt("Enter choice: ", 0, 2);
        switch (ch) {
            case 1: dept_list(); pauseScreen(); break;
            case 2: addDept();   pauseScreen(); break;
            default: break;
        }
    } while (ch != 0);
}
