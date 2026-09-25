#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Faculty {
    string id, name, department, email, designation;
    optional<string> courseId;
};

struct Course {
    string id, code, title, department;
    int capacity = 0;
    int enrolled = 0;
    optional<string> facultyId;
};

struct Exam {
    string id, title, courseId;
    int maxMarks = 0;
    string date, time;
};

struct Student {
    string id, name;
};

struct Mark {
    string examId, studentId, facultyId;
    int marks = 0;
};

struct Registration {
    string studentId, courseId;
};

struct Result {
    string examId, examTitle, courseCode, courseTitle, date, time;
    int marks = 0, maxMarks = 0;
};

class UniversityManagementSystem {
    unordered_map<string, Faculty> faculty;
    unordered_map<string, Course> courses;
    unordered_map<string, Exam> exams;
    unordered_map<string, Student> students;
    vector<Mark> marks;
    vector<Registration> registrations;
    string dir;

    static vector<string> split(const string& value, char delimiter = '|') {
        vector<string> parts;
        string part;
        stringstream stream(value);
        while (getline(stream, part, delimiter)) parts.push_back(part);
        return parts;
    }

    static string lower(string value) {
        transform(value.begin(), value.end(), value.begin(),
                  [](unsigned char c) { return static_cast<char>(tolower(c)); });
        return value;
    }

    static bool validDate(const string& date) {
        static const regex pattern(R"(^\d{4}-\d{2}-\d{2}$)");
        if (!regex_match(date, pattern)) return false;
        const int month = stoi(date.substr(5, 2));
        const int day = stoi(date.substr(8, 2));
        return month >= 1 && month <= 12 && day >= 1 && day <= 31;
    }

    static bool validTime(const string& time) {
        static const regex pattern(R"(^\d{2}:\d{2}$)");
        if (!regex_match(time, pattern)) return false;
        const int hour = stoi(time.substr(0, 2));
        const int minute = stoi(time.substr(3, 2));
        return hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59;
    }

    bool saveFaculty() const {
        ofstream file(dir + "/faculty.txt");
        if (!file) return false;
        for (const auto& entry : faculty) {
            const auto& value = entry.second;
            file << value.id << '|' << value.name << '|' << value.department << '|'
                 << value.email << '|' << value.designation << '|'
                 << (value.courseId ? *value.courseId : "") << '\n';
        }
        return true;
    }

    bool saveCourses() const {
        ofstream file(dir + "/courses.txt");
        if (!file) return false;
        for (const auto& entry : courses) {
            const auto& value = entry.second;
            file << value.id << '|' << value.code << '|' << value.title << '|'
                 << value.department << '|' << value.capacity << '|'
                 << value.enrolled << '|'
                 << (value.facultyId ? *value.facultyId : "") << '\n';
        }
        return true;
    }

    bool saveExams() const {
        ofstream file(dir + "/exams.txt");
        if (!file) return false;
        for (const auto& entry : exams) {
            const auto& value = entry.second;
            file << value.id << '|' << value.title << '|' << value.courseId << '|'
                 << value.maxMarks << '|' << value.date << '|' << value.time << '\n';
        }
        return true;
    }

    bool saveStudents() const {
        ofstream file(dir + "/students.txt");
        if (!file) return false;
        for (const auto& entry : students) {
            const auto& value = entry.second;
            file << value.id << '|' << value.name << '\n';
        }
        return true;
    }

    bool saveMarks() const {
        ofstream file(dir + "/marks.txt");
        if (!file) return false;
        for (const auto& value : marks) {
            file << value.examId << '|' << value.studentId << '|' << value.facultyId
                 << '|' << value.marks << '\n';
        }
        return true;
    }

    bool saveRegistrations() const {
        ofstream file(dir + "/registrations.txt");
        if (!file) return false;
        for (const auto& value : registrations) {
            file << value.studentId << '|' << value.courseId << '\n';
        }
        return true;
    }

