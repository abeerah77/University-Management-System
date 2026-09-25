# University Management System — Complete C++ Project

Complete implementation of the **15 user stories** from the University Management System assignment, organized in the Jira project across **UMS Sprint 1** and **UMS Sprint 2**.

## User stories implemented

### Epic 1 — Faculty Management
- UMS-24 / US-01 — Add Faculty
- UMS-10 / US-02 — View Faculty
- UMS-11 / US-03 — Update Faculty
- UMS-12 / US-04 — Remove Faculty
- UMS-13 / US-05 — Assign Faculty to Course

### Epic 2 — Examination Management
- UMS-14 / US-06 — Create Examination
- UMS-15 / US-07 — Schedule Examination
- UMS-16 / US-08 — Assign Examination to Course
- UMS-17 / US-09 — Enter Student Marks
- UMS-18 / US-10 — View Examination Results

### Epic 3 — Course Registration
- UMS-19 / US-11 — View Available Courses
- UMS-20 / US-12 — Register for Course
- UMS-21 / US-13 — Prevent Duplicate Registration
- UMS-22 / US-14 — Check Course Capacity
- UMS-23 / US-15 — Drop Course

## C++ implementation

The application is a C++20 console project with file-based persistence. It implements validation, CRUD operations, faculty/course associations, exam scheduling, exam-course association, marks entry with faculty authorization, student result access control, course registration, duplicate-registration prevention, capacity enforcement, and course dropping with confirmation.

Data files are stored under `data/`.

## Build

Requires a C++20 compiler.

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic main.cpp -o ums
./ums
```

## Test

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic tests.cpp -o ums_tests
./ums_tests
```

The test suite covers all 15 user-story areas plus persistence checks.

## CI

GitHub Actions builds the application and test executable and runs the complete C++ test suite on pushes and pull requests.

## Project management

Jira project: **UMS — University Management System**

Two completed Scrum sprints:
- **UMS Sprint 1** — Faculty foundation, examination setup, and core registration workflows.
- **UMS Sprint 2** — Remaining faculty/examination/course workflows and full-system integration.

The project is implemented collaboratively between **Abeerah Amin** and **Wardah Waleed**, while the repository contains the integrated complete project.
