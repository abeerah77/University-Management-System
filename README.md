# University Management System — C++11 / MSYS2 UCRT64

Implementation for **Wardah's assigned Jira stories**.

## Assigned stories

- UMS-24 / US-01 — Add Faculty
- UMS-11 / US-03 — Update Faculty
- UMS-13 / US-05 — Assign Faculty to Course
- UMS-14 / US-06 — Create Examination
- UMS-17 / US-09 — Enter Student Marks
- UMS-19 / US-11 — View Available Courses
- UMS-22 / US-14 — Check Course Capacity

### Faculty Management
- US-01 Add Faculty
- US-02 View Faculty
- US-03 Update Faculty
- US-04 Remove Faculty
- US-05 Assign Faculty to Course

### Examination Management
- US-06 Create Examination
- US-07 Schedule Examination
- US-08 Assign Examination to Course
- US-09 Enter Student Marks
- US-10 View Examination Results

### Course Registration
- US-11 View Available Courses
- US-12 Register for Course
- US-13 Prevent Duplicate Registration
- US-14 Check Course Capacity
- US-15 Drop Course

## Compiler compatibility

The source is intentionally written in C++11 so it works with the GNU compiler supplied by MSYS2 UCRT64 without requiring C++17 or C++20.

The compiler/debugger paths used by the VS Code configuration are:

    C:/msys64/ucrt64/bin/g++.exe
    C:/msys64/ucrt64/bin/gdb.exe

## Build in the VS Code terminal

Run:

    C:/msys64/ucrt64/bin/g++.exe -std=c++11 -Wall -Wextra -Wpedantic main.cpp -o main.exe

Then:

    .\main.exe

## Build and run tests

    C:/msys64/ucrt64/bin/g++.exe -std=c++11 -Wall -Wextra -Wpedantic tests.cpp -o ums_tests.exe
    .\ums_tests.exe

Expected final test message:

    All 15 user-story test groups passed.

## VS Code

The repository contains:

- .vscode/tasks.json — builds with MSYS2 UCRT64 g++
- .vscode/launch.json — runs/debugs with MSYS2 UCRT64 gdb

Use Run UMS (MSYS2 UCRT64) from the Run and Debug panel.

## CMake

CMake is configured for C++11:

    cmake -S . -B build
    cmake --build build

## Data

The application creates and maintains:

    data/faculty.txt
    data/courses.txt
    data/exams.txt
    data/students.txt
    data/marks.txt
    data/registrations.txt

## Automated verification

GitHub Actions checks both:

1. Linux GCC with C++11
2. Windows MSYS2 UCRT64 GCC with C++11

Both build the application and the test suite with:

    -Wall -Wextra -Wpedantic

and then run all 15 user-story test groups plus persistence checks.