    static bool validFaculty(const Faculty& value, string& error) {
        if (value.id.empty() || value.name.empty() || value.department.empty() ||
            value.email.empty() || value.designation.empty()) {
            error = "All faculty fields are required.";
            return false;
        }

        const size_t at = value.email.find('@');
        const size_t dot = value.email.find('.', at == string::npos ? 0 : at);
        if (at == string::npos || dot == string::npos || at == 0 || dot <= at + 1) {
            error = "Invalid email address.";
            return false;
        }
        return true;
    }

public:
    explicit UniversityManagementSystem(string dataDir = "data")
        : dir(move(dataDir)) {
        filesystem::create_directories(dir);
    }

    void seed() {
        if (courses.empty()) {
            courses["C001"] = {"C001", "CS101", "Programming Fundamentals",
                               "Computer Science", 40, 35, nullopt};
            courses["C002"] = {"C002", "DB201", "Database Systems",
                               "Computer Science", 30, 30, nullopt};
            courses["C003"] = {"C003", "SE210", "Software Engineering",
                               "Computer Science", 35, 21, nullopt};
            courses["C004"] = {"C004", "MTH101", "Calculus I",
                               "Mathematics", 50, 44, nullopt};
        }

        if (students.empty()) {
            students["S001"] = {"S001", "Ali Raza"};
            students["S002"] = {"S002", "Sara Khan"};
            students["S003"] = {"S003", "Hamza Ahmed"};
        }
    }

    bool load(string& error) {
        auto readFile = [&](const string& filename, const auto& loader) -> bool {
            ifstream file(dir + "/" + filename);
            if (!file) return true;

            string line;
            while (getline(file, line)) {
                if (line.empty()) continue;
                if (!loader(split(line))) {
                    error = "Malformed " + filename;
                    return false;
                }
            }
            return true;
        };

        faculty.clear();
        courses.clear();
        exams.clear();
        students.clear();
        marks.clear();
        registrations.clear();

        if (!readFile("courses.txt", [&](const vector<string>& p) {
                if (p.size() != 7) return false;
                courses[p[0]] = {p[0], p[1], p[2], p[3], stoi(p[4]), stoi(p[5]),
                                 p[6].empty() ? nullopt : optional<string>(p[6])};
                return true;
            })) return false;

        if (!readFile("students.txt", [&](const vector<string>& p) {
                if (p.size() != 2) return false;
                students[p[0]] = {p[0], p[1]};
                return true;
            })) return false;

        if (!readFile("faculty.txt", [&](const vector<string>& p) {
                if (p.size() != 6) return false;
                faculty[p[0]] = {p[0], p[1], p[2], p[3], p[4],
                                 p[5].empty() ? nullopt : optional<string>(p[5])};
                return true;
            })) return false;

        if (!readFile("exams.txt", [&](const vector<string>& p) {
                if (p.size() != 6 && p.size() != 4) return false;
                exams[p[0]] = {p[0], p[1], p[2], stoi(p[3]),
                               p.size() == 6 ? p[4] : "",
                               p.size() == 6 ? p[5] : ""};
                return true;
            })) return false;

        if (!readFile("marks.txt", [&](const vector<string>& p) {
                if (p.size() != 4) return false;
                marks.push_back({p[0], p[1], p[2], stoi(p[3])});
                return true;
            })) return false;

        if (!readFile("registrations.txt", [&](const vector<string>& p) {
                if (p.size() != 2) return false;
                registrations.push_back({p[0], p[1]});
                return true;
            })) return false;

        if (courses.empty() || students.empty()) {
            seed();
            save();
        }
        return true;
    }

    bool save() const {
        return saveFaculty() && saveCourses() && saveExams() &&
               saveStudents() && saveMarks() && saveRegistrations();
    }

    // US-01 — Add Faculty
    bool addFaculty(Faculty value, string& error) {
        if (!validFaculty(value, error)) return false;
        if (faculty.count(value.id)) {
            error = "Faculty ID already exists.";
            return false;
        }
        faculty[value.id] = move(value);
        return save();
    }

    // US-02 — View Faculty
    vector<Faculty> getFacultyList() const {
        vector<Faculty> result;
        for (const auto& entry : faculty) result.push_back(entry.second);
        sort(result.begin(), result.end(),
             [](const Faculty& a, const Faculty& b) { return a.id < b.id; });
        return result;
    }

    const Faculty* getFaculty(const string& id) const {
        const auto it = faculty.find(id);
        return it == faculty.end() ? nullptr : &it->second;
    }

