#include <cassert>
#include <cstdio>
#include <iostream>
#define main ums_app_main
#include "main.cpp"
#undef main
int main(){
 const string dir="test_data";filesystem::remove_all(dir);UniversityManagementSystem u(dir);string e;assert(u.load(e));
 assert(u.addFaculty({"F001","Dr. Ahmed","Computer Science","ahmed@ums.edu","Professor",nullopt},e));assert(u.getFaculty("F001"));assert(!u.addFaculty({"F001","Duplicate","CS","duplicate@ums.edu","Lecturer",nullopt},e));assert(!u.addFaculty({"F002","","CS","bad@ums.edu","Lecturer",nullopt},e));
 assert(u.updateFaculty("F001",{"F001","Dr. Ahmed Updated","Computer Science","updated@ums.edu","Senior Professor",nullopt},e));assert(u.getFaculty("F001")->name=="Dr. Ahmed Updated");assert(u.assignFacultyToCourse("F001","C001",e));
 assert(u.createExam({"EX001","Midterm","C001",100,nullopt},e));assert(!u.createExam({"EX002","Bad","NO_COURSE",100,nullopt},e));assert(u.scheduleExam("EX001","2026-10-15 10:30",e));assert(!u.scheduleExam("EX001","2026-99-99 25:61",e));
 assert(u.enterMarks("EX001","S001","F001",85,e));assert(!u.enterMarks("EX001","S001","F001",101,e));auto rr=u.resultsForStudent("S001",e);assert(rr.size()==1&&rr[0].marks==85&&rr[0].maxMarks==100);
 auto found=u.availableCourses("Software");assert(found.size()==1&&found[0].id=="C003");int rem;bool full;assert(u.capacity("C001",rem,full,e)&&rem==5&&!full);assert(u.capacity("C002",rem,full,e)&&rem==0&&full);
 assert(u.registerCourse("S001","C001",e));assert(!u.registerCourse("S001","C001",e));assert(!u.registerCourse("S001","C002",e));assert(u.registeredCourses("S001",e).size()==1);assert(u.dropCourse("S001","C001",e));assert(u.registeredCourses("S001",e).empty());assert(!u.dropCourse("S001","C001",e));
 assert(u.removeFaculty("F001",e));assert(!u.getFaculty("F001"));assert(!u.assignFacultyToCourse("F001","C001",e));filesystem::remove_all(dir);cout<<"All assigned C++ tests passed.\n";
}
