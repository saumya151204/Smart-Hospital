/* Phase 1 test program: checks utils + file_manager work. Replaced in Phase 3. */
#include "common.h"
#include "utils.h"
#include "file_manager.h"

int main(void)
{
    Patient p, *loaded;
    int n, i;

    initTerminal();
    fm_init();
    clearScreen();
    printHeader("SMART HOSPITAL - PHASE 1 TEST");
    printf("Date/Time: "); printCurrentDateTime(); printf("\n\n");

    memset(&p, 0, sizeof p);
    p.id = 1;
    getString("Patient name : ", p.name, MAX_NAME, 0);
    p.age    = getInt("Age          : ", 0, 120);
    p.gender = getGender("Gender (M/F/O): ");
    getPhone("Phone (10 digits): ", p.phone);
    p.priority = P_WALKIN;
    p.status   = ST_WAITING;
    p.regDate  = todayDate();

    if (fm_save(F_PATIENTS, &p, sizeof(Patient), 1) == 0)
        printSuccess("Saved to data/patients.dat");

    loaded = fm_load(F_PATIENTS, sizeof(Patient), &n);
    printf("\nLoaded %d record(s) from file:\n", n);
    for (i = 0; i < n; i++) {
        printf("ID %d | %s | %d | %c | %s | ", loaded[i].id, loaded[i].name,
               loaded[i].age, loaded[i].gender, loaded[i].phone);
        printDate(loaded[i].regDate);
        printf("\n");
    }
    free(loaded);

    printf("\nHash of 'admin123' = %lu\n", hashPassword("admin123"));
    pauseScreen();
    return 0;
}