    // US-03 — Update Faculty
    bool updateFaculty(const string& id, Faculty updated, string& error) {
        const auto it = faculty.find(id);
        if (it == faculty.end()) {
            error = "Faculty not found.";
            return false;
        }

        if (!validFaculty(updated, error)) return false;
        if (updated.id != id && faculty.count(updated.id)) {
            error = "Replacement faculty ID already exists.";
            return false;
        }

        updated.courseId = it->second.courseId;
        const string newId = updated.id;
        faculty.erase(it);
        faculty[newId] = move(updated);

        for (auto& entry : courses) {
            Course& course = entry.second;
            if (course.facultyId && *course.facultyId == id) {
                course.facultyId = newId;
            }
        }

        for (auto& mark : marks) {
            if (mark.facultyId == id) mark.facultyId = newId;
        }

        return save();
    }

    // US-04 — Remove Faculty
    bool removeFaculty(const string& id, string& error) {
        const auto it = faculty.find(id);
        if (it == faculty.end()) {
            error = "Faculty not found.";
            return false;
        }

        for (auto& entry : courses) {
            Course& course = entry.second;
            if (course.facultyId && *course.facultyId == id) {
                course.facultyId.reset();
            }
        }

        for (auto& mark : marks) {
            if (mark.facultyId == id) mark.facultyId.clear();
        }

        faculty.erase(it);
        return save();
    }

    // US-05 — Assign Faculty to Course
    bool assignFacultyToCourse(const string& facultyId,
                               const string& courseId,
                               string& error) {
        auto facultyIt = faculty.find(facultyId);
        const auto courseIt = courses.find(courseId);

        if (facultyIt == faculty.end()) {
            error = "Faculty not found.";
            return false;
        }
        if (courseIt == courses.end()) {
            error = "Course not found.";
            return false;
        }

        if (courseIt->second.facultyId &&
            *courseIt->second.facultyId != facultyId) {
            error = "Course already has another faculty member.";
            return false;
        }

        if (facultyIt->second.courseId &&
            *facultyIt->second.courseId != courseId) {
            courses[*facultyIt->second.courseId].facultyId.reset();
        }

        facultyIt->second.courseId = courseId;
        courseIt->second.facultyId = facultyId;
        return save();
    }

    // US-06 — Create Examination
    bool createExam(Exam exam, string& error) {
        if (exam.id.empty() || exam.title.empty()) {
            error = "Exam ID and title are required.";
            return false;
        }
        if (exam.maxMarks <= 0) {
            error = "Maximum marks must be positive.";
            return false;
        }
        if (exams.count(exam.id)) {
            error = "Exam ID already exists.";
            return false;
        }
        if (!exam.courseId.empty() && !courses.count(exam.courseId)) {
            error = "Course not found.";
            return false;
        }
        if ((!exam.date.empty() && !validDate(exam.date)) ||
            (!exam.time.empty() && !validTime(exam.time))) {
            error = "Invalid examination date or time.";
            return false;
        }

        exams[exam.id] = move(exam);
        return save();
    }

    // US-07 — Schedule Examination
    bool scheduleExam(const string& examId,
                      const string& date,
                      const string& time,
                      string& error) {
        const auto it = exams.find(examId);
        if (it == exams.end()) {
            error = "Examination not found.";
            return false;
        }
        if (!validDate(date)) {
            error = "Date must use YYYY-MM-DD.";
            return false;
        }
        if (!validTime(time)) {
            error = "Time must use HH:MM.";
            return false;
        }

        it->second.date = date;
        it->second.time = time;
        return save();
    }

    // US-08 — Assign Examination to Course
    bool assignExamToCourse(const string& examId,
                            const string& courseId,
                            string& error) {
        const auto examIt = exams.find(examId);
        if (examIt == exams.end()) {
            error = "Examination not found.";
            return false;
        }
        if (!courses.count(courseId)) {
            error = "Course not found.";
            return false;
        }

        examIt->second.courseId = courseId;
        return save();
    }

