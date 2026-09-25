#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

using namespace std;

struct Faculty {
    string id;
    string name;
    string department;
    string email;
    string designation;
    string courseId;
};

struct Course {
    string id;
    string code;
    string title;
    string department;
    int capacity;
    int enrolled;
    string facultyId;
};

struct Exam {
    string id;
    string title;
    string courseId;
    int maxMarks;
    string date;
    string time;
};

struct Student {
    string id;
    string name;
};

struct Mark {
    string examId;
    string studentId;
    string facultyId;
    int marks;
};

struct Registration {
    string studentId;
    string courseId;
};

struct Result {
    string examId;
    string examTitle;
    string courseCode;
    string courseTitle;
    string date;
    string time;
    int marks;
    int maxMarks;
};

static void ensureDirectory(const string& path) {
#ifdef _WIN32
    _mkdir(path.c_str());
#else
    mkdir(path.c_str(), 0755);
#endif
}

static vector<string> split(const string& value, char delimiter = '|') {
    vector<string> parts;
    string part;
    stringstream stream(value);

    while (getline(stream, part, delimiter)) {
        parts.push_back(part);
    }

    if (!value.empty() && value[value.size() - 1] == delimiter) {
        parts.push_back("");
    }

    return parts;
}

static string lowerText(string value) {
    transform(value.begin(), value.end(), value.begin(),
              [](char c) {
                  return static_cast<char>(
                      tolower(static_cast<unsigned char>(c)));
              });
    return value;
}

static bool isDigits(const string& value, size_t start, size_t count) {
    if (start + count > value.size()) {
        return false;
    }

    for (size_t i = start; i < start + count; ++i) {
        if (!isdigit(static_cast<unsigned char>(value[i]))) {
            return false;
        }
    }

    return true;
}

static bool validDate(const string& date) {
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') {
        return false;
    }

    if (!isDigits(date, 0, 4) ||
        !isDigits(date, 5, 2) ||
        !isDigits(date, 8, 2)) {
        return false;
    }

    const int month = stoi(date.substr(5, 2));
    const int day = stoi(date.substr(8, 2));

    return month >= 1 && month <= 12 &&
           day >= 1 && day <= 31;
}

static bool validTime(const string& time) {
    if (time.size() != 5 || time[2] != ':') {
        return false;
    }

    if (!isDigits(time, 0, 2) ||
        !isDigits(time, 3, 2)) {
        return false;
    }

    const int hour = stoi(time.substr(0, 2));
    const int minute = stoi(time.substr(3, 2));

    return hour >= 0 && hour <= 23 &&
           minute >= 0 && minute <= 59;
}

class UniversityManagementSystem {
private:
    map<string, Faculty> faculty;
    map<string, Course> courses;
    map<string, Exam> exams;
    map<string, Student> students;
    vector<Mark> marks;
    vector<Registration> registrations;
    string dir;

    bool saveFaculty() const {
        ofstream file((dir + "/faculty.txt").c_str());
        if (!file) return false;

        for (map<string, Faculty>::const_iterator it = faculty.begin();
             it != faculty.end(); ++it) {
            const Faculty& value = it->second;
            file << value.id << '|'
                 << value.name << '|'
                 << value.department << '|'
                 << value.email << '|'
                 << value.designation << '|'
                 << value.courseId << '\n';
        }

        return true;
    }

    bool saveCourses() const {
        ofstream file((dir + "/courses.txt").c_str());
        if (!file) return false;

        for (map<string, Course>::const_iterator it = courses.begin();
             it != courses.end(); ++it) {
            const Course& value = it->second;
            file << value.id << '|'
                 << value.code << '|'
                 << value.title << '|'
                 << value.department << '|'
                 << value.capacity << '|'
                 << value.enrolled << '|'
                 << value.facultyId << '\n';
        }

        return true;
    }

    bool saveExams() const {
        ofstream file((dir + "/exams.txt").c_str());
        if (!file) return false;

        for (map<string, Exam>::const_iterator it = exams.begin();
             it != exams.end(); ++it) {
            const Exam& value = it->second;
            file << value.id << '|'
                 << value.title << '|'
                 << value.courseId << '|'
                 << value.maxMarks << '|'
                 << value.date << '|'
                 << value.time << '\n';
        }

        return true;
    }

