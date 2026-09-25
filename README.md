# University Management System — C++

Implementation of the assigned **University Management System (UMS)** Jira scope in C++20.

## Implemented UMS Features

### Faculty Management
- Add Faculty
- Update Faculty
- Remove Faculty
- View Faculty Records
- Assign Faculty to Course
- Faculty/course assignment validation

### Examination Management
- Create Examination
- Schedule Examination
- Enter Student Marks
- View Student Examination Results
- Exam/course/faculty validation
- Marks range validation

### Course Registration
- View Available Courses
- Search available courses
- Check Course Capacity
- Register for a Course
- Prevent duplicate registration
- Prevent registration in a full course
- Drop a Registered Course

### Data Persistence
The application stores UMS records in text files under the `data/` directory:
- `faculty.txt`
- `courses.txt`
- `students.txt`
- `exams.txt`
- `marks.txt`
- `registrations.txt`

## Assigned Jira Scope

The implementation covers the assigned UMS stories/tasks, including:
- US-01 — Add Faculty
- US-03 — Update Faculty
- US-05 — Assign Faculty to Course
- US-06 — Create Examination
- US-09 — Enter Student Marks
- US-11 — View Available Courses
- US-14 — Check Course Capacity
- Faculty removal
- Examination scheduling and results
- Course registration and course drop

## Build

Requires a **C++20** compiler.

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic main.cpp -o ums
./ums
```

## Run Tests

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic tests.cpp -o ums_tests
./ums_tests
```

The test suite covers faculty management, faculty-course assignment, examination creation and scheduling, marks entry, results, course search/capacity, registration, duplicate-registration prevention, course dropping, and validation/error cases.

## Agile / Development Notes

The UMS was developed incrementally using Agile practices. Features were implemented as small working increments, tested continuously, and integrated through GitHub.

The project applies XP-oriented practices such as:
- Automated testing
- Refactoring
- Continuous integration
- Frequent feedback and validation

## Repository Workflow

Feature work is developed on dedicated branches and reviewed through pull requests before merging into `main`.

Current implementation branch:

`wardah/complete-assigned-jira-tasks`