    // US-09 — Enter Student Marks
    bool enterMarks(const string& examId,
                    const string& studentId,
                    const string& facultyId,
                    int value,
                    string& error) {
        const auto examIt = exams.find(examId);
        const auto studentIt = students.find(studentId);
        const auto facultyIt = faculty.find(facultyId);

        if (examIt == exams.end()) {
            error = "Examination not found.";
            return false;
        }
        if (studentIt == students.end()) {
            error = "Student not found.";
            return false;
        }
        if (facultyIt == faculty.end()) {
            error = "Faculty not found.";
            return false;
        }
        if (examIt->second.courseId.empty()) {
            error = "Examination must be assigned to a course first.";
            return false;
        }
        if (!facultyIt->second.courseId ||
            *facultyIt->second.courseId != examIt->second.courseId) {
            error = "Faculty is not authorized for this exam course.";
            return false;
        }
        if (value < 0 || value > examIt->second.maxMarks) {
            error = "Marks are outside the allowed range.";
            return false;
        }

        const auto markIt = find_if(
            marks.begin(), marks.end(),
            [&](const Mark& mark) {
                return mark.examId == examId && mark.studentId == studentId;
            });

        if (markIt == marks.end()) {
            marks.push_back({examId, studentId, facultyId, value});
        } else {
            markIt->facultyId = facultyId;
            markIt->marks = value;
        }

        return saveMarks();
    }

    // US-10 — View Examination Results with student-level access control
    vector<Result> getResultsForStudent(const string& requesterId,
                                        const string& requestedStudentId,
                                        string& error) const {
        if (!students.count(requesterId) || !students.count(requestedStudentId)) {
            error = "Student not found.";
            return {};
        }

        if (requesterId != requestedStudentId) {
            error = "Access denied: a student can only view their own results.";
            return {};
        }

        vector<Result> result;
        for (const auto& mark : marks) {
            if (mark.studentId != requesterId) continue;

            const auto examIt = exams.find(mark.examId);
            if (examIt == exams.end() || examIt->second.courseId.empty()) continue;

            const auto courseIt = courses.find(examIt->second.courseId);
            if (courseIt == courses.end()) continue;

            result.push_back({
                examIt->second.id,
                examIt->second.title,
                courseIt->second.code,
                courseIt->second.title,
                examIt->second.date,
                examIt->second.time,
                mark.marks,
                examIt->second.maxMarks
            });
        }

        sort(result.begin(), result.end(),
             [](const Result& a, const Result& b) {
                 return a.examId < b.examId;
             });
        return result;
    }

    // US-11 — View Available Courses
    vector<Course> availableCourses(const string& queryText = "") const {
        vector<Course> result;
        const string query = lower(queryText);

        for (const auto& entry : courses) {
            const Course& course = entry.second;
            const bool matches =
                query.empty() ||
                lower(course.code).find(query) != string::npos ||
                lower(course.title).find(query) != string::npos ||
                lower(course.department).find(query) != string::npos;

            if (matches && course.enrolled < course.capacity) {
                result.push_back(course);
            }
        }

        sort(result.begin(), result.end(),
             [](const Course& a, const Course& b) {
                 return a.code < b.code;
             });
        return result;
    }

    // US-12 + US-13 — Register and prevent duplicates
    bool registerCourse(const string& studentId,
                        const string& courseId,
                        string& error) {
        if (!students.count(studentId)) {
            error = "Student not found.";
            return false;
        }

        const auto courseIt = courses.find(courseId);
        if (courseIt == courses.end()) {
            error = "Course not found.";
            return false;
        }

        if (hasRegistration(studentId, courseId)) {
            error = "Duplicate registration is not allowed.";
            return false;
        }

        if (courseIt->second.enrolled >= courseIt->second.capacity) {
            error = "Course is full.";
            return false;
        }

        registrations.push_back({studentId, courseId});
        courses[courseId].enrolled++;
        return save();
    }

    bool hasRegistration(const string& studentId,
                          const string& courseId) const {
        return any_of(
            registrations.begin(), registrations.end(),
            [&](const Registration& registration) {
                return registration.studentId == studentId &&
                       registration.courseId == courseId;
            });
    }