    bool saveStudents() const {
        ofstream file((dir + "/students.txt").c_str());
        if (!file) return false;

        for (map<string, Student>::const_iterator it = students.begin();
             it != students.end(); ++it) {
            const Student& value = it->second;
            file << value.id << '|' << value.name << '\n';
        }

        return true;
    }

    bool saveMarks() const {
        ofstream file((dir + "/marks.txt").c_str());
        if (!file) return false;

        for (size_t i = 0; i < marks.size(); ++i) {
            file << marks[i].examId << '|'
                 << marks[i].studentId << '|'
                 << marks[i].facultyId << '|'
                 << marks[i].marks << '\n';
        }

        return true;
    }

    bool saveRegistrations() const {
        ofstream file((dir + "/registrations.txt").c_str());
        if (!file) return false;

        for (size_t i = 0; i < registrations.size(); ++i) {
            file << registrations[i].studentId << '|'
                 << registrations[i].courseId << '\n';
        }

        return true;
    }

    static bool validFaculty(const Faculty& value, string& error) {
        if (value.id.empty() ||
            value.name.empty() ||
            value.department.empty() ||
            value.email.empty() ||
            value.designation.empty()) {
            error = "All faculty fields are required.";
            return false;
        }

        const size_t at = value.email.find('@');
        const size_t dot =
            value.email.find('.', at == string::npos ? 0 : at);

        if (at == string::npos ||
            dot == string::npos ||
            at == 0 ||
            dot <= at + 1) {
            error = "Invalid email address.";
            return false;
        }

        return true;
    }

    void seed() {
        if (courses.empty()) {
            courses["C001"] = Course{
                "C001", "CS101", "Programming Fundamentals",
                "Computer Science", 40, 35, ""
            };

            courses["C002"] = Course{
                "C002", "DB201", "Database Systems",
                "Computer Science", 30, 30, ""
            };

            courses["C003"] = Course{
                "C003", "SE210", "Software Engineering",
                "Computer Science", 35, 21, ""
            };

            courses["C004"] = Course{
                "C004", "MTH101", "Calculus I",
                "Mathematics", 50, 44, ""
            };
        }

        if (students.empty()) {
            students["S001"] = Student{"S001", "Ali Raza"};
            students["S002"] = Student{"S002", "Sara Khan"};
            students["S003"] = Student{"S003", "Hamza Ahmed"};
        }
    }

public:
    explicit UniversityManagementSystem(const string& dataDir = "data")
        : dir(dataDir) {
        ensureDirectory(dir);
    }

