# University Management System — C++

Implementation for **Abeerah Amin's assigned Jira stories**.

## Assigned stories
- UMS-24 / US-01 — Add Faculty
- UMS-11 / US-03 — Update Faculty
- UMS-13 / US-05 — Assign Faculty to Course
- UMS-14 / US-06 — Create Examination
- UMS-17 / US-09 — Enter Student Marks
- UMS-19 / US-11 — View Available Courses
- UMS-22 / US-14 — Check Course Capacity

## Build

Requires a C++20 compiler.

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic main.cpp -o ums
./ums
```

Run tests:

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic tests.cpp -o ums_tests
./ums_tests
```

The application stores faculty, course, examination and marks data in the `data/` directory.

The registration, drop-course, exam scheduling/result viewing and faculty removal features are intentionally outside this member's assignment.