    vector<Course> getRegisteredCourses(const string& studentId,
                                        string& error) const {
        if (!students.count(studentId)) {
            error = "Student not found.";
            return {};
        }

        vector<Course> result;
        for (const auto& registration : registrations) {
            if (registration.studentId != studentId) continue;

            const auto courseIt = courses.find(registration.courseId);
            if (courseIt != courses.end()) result.push_back(courseIt->second);
        }

        sort(result.begin(), result.end(),
             [](const Course& a, const Course& b) {
                 return a.code < b.code;
             });
        return result;
    }

    // US-14 — Check Course Capacity
    bool capacity(const string& courseId,
                  int& remaining,
                  bool& full,
                  string& error) const {
        const auto it = courses.find(courseId);
        if (it == courses.end()) {
            error = "Course not found.";
            return false;
        }

        remaining = max(0, it->second.capacity - it->second.enrolled);
        full = it->second.enrolled >= it->second.capacity;
        return true;
    }

    // US-15 — Drop Course
    bool dropCourse(const string& studentId,
                    const string& courseId,
                    string& error) {
        if (!students.count(studentId)) {
            error = "Student not found.";
            return false;
        }

        const auto registrationIt = find_if(
            registrations.begin(), registrations.end(),
            [&](const Registration& registration) {
                return registration.studentId == studentId &&
                       registration.courseId == courseId;
            });

        if (registrationIt == registrations.end()) {
            error = "Registration not found.";
            return false;
        }

        auto courseIt = courses.find(courseId);
        if (courseIt != courses.end() && courseIt->second.enrolled > 0) {
            courseIt->second.enrolled--;
        }

        registrations.erase(registrationIt);
        return save();
    }

    vector<Exam> getExamList() const {
        vector<Exam> result;
        for (const auto& entry : exams) result.push_back(entry.second);
        sort(result.begin(), result.end(),
             [](const Exam& a, const Exam& b) { return a.id < b.id; });
        return result;
    }

    vector<Student> getStudentList() const {
        vector<Student> result;
        for (const auto& entry : students) result.push_back(entry.second);
        sort(result.begin(), result.end(),
             [](const Student& a, const Student& b) { return a.id < b.id; });
        return result;
    }

    vector<Mark> getMarks() const { return marks; }

    vector<Registration> getRegistrations() const { return registrations; }
};

static void header(const string& title) {
    cout << "\n========================================\n"
         << title << "\n"
         << "========================================\n";
}

static string lineInput(const string& prompt) {
    cout << prompt;
    string value;
    getline(cin, value);
    return value;
}

static int numberInput(const string& prompt) {
    while (true) {
        cout << prompt;
        int value;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid number.\n";
    }
}

static bool confirm(const string& prompt) {
    const string answer = lineInput(prompt + " (y/n): ");
    return !answer.empty() && (answer[0] == 'y' || answer[0] == 'Y');
}

static void facultyListUI(const UniversityManagementSystem& system) {
    header("FACULTY RECORDS");
    for (const auto& f : system.getFacultyList()) {
        cout << f.id << " | " << f.name << " | " << f.department
             << " | " << f.email << " | " << f.designation;
        if (f.courseId) cout << " | Course: " << *f.courseId;
        cout << '\n';
    }
}

static void addFacultyUI(UniversityManagementSystem& system) {
    Faculty value{
        lineInput("Faculty ID: "),
        lineInput("Name: "),
        lineInput("Department: "),
        lineInput("Email: "),
        lineInput("Designation: "),
        nullopt
    };
    string error;
    cout << (system.addFaculty(value, error)
                 ? "Faculty added successfully.\n"
                 : "[ERROR] " + error + "\n");
}

static void updateFacultyUI(UniversityManagementSystem& system) {
    const string id = lineInput("Faculty ID: ");
    const Faculty* old = system.getFaculty(id);
    if (!old) {
        cout << "[ERROR] Faculty not found.\n";
        return;
    }

    Faculty updated = *old;
    string value = lineInput("Name [" + updated.name + "]: ");
    if (!value.empty()) updated.name = value;
    value = lineInput("Department [" + updated.department + "]: ");
    if (!value.empty()) updated.department = value;
    value = lineInput("Email [" + updated.email + "]: ");
    if (!value.empty()) updated.email = value;
    value = lineInput("Designation [" + updated.designation + "]: ");
    if (!value.empty()) updated.designation = value;

    string error;
    cout << (system.updateFaculty(id, updated, error)
                 ? "Faculty updated successfully.\n"
                 : "[ERROR] " + error + "\n");
}