    bool load(string& error) {
        faculty.clear();
        courses.clear();
        exams.clear();
        students.clear();
        marks.clear();
        registrations.clear();

        {
            ifstream file((dir + "/courses.txt").c_str());

            if (file) {
                string line;

                while (getline(file, line)) {
                    if (line.empty()) continue;

                    vector<string> p = split(line);
                    if (p.size() != 7) {
                        error = "Malformed courses.txt.";
                        return false;
                    }

                    try {
                        courses[p[0]] = Course{
                            p[0], p[1], p[2], p[3],
                            stoi(p[4]), stoi(p[5]), p[6]
                        };
                    } catch (...) {
                        error = "Invalid numeric value in courses.txt.";
                        return false;
                    }
                }
            }
        }

        {
            ifstream file((dir + "/students.txt").c_str());

            if (file) {
                string line;

                while (getline(file, line)) {
                    if (line.empty()) continue;

                    vector<string> p = split(line);
                    if (p.size() != 2) {
                        error = "Malformed students.txt.";
                        return false;
                    }

                    students[p[0]] = Student{p[0], p[1]};
                }
            }
        }

        {
            ifstream file((dir + "/faculty.txt").c_str());

            if (file) {
                string line;

                while (getline(file, line)) {
                    if (line.empty()) continue;

                    vector<string> p = split(line);
                    if (p.size() != 6) {
                        error = "Malformed faculty.txt.";
                        return false;
                    }

                    faculty[p[0]] = Faculty{
                        p[0], p[1], p[2], p[3], p[4], p[5]
                    };
                }
            }
        }

        {
            ifstream file((dir + "/exams.txt").c_str());

            if (file) {
                string line;

                while (getline(file, line)) {
                    if (line.empty()) continue;

                    vector<string> p = split(line);

                    if (p.size() != 6 && p.size() != 4) {
                        error = "Malformed exams.txt.";
                        return false;
                    }

                    try {
                        exams[p[0]] = Exam{
                            p[0],
                            p[1],
                            p[2],
                            stoi(p[3]),
                            p.size() == 6 ? p[4] : "",
                            p.size() == 6 ? p[5] : ""
                        };
                    } catch (...) {
                        error = "Invalid numeric value in exams.txt.";
                        return false;
                    }
                }
            }
        }

        {
            ifstream file((dir + "/marks.txt").c_str());

            if (file) {
                string line;

                while (getline(file, line)) {
                    if (line.empty()) continue;

                    vector<string> p = split(line);
                    if (p.size() != 4) {
                        error = "Malformed marks.txt.";
                        return false;
                    }

                    try {
                        marks.push_back(
                            Mark{p[0], p[1], p[2], stoi(p[3])}
                        );
                    } catch (...) {
                        error = "Invalid numeric value in marks.txt.";
                        return false;
                    }
                }
            }
        }

        {
            ifstream file((dir + "/registrations.txt").c_str());

            if (file) {
                string line;

                while (getline(file, line)) {
                    if (line.empty()) continue;

                    vector<string> p = split(line);
                    if (p.size() != 2) {
                        error = "Malformed registrations.txt.";
                        return false;
                    }

                    registrations.push_back(
                        Registration{p[0], p[1]}
                    );
                }
            }
        }

        if (courses.empty() || students.empty()) {
            seed();

            if (!save()) {
                error = "Could not create initial data files.";
                return false;
            }
        }

        return true;
    }

    bool save() const {
        return saveFaculty() &&
               saveCourses() &&
               saveExams() &&
               saveStudents() &&
               saveMarks() &&
               saveRegistrations();
    }

    // US-01
    bool addFaculty(Faculty value, string& error) {
        if (!validFaculty(value, error)) {
            return false;
        }

        if (faculty.count(value.id) != 0) {
            error = "Faculty ID already exists.";
            return false;
        }

        faculty[value.id] = value;
        return save();
    }

    // US-02
    vector<Faculty> getFacultyList() const {
        vector<Faculty> result;

        for (map<string, Faculty>::const_iterator it = faculty.begin();
             it != faculty.end(); ++it) {
            result.push_back(it->second);
        }

        return result;
    }

    const Faculty* getFaculty(const string& id) const {
        map<string, Faculty>::const_iterator it = faculty.find(id);
        return it == faculty.end() ? NULL : &it->second;
    }

    // US-03
    bool updateFaculty(const string& id,
                       Faculty updated,
                       string& error) {
        map<string, Faculty>::iterator it = faculty.find(id);

        if (it == faculty.end()) {
            error = "Faculty not found.";
            return false;
        }

        if (!validFaculty(updated, error)) {
            return false;
        }

        if (updated.id != id &&
            faculty.count(updated.id) != 0) {
            error = "Replacement faculty ID already exists.";
            return false;
        }

        updated.courseId = it->second.courseId;

        const string newId = updated.id;

        faculty.erase(it);
        faculty[newId] = updated;

        for (map<string, Course>::iterator courseIt = courses.begin();
             courseIt != courses.end(); ++courseIt) {
            if (courseIt->second.facultyId == id) {
                courseIt->second.facultyId = newId;
            }
        }

        for (size_t i = 0; i < marks.size(); ++i) {
            if (marks[i].facultyId == id) {
                marks[i].facultyId = newId;
            }
        }

        return save();
    }

