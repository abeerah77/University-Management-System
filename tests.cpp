#include <cassert>
#include <filesystem>
#include <iostream>

#define main ums_app_main
#include "main.cpp"
#undef main

int main() {
    const std::string dir = "test_data";
    std::filesystem::remove_all(dir);

    UniversityManagementSystem system(dir);
    std::string error;
    assert(system.load(error));

    // US-01 — Add Faculty
    assert(system.addFaculty(
        {"F001", "Dr. Ahmed", "Computer Science",
         "ahmed@ums.edu", "Professor", std::nullopt}, error));
    assert(system.getFaculty("F001") != nullptr);
    assert(!system.addFaculty(
        {"F001", "Duplicate", "CS", "duplicate@ums.edu",
         "Lecturer", std::nullopt}, error));
    assert(!system.addFaculty(
        {"F002", "", "CS", "bad@ums.edu",
         "Lecturer", std::nullopt}, error));

    // US-02 — View Faculty
    assert(system.getFacultyList().size() == 1);
    assert(system.getFacultyList().front().name == "Dr. Ahmed");

    // US-03 — Update Faculty
    assert(system.updateFaculty(
        "F001",
        {"F001", "Dr. Ahmed Updated", "Computer Science",
         "updated@ums.edu", "Senior Professor", std::nullopt},
        error));
    assert(system.getFaculty("F001")->name == "Dr. Ahmed Updated");

    // US-05 — Assign Faculty to Course
    assert(system.assignFacultyToCourse("F001", "C001", error));
    assert(system.getFaculty("F001")->courseId.value() == "C001");

    // US-04 — Remove Faculty
    assert(system.removeFaculty("F001", error));
    assert(system.getFaculty("F001") == nullptr);
    assert(!system.assignFacultyToCourse("F001", "C001", error));

    // Prepare an authorized faculty member for examination workflows.
    assert(system.addFaculty(
        {"F002", "Dr. Sara", "Computer Science",
         "sara@ums.edu", "Professor", std::nullopt}, error));
    assert(system.assignFacultyToCourse("F002", "C001", error));

    // US-06 — Create Examination
    assert(system.createExam(
        {"EX001", "Midterm", "", 100, "", ""}, error));
    assert(!system.createExam(
        {"EX001", "Duplicate", "", 100, "", ""}, error));

    // US-07 — Schedule Examination
    assert(system.scheduleExam("EX001", "2026-10-15", "09:30", error));
    assert(!system.scheduleExam("EX001", "15-10-2026", "09:30", error));

    // US-08 — Assign Examination to Course
    assert(system.assignExamToCourse("EX001", "C001", error));
    assert(!system.assignExamToCourse("EX001", "NO_COURSE", error));

    // US-09 — Enter Student Marks
    assert(system.enterMarks("EX001", "S001", "F002", 85, error));
    assert(system.getMarks().size() == 1);
    assert(!system.enterMarks("EX001", "S001", "F002", 101, error));

    // US-10 — View Examination Results / student access control
    auto results = system.getResultsForStudent("S001", "S001", error);
    assert(error.empty());
    assert(results.size() == 1);
    assert(results.front().marks == 85);
    assert(results.front().maxMarks == 100);

    auto denied = system.getResultsForStudent("S001", "S002", error);
    assert(denied.empty());
    assert(!error.empty());

    // US-11 — View Available Courses
    error.clear();
    assert(!system.availableCourses().empty());
    auto found = system.availableCourses("Software");
    assert(found.size() == 1 && found.front().id == "C003");

    // US-12 — Register for Course
    assert(system.registerCourse("S001", "C003", error));
    assert(system.hasRegistration("S001", "C003"));

    // US-13 — Prevent Duplicate Registration
    assert(!system.registerCourse("S001", "C003", error));
    assert(system.hasRegistration("S001", "C003"));

    // US-14 — Capacity
    int remaining = 0;
    bool full = false;
    assert(system.capacity("C002", remaining, full, error));
    assert(remaining == 0 && full);
    assert(!system.registerCourse("S001", "C002", error));

    // US-15 — Drop Course
    assert(system.dropCourse("S001", "C003", error));
    assert(!system.hasRegistration("S001", "C003"));
    assert(!system.dropCourse("S001", "C003", error));

    // Persistence
    assert(system.save());
    UniversityManagementSystem reloaded(dir);
    assert(reloaded.load(error));
    assert(reloaded.getFaculty("F002") != nullptr);
    assert(reloaded.getExamList().size() == 1);
    assert(reloaded.getMarks().size() == 1);
    assert(!reloaded.hasRegistration("S001", "C003"));

    std::filesystem::remove_all(dir);
    std::cout << "All 15 user-story test groups passed.\n";
    return 0;
}