static void removeFacultyUI(UniversityManagementSystem& system) {
    const string id = lineInput("Faculty ID to remove: ");
    if (!system.getFaculty(id)) {
        cout << "[ERROR] Faculty not found.\n";
        return;
    }
    if (!confirm("Confirm faculty removal")) {
        cout << "Removal cancelled.\n";
        return;
    }

    string error;
    cout << (system.removeFaculty(id, error)
                 ? "Faculty removed successfully.\n"
                 : "[ERROR] " + error + "\n");
}

static void assignFacultyUI(UniversityManagementSystem& system) {
    const string facultyId = lineInput("Faculty ID: ");
    const string courseId = lineInput("Course ID: ");
    string error;
    cout << (system.assignFacultyToCourse(facultyId, courseId, error)
                 ? "Faculty-course assignment saved.\n"
                 : "[ERROR] " + error + "\n");
}

static void createExamUI(UniversityManagementSystem& system) {
    Exam exam{
        lineInput("Exam ID: "),
        lineInput("Exam title: "),
        "",
        numberInput("Maximum marks: "),
        "",
        ""
    };
    string error;
    cout << (system.createExam(exam, error)
                 ? "Examination created.\n"
                 : "[ERROR] " + error + "\n");
}

static void scheduleExamUI(UniversityManagementSystem& system) {
    const string examId = lineInput("Exam ID: ");
    const string date = lineInput("Date (YYYY-MM-DD): ");
    const string time = lineInput("Time (HH:MM): ");
    string error;
    cout << (system.scheduleExam(examId, date, time, error)
                 ? "Examination schedule saved.\n"
                 : "[ERROR] " + error + "\n");
}

static void assignExamUI(UniversityManagementSystem& system) {
    const string examId = lineInput("Exam ID: ");
    const string courseId = lineInput("Course ID: ");
    string error;
    cout << (system.assignExamToCourse(examId, courseId, error)
                 ? "Examination-course association saved.\n"
                 : "[ERROR] " + error + "\n");
}

static void marksUI(UniversityManagementSystem& system) {
    header("ENTER STUDENT MARKS");
    for (const auto& exam : system.getExamList()) {
        cout << exam.id << " | " << exam.title
             << " | Course: " << (exam.courseId.empty() ? "Unassigned" : exam.courseId)
             << " | Max: " << exam.maxMarks
             << (exam.date.empty() ? "" : " | " + exam.date + " " + exam.time)
             << '\n';
    }
    for (const auto& student : system.getStudentList()) {
        cout << student.id << " | " << student.name << '\n';
    }

    const string examId = lineInput("Exam ID: ");
    const string studentId = lineInput("Student ID: ");
    const string facultyId = lineInput("Faculty ID: ");
    const int value = numberInput("Marks: ");

    string error;
    cout << (system.enterMarks(examId, studentId, facultyId, value, error)
                 ? "Marks saved.\n"
                 : "[ERROR] " + error + "\n");
}

static void resultsUI(const UniversityManagementSystem& system) {
    const string requesterId = lineInput("Your student ID: ");
    const string requestedId = lineInput("Student ID whose results are requested: ");
    string error;
    const auto results =
        system.getResultsForStudent(requesterId, requestedId, error);

    if (!error.empty()) {
        cout << "[ERROR] " << error << '\n';
        return;
    }

    header("EXAMINATION RESULTS");
    if (results.empty()) {
        cout << "No examination results found.\n";
        return;
    }

    for (const auto& result : results) {
        cout << result.examId << " | " << result.examTitle
             << " | " << result.courseCode << " | " << result.courseTitle
             << " | " << result.marks << "/" << result.maxMarks;
        if (!result.date.empty()) cout << " | " << result.date << " " << result.time;
        cout << '\n';
    }
}

static void coursesUI(const UniversityManagementSystem& system) {
    header("AVAILABLE COURSES");
    const auto available =
        system.availableCourses(lineInput("Search (Enter for all): "));
    if (available.empty()) {
        cout << "No available courses found.\n";
        return;
    }

    for (const auto& course : available) {
        cout << course.id << " | " << course.code
             << " | " << course.title << " | " << course.department
             << " | " << course.enrolled << "/" << course.capacity << '\n';
    }
}