    // US-04
    bool removeFaculty(const string& id, string& error) {
        map<string, Faculty>::iterator it = faculty.find(id);

        if (it == faculty.end()) {
            error = "Faculty not found.";
            return false;
        }

        for (map<string, Course>::iterator courseIt = courses.begin();
             courseIt != courses.end(); ++courseIt) {
            if (courseIt->second.facultyId == id) {
                courseIt->second.facultyId.clear();
            }
        }

        for (size_t i = 0; i < marks.size(); ++i) {
            if (marks[i].facultyId == id) {
                marks[i].facultyId.clear();
            }
        }

        faculty.erase(it);
        return save();
    }

    // US-05
    bool assignFacultyToCourse(const string& facultyId,
                               const string& courseId,
                               string& error) {
        map<string, Faculty>::iterator facultyIt =
            faculty.find(facultyId);

        map<string, Course>::iterator courseIt =
            courses.find(courseId);

        if (facultyIt == faculty.end()) {
            error = "Faculty not found.";
            return false;
        }

        if (courseIt == courses.end()) {
            error = "Course not found.";
            return false;
        }

        if (!courseIt->second.facultyId.empty() &&
            courseIt->second.facultyId != facultyId) {
            error = "Course already has another faculty member.";
            return false;
        }

        if (!facultyIt->second.courseId.empty() &&
            facultyIt->second.courseId != courseId) {
            map<string, Course>::iterator oldCourse =
                courses.find(facultyIt->second.courseId);

            if (oldCourse != courses.end()) {
                oldCourse->second.facultyId.clear();
            }
        }

        facultyIt->second.courseId = courseId;
        courseIt->second.facultyId = facultyId;

        return save();
    }

    // US-06
    bool createExam(Exam exam, string& error) {
        if (exam.id.empty() || exam.title.empty()) {
            error = "Exam ID and title are required.";
            return false;
        }

        if (exam.maxMarks <= 0) {
            error = "Maximum marks must be positive.";
            return false;
        }

        if (exams.count(exam.id) != 0) {
            error = "Exam ID already exists.";
            return false;
        }

        if (!exam.courseId.empty() &&
            courses.count(exam.courseId) == 0) {
            error = "Course not found.";
            return false;
        }

        if ((!exam.date.empty() && !validDate(exam.date)) ||
            (!exam.time.empty() && !validTime(exam.time))) {
            error = "Invalid examination date or time.";
            return false;
        }

        exams[exam.id] = exam;
        return save();
    }

