#include <cassert>
#include <filesystem>
#include <iostream>

#define main ums_app_main
#include "main.cpp"
#undef main

int main() {
    const std::string dir = "test_data";
    std::filesystem::remove_all(dir);

    UniversityManagementSystem u(dir);
    std::string error;
    assert(u.load(error));

    // US-01 Add Faculty
    assert(u.addFaculty({"F001","Dr. Ahmed","Computer Science","ahmed@ums.edu","Professor",std::nullopt}, error));
    assert(u.getFaculty("F001") != nullptr);
    assert(!u.addFaculty({"F001","Duplicate","CS","duplicate@ums.edu","Lecturer",std::nullopt}, error));
    assert(!u.addFaculty({"F002","","CS","bad@ums.edu","Lecturer",std::nullopt}, error));

    // US-03 Update Faculty
    assert(u.updateFaculty("F001", {"F001","Dr. Ahmed Updated","Computer Science","updated@ums.edu","Senior Professor",std::nullopt}, error));
    assert(u.getFaculty("F001")->name == "Dr. Ahmed Updated");

    // US-05 Assign Faculty to Course
    assert(u.assignFacultyToCourse("F001","C001",error));
    assert(u.getFaculty("F001")->courseId.value() == "C001");

    // US-06 Create Examination
    assert(u.createExam({"EX001","Midterm","C001",100},error));
    assert(!u.createExam({"EX002","Bad","NO_COURSE",100},error));
    assert(!u.createExam({"EX001","Duplicate","C001",100},error));

    // US-09 Enter Student Marks
    assert(u.enterMarks("EX001","S001","F001",85,error));
    assert(u.getMarks().size()==1);
    assert(!u.enterMarks("EX001","S001","F001",101,error));

    // US-11 View Available Courses
    assert(!u.availableCourses().empty());
    auto found=u.availableCourses("Software");
    assert(found.size()==1 && found.front().id=="C003");

    // US-14 Check Course Capacity
    int remaining=0; bool full=false;
    assert(u.capacity("C001",remaining,full,error));
    assert(remaining==5 && !full);
    assert(u.capacity("C002",remaining,full,error));
    assert(remaining==0 && full);

    std::filesystem::remove_all(dir);
    std::cout << "All assigned C++ tests passed.\n";
    return 0;
}