static void registerCourseUI(UniversityManagementSystem& system) {
    const string studentId = lineInput("Student ID: ");
    const string courseId = lineInput("Course ID: ");
    string error;
    cout << (system.registerCourse(studentId, courseId, error)
                 ? "Course registration successful.\n"
                 : "[ERROR] " + error + "\n");
}

static void duplicateCheckUI(const UniversityManagementSystem& system) {
    const string studentId = lineInput("Student ID: ");
    const string courseId = lineInput("Course ID: ");
    cout << (system.hasRegistration(studentId, courseId)
                 ? "Duplicate registration detected; new registration is blocked.\n"
                 : "No existing registration found.\n");
}

static void capacityUI(const UniversityManagementSystem& system) {
    const string courseId = lineInput("Course ID: ");
    int remaining = 0;
    bool full = false;
    string error;

    if (!system.capacity(courseId, remaining, full, error)) {
        cout << "[ERROR] " << error << '\n';
        return;
    }

    cout << "Remaining seats: " << remaining << '\n'
         << (full ? "Course is FULL; registration must be blocked.\n"
                  : "Capacity available.\n");
}

static void registeredCoursesUI(const UniversityManagementSystem& system) {
    const string studentId = lineInput("Student ID: ");
    string error;
    const auto result = system.getRegisteredCourses(studentId, error);

    if (!error.empty()) {
        cout << "[ERROR] " << error << '\n';
        return;
    }

    header("REGISTERED COURSES");
    if (result.empty()) {
        cout << "No courses registered.\n";
        return;
    }

    for (const auto& course : result) {
        cout << course.id << " | " << course.code
             << " | " << course.title << '\n';
    }
}

static void dropCourseUI(UniversityManagementSystem& system) {
    const string studentId = lineInput("Student ID: ");
    const string courseId = lineInput("Course ID: ");

    if (!system.hasRegistration(studentId, courseId)) {
        cout << "[ERROR] Registration not found.\n";
        return;
    }

    if (!confirm("Confirm course drop")) {
        cout << "Course drop cancelled.\n";
        return;
    }

    string error;
    cout << (system.dropCourse(studentId, courseId, error)
                 ? "Course dropped successfully.\n"
                 : "[ERROR] " + error + "\n");
}

int main() {
    UniversityManagementSystem system("data");
    string error;
    if (!system.load(error)) {
        cerr << "[ERROR] " << error << '\n';
        return 1;
    }

    while (true) {
        header("UNIVERSITY MANAGEMENT SYSTEM - COMPLETE C++ PROJECT");

        cout << "1.  Add Faculty\n"
             << "2.  View Faculty\n"
             << "3.  Update Faculty\n"
             << "4.  Remove Faculty\n"
             << "5.  Assign Faculty to Course\n"
             << "6.  Create Examination\n"
             << "7.  Schedule Examination\n"
             << "8.  Assign Examination to Course\n"
             << "9.  Enter Student Marks\n"
             << "10. View Examination Results\n"
             << "11. View Available Courses\n"
             << "12. Register for Course\n"
             << "13. Check Duplicate Registration\n"
             << "14. Check Course Capacity\n"
             << "15. View Registered Courses\n"
             << "16. Drop Course\n"
             << "17. Exit\n";

        switch (numberInput("Choose: ")) {
            case 1: addFacultyUI(system); break;
            case 2: facultyListUI(system); break;
            case 3: updateFacultyUI(system); break;
            case 4: removeFacultyUI(system); break;
            case 5: assignFacultyUI(system); break;
            case 6: createExamUI(system); break;
            case 7: scheduleExamUI(system); break;
            case 8: assignExamUI(system); break;
            case 9: marksUI(system); break;
            case 10: resultsUI(system); break;
            case 11: coursesUI(system); break;
            case 12: registerCourseUI(system); break;
            case 13: duplicateCheckUI(system); break;
            case 14: capacityUI(system); break;
            case 15: registeredCoursesUI(system); break;
            case 16: dropCourseUI(system); break;
            case 17: return 0;
            default: cout << "Invalid choice.\n"; break;
        }
    }
}
