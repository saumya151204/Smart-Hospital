# Smart Hospital Queue & Patient Management

A console-based (CLI) Hospital Queue & Patient Management System written in C.
The project manages patient registration, doctor records, department-wise
smart queues (priority-based), appointments, consultations, and role-based
dashboards for Admin, Doctor, and Receptionist users.

---

## 💻 Tech Stack

| Component     | Choice                          |
|---------------|----------------------------------|
| Language      | C                                 |
| Compiler      | GCC (MinGW / MSYS2 UCRT64)        |
| IDE           | VS Code                           |
| Storage       | Binary file handling (`.dat`)     |
| Version Control | Git + GitHub                    |
| Interface     | CLI / Terminal (colored, menu-driven) |

---

## 🧠 Core C Concepts Used

- `struct`, nested `struct`, `enum`
- Arrays, Pointers, Strings
- Dynamic Memory Allocation (`malloc` / `free`)
- Functions, Modular Programming, Header Files
- File Handling (`fopen`, `fread`, `fwrite`, `fseek`, `fclose`)
- `switch-case`, Loops
- Input Validation & Error Handling

## 🗃️ Data Structures & Algorithms

- **Linked List** — Patient, Doctor, Staff/User records (in memory)
- **Queue / Priority Queue** — Smart Queue system (Phase 4)
- **Structures** — `Patient`, `Doctor`, `Appointment`, `Consultation`, `Department`, `User`
- **Linear Search** — name / phone / department search
- **Binary Search** — Patient ID search (sorted by auto-increment ID)
- **Bubble Sort** — sort by ID / Name / Age
- **Selection Sort** — sort by Priority / Registration Time

---

## 📂 Project Structure

```
Smart-Hospital/
│
├── main.c              # Login + role-based dashboards
├── common.h            # Shared structs & enums
│
├── utils.c / utils.h           # Input validation, colors, date/time, password hashing
├── file_manager.c / file_manager.h   # Generic binary file save/load
├── auth.c / auth.h             # Login, hashed passwords, staff/user management
├── department.c / department.h # Department CRUD
├── patient.c / patient.h       # Patient CRUD, search, sort
├── doctor.c / doctor.h         # Doctor CRUD, search, availability
│
├── data/                # Auto-created at runtime
│   ├── patients.dat
│   ├── doctors.dat
│   ├── departments.dat
│   └── users.dat
│
├── Makefile
├── .gitignore
└── README.md
```

---

## ⚙️ Build & Run

```bash
mingw32-make        # compiles the project
.\hospital.exe      # runs it (Windows)
./hospital          # runs it (Linux/Mac)
```

To rebuild from scratch:
```bash
mingw32-make clean
mingw32-make
```

---

## 🔐 Default Login

On the very first run, a default admin account is created automatically:

| Username | Password  |
|----------|-----------|
| admin    | admin123  |

**Change this password after first login** (Staff/User Management is a
planned menu option for this).

---

## 👥 Roles & Dashboards

| Role | Access |
|---|---|
| **Admin** | Patient Management, Doctor Management, Department Management, Queue Management*, Appointment Management*, Staff/User Management, Reports & Statistics* |
| **Doctor** | Today's Appointments*, Waiting/Emergency Queue*, Patient History*, Consultation*, Completed Patients* |
| **Receptionist** | Register Patient, Book Appointment*, Add to Queue*, View Waiting Queue*, Search Patient, Check Patient Status* |

`*` = coming in a later phase (see Progress below).

Login locks out after 3 wrong password attempts.

---

## 📊 Progress

- [x] **Phase 1** — `common.h`, `utils`, `file_manager`, Makefile
- [x] **Phase 2** — Patient, Doctor, Department CRUD (add/view/search/update/delete, sort)
- [x] **Phase 3** — Login system (hashed passwords) + role-based dashboards (Admin / Doctor / Receptionist)
- [ ] **Phase 4** — Smart Queue System (priority queue: Emergency > Critical > Appointment > Walk-in, serve next, position, estimated wait time)
- [ ] **Phase 5** — Appointment Management (book/cancel/reschedule, doctor-wise & date-wise view)
- [ ] **Phase 6** — Consultation Module (diagnosis, prescription, follow-up)
- [ ] **Phase 7** — Full Search/Sort integration + Reports & Statistics
- [ ] **Phase 8** — Polish (confirmations, exit handling, final README pass)

---

## 🩺 Data Managed

**Patient:** ID, Name, Age, Gender, Phone, Address, Blood Group, Problem,
Department, Priority, Status, Registration Date

**Doctor:** ID, Name, Specialization, Department, Phone, Room, Availability

**Department:** ID, Name (6 defaults auto-created on first run)

**User (staff login):** Username, Hashed Password, Role, Linked Doctor ID

---

## 🧹 Notes

- Passwords are stored as a hash (djb2), never in plain text.
- File data persists between runs — patients/doctors added in one session
  are still there the next time you run the program.
- Duplicate patients (same name + phone) and duplicate doctor phone numbers
  are blocked on registration.

---

## Author

Saumya — MCA Project