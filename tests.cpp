#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#define main ums_app_main
#include "main.cpp"
#undef main

static void removeTestData() {
    std::remove("test_data/faculty.txt");
    std::remove("test_data/courses.txt");
    std::remove("test_data/exams.txt");
    std::remove("test_data/students.txt");
    std::remove("test_data/marks.txt");
    std::remove("test_data/registrations.txt");
}

int main() {
    UniversityManagementSystem system("test_data");
    std::string error;

    removeTestData();
    assert(system.load(error));

    // US-01 — Add Faculty
    assert(system.addFaculty(
        Faculty{"F001", "Dr. Ahmed", "Computer Science",
                "ahmed@ums.edu", "Professor", ""},
        error));

    assert(system.getFaculty("F001") != NULL);

    assert(!system.addFaculty(
        Faculty{"F001", "Duplicate", "CS",
                "duplicate@ums.edu", "Lecturer", ""},
        error));

    assert(!system.addFaculty(
        Faculty{"F002", "", "CS",
                "bad@ums.edu", "Lecturer", ""},
        error));

    // US-02 — View Faculty
    assert(system.getFacultyList().size() == 1);
    assert(system.getFacultyList()[0].name == "Dr. Ahmed");

    // US-03 — Update Faculty
    assert(system.updateFaculty(
        "F001",
        Faculty{"F001", "Dr. Ahmed Updated", "Computer Science",
                "updated@ums.edu", "Senior Professor", ""},
        error));

    assert(system.getFaculty("F001")->name == "Dr. Ahmed Updated");

    // US-05 — Assign Faculty to Course
    assert(system.assignFacultyToCourse("F001", "C001", error));
    assert(system.getFaculty("F001")->courseId == "C001");

    // US-04 — Remove Faculty
    assert(system.removeFaculty("F001", error));
    assert(system.getFaculty("F001") == NULL);
    assert(!system.assignFacultyToCourse("F001", "C001", error));

    // Authorized faculty for examination workflows.
    assert(system.addFaculty(
        Faculty{"F002", "Dr. Sara", "Computer Science",
                "sara@ums.edu", "Professor", ""},
        error));

    assert(system.assignFacultyToCourse("F002", "C001", error));

    // US-06 — Create Examination
    assert(system.createExam(
        Exam{"EX001", "Midterm", "", 100, "", ""},
        error));

    assert(!system.createExam(
        Exam{"EX001", "Duplicate", "", 100, "", ""},
        error));

    // US-07 — Schedule Examination
    assert(system.scheduleExam(
        "EX001", "2026-10-15", "09:30", error));

    assert(!system.scheduleExam(
        "EX001", "15-10-2026", "09:30", error));

    // US-08 — Assign Examination to Course
    assert(system.assignExamToCourse(
        "EX001", "C001", error));

    assert(!system.assignExamToCourse(
        "EX001", "NO_COURSE", error));

    // US-09 — Enter Student Marks
    assert(system.enterMarks(
        "EX001", "S001", "F002", 85, error));

    assert(system.getMarks().size() == 1);

    assert(!system.enterMarks(
        "EX001", "S001", "F002", 101, error));

    // US-10 — View results with student access control
    error.clear();

    std::vector<Result> results =
        system.getResultsForStudent("S001", "S001", error);

    assert(error.empty());
    assert(results.size() == 1);
    assert(results[0].marks == 85);
    assert(results[0].maxMarks == 100);

    std::vector<Result> denied =
        system.getResultsForStudent("S001", "S002", error);

    assert(denied.empty());
    assert(!error.empty());

    // US-11 — View Available Courses
    error.clear();

    assert(!system.availableCourses().empty());

    std::vector<Course> found =
        system.availableCourses("Software");

    assert(found.size() == 1);
    assert(found[0].id == "C003");

    // US-12 — Register for Course
    assert(system.registerCourse(
        "S001", "C003", error));

    assert(system.hasRegistration(
        "S001", "C003"));

    // US-13 — Prevent Duplicate Registration
    assert(!system.registerCourse(
        "S001", "C003", error));

    assert(system.hasRegistration(
        "S001", "C003"));

    // US-14 — Course Capacity
    int remaining = 0;
    bool full = false;

    assert(system.capacity(
        "C002", remaining, full, error));

    assert(remaining == 0);
    assert(full);

    assert(!system.registerCourse(
        "S001", "C002", error));

    // US-15 — Drop Course
    assert(system.dropCourse(
        "S001", "C003", error));

    assert(!system.hasRegistration(
        "S001", "C003"));

    assert(!system.dropCourse(
        "S001", "C003", error));

    // Persistence
    assert(system.save());

    UniversityManagementSystem reloaded("test_data");

    assert(reloaded.load(error));
    assert(reloaded.getFaculty("F002") != NULL);
    assert(reloaded.getExamList().size() == 1);
    assert(reloaded.getMarks().size() == 1);
    assert(!reloaded.hasRegistration(
        "S001", "C003"));

    removeTestData();

    std::cout
        << "All 15 user-story test groups passed.\n";

    return 0;
}