    // US-07
    bool scheduleExam(const string& examId,
                      const string& date,
                      const string& time,
                      string& error) {
        map<string, Exam>::iterator it = exams.find(examId);

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

    // US-08
    bool assignExamToCourse(const string& examId,
                            const string& courseId,
                            string& error) {
        map<string, Exam>::iterator examIt = exams.find(examId);

        if (examIt == exams.end()) {
            error = "Examination not found.";
            return false;
        }

        if (courses.count(courseId) == 0) {
            error = "Course not found.";
            return false;
        }

        examIt->second.courseId = courseId;
        return save();
    }

    // US-09
    bool enterMarks(const string& examId,
                    const string& studentId,
                    const string& facultyId,
                    int value,
                    string& error) {
        map<string, Exam>::const_iterator examIt = exams.find(examId);
        map<string, Student>::const_iterator studentIt =
            students.find(studentId);
        map<string, Faculty>::const_iterator facultyIt =
            faculty.find(facultyId);

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

        if (facultyIt->second.courseId != examIt->second.courseId) {
            error = "Faculty is not authorized for this exam course.";
            return false;
        }

        if (value < 0 || value > examIt->second.maxMarks) {
            error = "Marks are outside the allowed range.";
            return false;
        }

        for (size_t i = 0; i < marks.size(); ++i) {
            if (marks[i].examId == examId &&
                marks[i].studentId == studentId) {
                marks[i].facultyId = facultyId;
                marks[i].marks = value;
                return saveMarks();
            }
        }

        marks.push_back(
            Mark{examId, studentId, facultyId, value}
        );

        return saveMarks();
    }

    // US-10
    vector<Result> getResultsForStudent(
        const string& requesterId,
        const string& requestedStudentId,
        string& error) const {

        if (students.count(requesterId) == 0 ||
            students.count(requestedStudentId) == 0) {
            error = "Student not found.";
            return vector<Result>();
        }

        if (requesterId != requestedStudentId) {
            error = "Access denied: a student can only view their own results.";
            return vector<Result>();
        }

        vector<Result> result;

        for (size_t i = 0; i < marks.size(); ++i) {
            if (marks[i].studentId != requesterId) {
                continue;
            }

            map<string, Exam>::const_iterator examIt =
                exams.find(marks[i].examId);

            if (examIt == exams.end() ||
                examIt->second.courseId.empty()) {
                continue;
            }

            map<string, Course>::const_iterator courseIt =
                courses.find(examIt->second.courseId);

            if (courseIt == courses.end()) {
                continue;
            }

            result.push_back(
                Result{
                    examIt->second.id,
                    examIt->second.title,
                    courseIt->second.code,
                    courseIt->second.title,
                    examIt->second.date,
                    examIt->second.time,
                    marks[i].marks,
                    examIt->second.maxMarks
                }
            );
        }

        sort(result.begin(), result.end(),
             [](const Result& a, const Result& b) {
                 return a.examId < b.examId;
             });

        return result;
    }

    // US-11
    vector<Course> availableCourses(
        const string& queryText = "") const {

        vector<Course> result;
        const string query = lowerText(queryText);

        for (map<string, Course>::const_iterator it = courses.begin();
             it != courses.end(); ++it) {
            const Course& course = it->second;

            const bool matches =
                query.empty() ||
                lowerText(course.code).find(query) != string::npos ||
                lowerText(course.title).find(query) != string::npos ||
                lowerText(course.department).find(query) != string::npos;

            if (matches &&
                course.enrolled < course.capacity) {
                result.push_back(course);
            }
        }

        sort(result.begin(), result.end(),
             [](const Course& a, const Course& b) {
                 return a.code < b.code;
             });

        return result;
    }

    // US-12 + US-13
    bool registerCourse(const string& studentId,
                        const string& courseId,
                        string& error) {
        if (students.count(studentId) == 0) {
            error = "Student not found.";
            return false;
        }

        map<string, Course>::iterator courseIt =
            courses.find(courseId);

        if (courseIt == courses.end()) {
            error = "Course not found.";
            return false;
        }

        if (hasRegistration(studentId, courseId)) {
            error = "Duplicate registration is not allowed.";
            return false;
        }

        if (courseIt->second.enrolled >=
            courseIt->second.capacity) {
            error = "Course is full.";
            return false;
        }

        registrations.push_back(
            Registration{studentId, courseId}
        );

        courseIt->second.enrolled++;

        return save();
    }

    bool hasRegistration(const string& studentId,
                         const string& courseId) const {
        for (size_t i = 0; i < registrations.size(); ++i) {
            if (registrations[i].studentId == studentId &&
                registrations[i].courseId == courseId) {
                return true;
            }
        }

        return false;
    }

    vector<Course> getRegisteredCourses(
        const string& studentId,
        string& error) const {

        if (students.count(studentId) == 0) {
            error = "Student not found.";
            return vector<Course>();
        }

        vector<Course> result;

        for (size_t i = 0; i < registrations.size(); ++i) {
            if (registrations[i].studentId != studentId) {
                continue;
            }

            map<string, Course>::const_iterator courseIt =
                courses.find(registrations[i].courseId);

            if (courseIt != courses.end()) {
                result.push_back(courseIt->second);
            }
        }

        sort(result.begin(), result.end(),
             [](const Course& a, const Course& b) {
                 return a.code < b.code;
             });

        return result;
    }

    // US-14
    bool capacity(const string& courseId,
                  int& remaining,
                  bool& full,
                  string& error) const {
        map<string, Course>::const_iterator it =
            courses.find(courseId);

        if (it == courses.end()) {
            error = "Course not found.";
            return false;
        }

        remaining =
            max(0, it->second.capacity - it->second.enrolled);

        full =
            it->second.enrolled >= it->second.capacity;

        return true;
    }

    // US-15
    bool dropCourse(const string& studentId,
                    const string& courseId,
                    string& error) {
        if (students.count(studentId) == 0) {
            error = "Student not found.";
            return false;
        }

        vector<Registration>::iterator registrationIt =
            registrations.end();

        for (vector<Registration>::iterator it = registrations.begin();
             it != registrations.end(); ++it) {
            if (it->studentId == studentId &&
                it->courseId == courseId) {
                registrationIt = it;
                break;
            }
        }

        if (registrationIt == registrations.end()) {
            error = "Registration not found.";
            return false;
        }

        map<string, Course>::iterator courseIt =
            courses.find(courseId);

        if (courseIt != courses.end() &&
            courseIt->second.enrolled > 0) {
            courseIt->second.enrolled--;
        }

        registrations.erase(registrationIt);

        return save();
    }

    vector<Exam> getExamList() const {
        vector<Exam> result;

        for (map<string, Exam>::const_iterator it = exams.begin();
             it != exams.end(); ++it) {
            result.push_back(it->second);
        }

        return result;
    }

    vector<Student> getStudentList() const {
        vector<Student> result;

        for (map<string, Student>::const_iterator it = students.begin();
             it != students.end(); ++it) {
            result.push_back(it->second);
        }

        return result;
    }

    vector<Mark> getMarks() const {
        return marks;
    }

    vector<Registration> getRegistrations() const {
        return registrations;
    }
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

    return !answer.empty() &&
           (answer[0] == 'y' || answer[0] == 'Y');
}

static void facultyListUI(
    const UniversityManagementSystem& system) {

    header("FACULTY RECORDS");

    vector<Faculty> records =
        system.getFacultyList();

    if (records.empty()) {
        cout << "No faculty records found.\n";
        return;
    }

    for (size_t i = 0; i < records.size(); ++i) {
        cout << records[i].id << " | "
             << records[i].name << " | "
             << records[i].department << " | "
             << records[i].email << " | "
             << records[i].designation;

        if (!records[i].courseId.empty()) {
            cout << " | Course: "
                 << records[i].courseId;
        }

        cout << '\n';
    }
}

static void addFacultyUI(
    UniversityManagementSystem& system) {

    Faculty value{
        lineInput("Faculty ID: "),
        lineInput("Name: "),
        lineInput("Department: "),
        lineInput("Email: "),
        lineInput("Designation: "),
        ""
    };

    string error;

    if (system.addFaculty(value, error)) {
        cout << "Faculty added successfully.\n";
    } else {
        cout << "[ERROR] " << error << '\n';
    }
}

static void updateFacultyUI(
    UniversityManagementSystem& system) {

    const string id =
        lineInput("Faculty ID: ");

    const Faculty* old =
        system.getFaculty(id);

    if (old == NULL) {
        cout << "[ERROR] Faculty not found.\n";
        return;
    }

    Faculty updated = *old;

    string value =
        lineInput("Name [" + updated.name + "]: ");

    if (!value.empty()) {
        updated.name = value;
    }

    value =
        lineInput("Department [" + updated.department + "]: ");

    if (!value.empty()) {
        updated.department = value;
    }

    value =
        lineInput("Email [" + updated.email + "]: ");

    if (!value.empty()) {
        updated.email = value;
    }

    value =
        lineInput(
            "Designation [" + updated.designation + "]: "
        );

    if (!value.empty()) {
        updated.designation = value;
    }

    string error;

    if (system.updateFaculty(id, updated, error)) {
        cout << "Faculty updated successfully.\n";
    } else {
        cout << "[ERROR] " << error << '\n';
    }
}

static void removeFacultyUI(
    UniversityManagementSystem& system) {

    const string id =
        lineInput("Faculty ID to remove: ");

    if (system.getFaculty(id) == NULL) {
        cout << "[ERROR] Faculty not found.\n";
        return;
    }

    if (!confirm("Confirm faculty removal")) {
        cout << "Removal cancelled.\n";
        return;
    }

    string error;

    if (system.removeFaculty(id, error)) {
        cout << "Faculty removed successfully.\n";
    } else {
        cout << "[ERROR] " << error << '\n';
    }
}

static void assignFacultyUI(
    UniversityManagementSystem& system) {

    const string facultyId =
        lineInput("Faculty ID: ");

    const string courseId =
        lineInput("Course ID: ");

    string error;

    if (system.assignFacultyToCourse(
            facultyId, courseId, error)) {
        cout << "Faculty-course assignment saved.\n";
    } else {
        cout << "[ERROR] " << error << '\n';
    }
}

static void createExamUI(
    UniversityManagementSystem& system) {

    Exam exam{
        lineInput("Exam ID: "),
        lineInput("Exam title: "),
        "",
        numberInput("Maximum marks: "),
        "",
        ""
    };

    string error;

    if (system.createExam(exam, error)) {
        cout << "Examination created.\n";
    } else {
        cout << "[ERROR] " << error << '\n';
    }
}

static void scheduleExamUI(
    UniversityManagementSystem& system) {

    const string examId =
        lineInput("Exam ID: ");

    const string date =
        lineInput("Date (YYYY-MM-DD): ");

    const string time =
        lineInput("Time (HH:MM): ");

    string error;

    if (system.scheduleExam(
            examId, date, time, error)) {
        cout << "Examination schedule saved.\n";
    } else {
        cout << "[ERROR] " << error << '\n';
    }
}

static void assignExamUI(
    UniversityManagementSystem& system) {

    const string examId =
        lineInput("Exam ID: ");

    const string courseId =
        lineInput("Course ID: ");

    string error;

    if (system.assignExamToCourse(
            examId, courseId, error)) {
        cout << "Examination-course association saved.\n";
    } else {
        cout << "[ERROR] " << error << '\n';
    }
}

static void marksUI(
    UniversityManagementSystem& system) {

    header("ENTER STUDENT MARKS");

    vector<Exam> examList =
        system.getExamList();

    vector<Student> studentList =
        system.getStudentList();

    for (size_t i = 0; i < examList.size(); ++i) {
        cout << examList[i].id << " | "
             << examList[i].title
             << " | Course: "
             << (examList[i].courseId.empty()
                     ? "Unassigned"
                     : examList[i].courseId)
             << " | Max: "
             << examList[i].maxMarks;

        if (!examList[i].date.empty()) {
            cout << " | "
                 << examList[i].date << " "
                 << examList[i].time;
        }

        cout << '\n';
    }

    for (size_t i = 0; i < studentList.size(); ++i) {
        cout << studentList[i].id << " | "
             << studentList[i].name << '\n';
    }

    const string examId =
        lineInput("Exam ID: ");

    const string studentId =
        lineInput("Student ID: ");

    const string facultyId =
        lineInput("Faculty ID: ");

    const int value =
        numberInput("Marks: ");

    string error;

    if (system.enterMarks(
            examId, studentId, facultyId, value, error)) {
        cout << "Marks saved.\n";
    } else {
        cout << "[ERROR] " << error << '\n';
    }
}

static void resultsUI(
    const UniversityManagementSystem& system) {

    const string requesterId =
        lineInput("Your student ID: ");

    const string requestedId =
        lineInput(
            "Student ID whose results are requested: "
        );

    string error;

    vector<Result> results =
        system.getResultsForStudent(
            requesterId, requestedId, error
        );

    if (!error.empty()) {
        cout << "[ERROR] " << error << '\n';
        return;
    }

    header("EXAMINATION RESULTS");

    if (results.empty()) {
        cout << "No examination results found.\n";
        return;
    }

    for (size_t i = 0; i < results.size(); ++i) {
        cout << results[i].examId << " | "
             << results[i].examTitle << " | "
             << results[i].courseCode << " | "
             << results[i].courseTitle << " | "
             << results[i].marks << "/"
             << results[i].maxMarks;

        if (!results[i].date.empty()) {
            cout << " | "
                 << results[i].date << " "
                 << results[i].time;
        }

        cout << '\n';
    }
}

static void coursesUI(
    const UniversityManagementSystem& system) {

    header("AVAILABLE COURSES");

    const string query =
        lineInput("Search (Enter for all): ");

    vector<Course> available =
        system.availableCourses(query);

    if (available.empty()) {
        cout << "No available courses found.\n";
        return;
    }

    for (size_t i = 0; i < available.size(); ++i) {
        cout << available[i].id << " | "
             << available[i].code << " | "
             << available[i].title << " | "
             << available[i].department << " | "
             << available[i].enrolled << "/"
             << available[i].capacity << '\n';
    }
}

static void registerCourseUI(
    UniversityManagementSystem& system) {

    const string studentId =
        lineInput("Student ID: ");

    const string courseId =
        lineInput("Course ID: ");

    string error;

    if (system.registerCourse(
            studentId, courseId, error)) {
        cout << "Course registration successful.\n";
    } else {
        cout << "[ERROR] " << error << '\n';
    }
}

static void duplicateCheckUI(
    const UniversityManagementSystem& system) {

    const string studentId =
        lineInput("Student ID: ");

    const string courseId =
        lineInput("Course ID: ");

    if (system.hasRegistration(
            studentId, courseId)) {
        cout << "Duplicate registration detected; "
             << "new registration is blocked.\n";
    } else {
        cout << "No existing registration found.\n";
    }
}

static void capacityUI(
    const UniversityManagementSystem& system) {

    const string courseId =
        lineInput("Course ID: ");

    int remaining = 0;
    bool full = false;
    string error;

    if (!system.capacity(
            courseId, remaining, full, error)) {
        cout << "[ERROR] " << error << '\n';
        return;
    }

    cout << "Remaining seats: "
         << remaining << '\n'
         << (full
             ? "Course is FULL; registration must be blocked.\n"
             : "Capacity available.\n");
}

static void registeredCoursesUI(
    const UniversityManagementSystem& system) {

    const string studentId =
        lineInput("Student ID: ");

    string error;

    vector<Course> result =
        system.getRegisteredCourses(
            studentId, error
        );

    if (!error.empty()) {
        cout << "[ERROR] " << error << '\n';
        return;
    }

    header("REGISTERED COURSES");

    if (result.empty()) {
        cout << "No courses registered.\n";
        return;
    }

    for (size_t i = 0; i < result.size(); ++i) {
        cout << result[i].id << " | "
             << result[i].code << " | "
             << result[i].title << '\n';
    }
}

static void dropCourseUI(
    UniversityManagementSystem& system) {

    const string studentId =
        lineInput("Student ID: ");

    const string courseId =
        lineInput("Course ID: ");

    if (!system.hasRegistration(
            studentId, courseId)) {
        cout << "[ERROR] Registration not found.\n";
        return;
    }

    if (!confirm("Confirm course drop")) {
        cout << "Course drop cancelled.\n";
        return;
    }

    string error;

    if (system.dropCourse(
            studentId, courseId, error)) {
        cout << "Course dropped successfully.\n";
    } else {
        cout << "[ERROR] " << error << '\n';
    }
}

int main() {
    UniversityManagementSystem system("data");
    string error;

    if (!system.load(error)) {
        cerr << "[ERROR] " << error << '\n';
        return 1;
    }

    while (true) {
        header("UNIVERSITY MANAGEMENT SYSTEM - C++11 PROJECT");

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
            case 1:
                addFacultyUI(system);
                break;

            case 2:
                facultyListUI(system);
                break;

            case 3:
                updateFacultyUI(system);
                break;

            case 4:
                removeFacultyUI(system);
                break;

            case 5:
                assignFacultyUI(system);
                break;

            case 6:
                createExamUI(system);
                break;

            case 7:
                scheduleExamUI(system);
                break;

            case 8:
                assignExamUI(system);
                break;

            case 9:
                marksUI(system);
                break;

            case 10:
                resultsUI(system);
                break;

            case 11:
                coursesUI(system);
                break;

            case 12:
                registerCourseUI(system);
                break;

            case 13:
                duplicateCheckUI(system);
                break;

            case 14:
                capacityUI(system);
                break;

            case 15:
                registeredCoursesUI(system);
                break;

            case 16:
                dropCourseUI(system);
                break;

            case 17:
                return 0;

            default:
                cout << "Invalid choice.\n";
                break;
        }
    }

    return 0;
}
