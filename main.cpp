#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Faculty {
    string id,name,department,email,designation;
    optional<string> courseId;
};

struct Course {
    string id,code,title,department;
    int capacity=0,enrolled=0;
    optional<string> facultyId;
};

struct Exam {
    string id,title,courseId;
    int maxMarks=0;
};

struct Student { string id,name; };
struct Mark { string examId,studentId,facultyId; int marks=0; };

class UniversityManagementSystem {
    unordered_map<string,Faculty> faculty;
    unordered_map<string,Course> courses;
    unordered_map<string,Exam> exams;
    unordered_map<string,Student> students;
    vector<Mark> marks;
    string dir;

    static vector<string> split(const string& s,char d='|') {
        vector<string> p; string x; stringstream ss(s);
        while(getline(ss,x,d)) p.push_back(x);
        return p;
    }
    static string lower(string s) {
        transform(s.begin(),s.end(),s.begin(),[](unsigned char c){return char(tolower(c));});
        return s;
    }
    bool saveFaculty() const {
        ofstream f(dir+"/faculty.txt"); if(!f) return false;
        for(auto& [k,v]:faculty)
            f<<v.id<<'|'<<v.name<<'|'<<v.department<<'|'<<v.email<<'|'<<v.designation<<'|'<<(v.courseId?*v.courseId:"")<<"\n";
        return true;
    }
    bool saveCourses() const {
        ofstream f(dir+"/courses.txt"); if(!f) return false;
        for(auto& [k,v]:courses)
            f<<v.id<<'|'<<v.code<<'|'<<v.title<<'|'<<v.department<<'|'<<v.capacity<<'|'<<v.enrolled<<'|'<<(v.facultyId?*v.facultyId:"")<<"\n";
        return true;
    }
    bool saveExams() const {
        ofstream f(dir+"/exams.txt"); if(!f) return false;
        for(auto& [k,v]:exams) f<<v.id<<'|'<<v.title<<'|'<<v.courseId<<'|'<<v.maxMarks<<"\n";
        return true;
    }
    bool saveStudents() const {
        ofstream f(dir+"/students.txt"); if(!f) return false;
        for(auto& [k,v]:students) f<<v.id<<'|'<<v.name<<"\n";
        return true;
    }
    bool saveMarks() const {
        ofstream f(dir+"/marks.txt"); if(!f) return false;
        for(const auto& v:marks) f<<v.examId<<'|'<<v.studentId<<'|'<<v.facultyId<<'|'<<v.marks<<"\n";
        return true;
    }
    bool validFaculty(const Faculty& f,string& e) const {
        if(f.id.empty()||f.name.empty()||f.department.empty()||f.email.empty()||f.designation.empty()) {
            e="All faculty fields are required."; return false;
        }
        size_t a=f.email.find('@'), d=f.email.find('.',a==string::npos?0:a);
        if(a==string::npos||d==string::npos||a==0||d<=a+1) { e="Invalid email address."; return false; }
        return true;
    }
public:
    explicit UniversityManagementSystem(string dataDir="data"):dir(move(dataDir)) { filesystem::create_directories(dir); }

    void seed() {
        if(courses.empty()) {
            courses["C001"]={"C001","CS101","Programming Fundamentals","Computer Science",40,35,nullopt};
            courses["C002"]={"C002","DB201","Database Systems","Computer Science",30,30,nullopt};
            courses["C003"]={"C003","SE210","Software Engineering","Computer Science",35,21,nullopt};
            courses["C004"]={"C004","MTH101","Calculus I","Mathematics",50,44,nullopt};
        }
        if(students.empty()) {
            students["S001"]={"S001","Ali Raza"};
            students["S002"]={"S002","Sara Khan"};
            students["S003"]={"S003","Hamza Ahmed"};
        }
    }

    bool load(string& e) {
        auto read=[&](const string& file,auto fn)->bool {
            ifstream in(dir+"/"+file); if(!in) return true;
            string line; while(getline(in,line)) if(!line.empty()) if(!fn(split(line))) {e="Malformed "+file; return false;} return true;
        };
        faculty.clear(); courses.clear(); exams.clear(); students.clear(); marks.clear();

        if(!read("courses.txt",[&](const vector<string>& p){ if(p.size()!=7)return false; courses[p[0]]={p[0],p[1],p[2],p[3],stoi(p[4]),stoi(p[5]),p[6].empty()?nullopt:optional<string>(p[6])}; return true; })) return false;
        if(!read("students.txt",[&](const vector<string>& p){ if(p.size()!=2)return false; students[p[0]]={p[0],p[1]}; return true; })) return false;
        if(!read("faculty.txt",[&](const vector<string>& p){ if(p.size()!=6)return false; faculty[p[0]]={p[0],p[1],p[2],p[3],p[4],p[5].empty()?nullopt:optional<string>(p[5])}; return true; })) return false;
        if(!read("exams.txt",[&](const vector<string>& p){ if(p.size()!=4)return false; exams[p[0]]={p[0],p[1],p[2],stoi(p[3])}; return true; })) return false;
        if(!read("marks.txt",[&](const vector<string>& p){ if(p.size()!=4)return false; marks.push_back({p[0],p[1],p[2],stoi(p[3])}); return true; })) return false;

        if(courses.empty()||students.empty()) { seed(); save(); }
        return true;
    }

    bool save() const {
        return saveFaculty()&&saveCourses()&&saveExams()&&saveStudents()&&saveMarks();
    }

    // US-01 / UMS-24
    bool addFaculty(Faculty f,string& e) {
        if(!validFaculty(f,e)) return false;
        if(faculty.count(f.id)){e="Faculty ID already exists.";return false;}
        faculty[f.id]=move(f); return save();
    }

    // US-03 / UMS-11
    bool updateFaculty(const string& id,Faculty f,string& e) {
        auto it=faculty.find(id); if(it==faculty.end()){e="Faculty not found.";return false;}
        if(!validFaculty(f,e)) return false;
        if(f.id!=id && faculty.count(f.id)){e="Replacement faculty ID already exists.";return false;}
        f.courseId=it->second.courseId;
        const string old=id;
        const string newId=f.id;
        faculty.erase(it);
        faculty[newId]=move(f);
        for(auto& [k,c]:courses)
            if(c.facultyId&&*c.facultyId==old) c.facultyId=newId;
        return save();
    }

    // US-05 / UMS-13
    bool assignFacultyToCourse(const string& fid,const string& cid,string& e) {
        auto fi=faculty.find(fid); auto ci=courses.find(cid);
        if(fi==faculty.end()){e="Faculty not found.";return false;}
        if(ci==courses.end()){e="Course not found.";return false;}
        if(ci->second.facultyId&&*ci->second.facultyId!=fid){e="Course already has another faculty member.";return false;}
        if(fi->second.courseId&&*fi->second.courseId!=cid) courses[*fi->second.courseId].facultyId.reset();
        fi->second.courseId=cid; ci->second.facultyId=fid; return save();
    }

    // US-06 / UMS-14
    bool createExam(Exam x,string& e) {
        if(x.id.empty()||x.title.empty()||x.courseId.empty()){e="Exam ID, title and course are required.";return false;}
        if(x.maxMarks<=0){e="Maximum marks must be positive.";return false;}
        if(exams.count(x.id)){e="Exam ID already exists.";return false;}
        if(!courses.count(x.courseId)){e="Course not found.";return false;}
        exams[x.id]=move(x); return save();
    }

    // US-09 / UMS-17
    bool enterMarks(const string& eid,const string& sid,const string& fid,int value,string& e) {
        auto ex=exams.find(eid);\n        auto st=students.find(sid);\n        auto fa=faculty.find(fid);
        if(ex==exams.end()){e="Examination not found.";return false;}
        if(st==students.end()){e="Student not found.";return false;}
        if(fa==faculty.end()){e="Faculty not found.";return false;}
        if(!fa->second.courseId||*fa->second.courseId!=ex->second.courseId){e="Faculty is not authorized for this exam course.";return false;}
        if(value<0||value>ex->second.maxMarks){e="Marks are outside the allowed range.";return false;}
        auto it=find_if(marks.begin(),marks.end(),[&](const Mark& m){return m.examId==eid&&m.studentId==sid;});
        if(it==marks.end()) marks.push_back({eid,sid,fid,value}); else {it->facultyId=fid;it->marks=value;}
        return saveMarks();
    }

    // US-11 / UMS-19
    vector<Course> availableCourses(const string& q="") const {
        vector<Course> r; string query=lower(q);
        for(const auto& [k,c]:courses) {
            bool match=query.empty()||lower(c.code).find(query)!=string::npos||lower(c.title).find(query)!=string::npos||lower(c.department).find(query)!=string::npos;
            if(c.enrolled<c.capacity&&match) r.push_back(c);
        }
        sort(r.begin(),r.end(),[](const Course&a,const Course&b){return a.code<b.code;}); return r;
    }

    // US-14 / UMS-22
    bool capacity(const string& cid,int& remaining,bool& full,string& e) const {
        auto it=courses.find(cid); if(it==courses.end()){e="Course not found.";return false;}
        remaining=max(0,it->second.capacity-it->second.enrolled); full=it->second.enrolled>=it->second.capacity; return true;
    }

    const Faculty* getFaculty(const string& id) const {auto it=faculty.find(id);return it==faculty.end()?nullptr:&it->second;}
    vector<Faculty> getFacultyList() const {vector<Faculty> r;for(auto&[k,v]:faculty)r.push_back(v);sort(r.begin(),r.end(),[](auto&a,auto&b){return a.id<b.id;});return r;}
    vector<Exam> getExamList() const {vector<Exam> r;for(auto&[k,v]:exams)r.push_back(v);return r;}
    vector<Student> getStudentList() const {vector<Student> r;for(auto&[k,v]:students)r.push_back(v);return r;}
    vector<Mark> getMarks() const {return marks;}
};

static void header(const string& s){cout<<"\n========================================\n"<<s<<"\n========================================\n";}
static string line(const string& p){cout<<p;string s;getline(cin,s);return s;}
static int number(const string& p){
    while(true){cout<<p;int n;if(cin>>n){cin.ignore(numeric_limits<streamsize>::max(),'\n');return n;}
        cin.clear();cin.ignore(numeric_limits<streamsize>::max(),'\n');cout<<"Invalid number.\n";}
}

static void facultyList(const UniversityManagementSystem& u){
    header("FACULTY RECORDS"); for(const auto& f:u.getFacultyList())
        cout<<f.id<<" | "<<f.name<<" | "<<f.department<<" | "<<f.email<<" | "<<f.designation<<(f.courseId?" | Course: "+*f.courseId:"")<<"\n";
}
static void addFacultyUI(UniversityManagementSystem&u){
    Faculty f{line("Faculty ID: "),line("Name: "),line("Department: "),line("Email: "),line("Designation: "),nullopt};string e;
    cout<<(u.addFaculty(f,e)?"Faculty added successfully.\n":"[ERROR] "+e+"\n");
}
static void updateFacultyUI(UniversityManagementSystem&u){
    string id=line("Faculty ID: ");const Faculty* old=u.getFaculty(id);if(!old){cout<<"[ERROR] Faculty not found.\n";return;}
    Faculty f=*old; string s=line("Name ["+f.name+"]: ");if(!s.empty())f.name=s;
    s=line("Department ["+f.department+"]: ");if(!s.empty())f.department=s;
    s=line("Email ["+f.email+"]: ");if(!s.empty())f.email=s;
    s=line("Designation ["+f.designation+"]: ");if(!s.empty())f.designation=s;
    string e;cout<<(u.updateFaculty(id,f,e)?"Faculty updated successfully.\n":"[ERROR] "+e+"\n");
}
static void coursesUI(const UniversityManagementSystem&u){
    header("AVAILABLE COURSES");auto r=u.availableCourses(line("Search (Enter for all): "));
    if(r.empty()){cout<<"No available courses found.\n";return;}
    for(const auto& c:r)cout<<c.id<<" | "<<c.code<<" | "<<c.title<<" | "<<c.department<<" | "<<c.enrolled<<"/"<<c.capacity<<"\n";
}
static void assignUI(UniversityManagementSystem&u){
    string e;cout<<(u.assignFacultyToCourse(line("Faculty ID: "),line("Course ID: "),e)?"Assignment saved.\n":"[ERROR] "+e+"\n");
}
static void examUI(UniversityManagementSystem&u){
    Exam x{line("Exam ID: "),line("Exam title: "),line("Course ID: "),number("Maximum marks: ")};string e;
    cout<<(u.createExam(x,e)?"Examination created.\n":"[ERROR] "+e+"\n");
}
static void marksUI(UniversityManagementSystem&u){
    header("ENTER STUDENT MARKS");for(auto&e:u.getExamList())cout<<e.id<<" | "<<e.title<<" | Course "<<e.courseId<<" | Max "<<e.maxMarks<<"\n";
    for(auto&s:u.getStudentList())cout<<s.id<<" | "<<s.name<<"\n";
    string e;cout<<(u.enterMarks(line("Exam ID: "),line("Student ID: "),line("Faculty ID: "),number("Marks: "),e)?"Marks saved.\n":"[ERROR] "+e+"\n");
}
static void capacityUI(const UniversityManagementSystem&u){
    int rem=0;bool full=false;string e;if(!u.capacity(line("Course ID: "),rem,full,e))cout<<"[ERROR] "<<e<<"\n";
    else cout<<"Remaining seats: "<<rem<<"\n"<<(full?"Course is FULL; registration must be blocked.\n":"Capacity available.\n");
}

int main(){
    UniversityManagementSystem u("data");string e;if(!u.load(e)){cerr<<"[ERROR] "<<e<<"\n";return 1;}
    while(true){
        header("UNIVERSITY MANAGEMENT SYSTEM - ABEERAH'S MODULE");
        cout<<"1. Add Faculty\n2. Update Faculty\n3. Assign Faculty to Course\n4. Create Examination\n5. Enter Student Marks\n6. View Available Courses\n7. Check Course Capacity\n8. View Faculty Records\n9. Exit\n";
        switch(number("Choose: ")){case 1:addFacultyUI(u);break;case 2:updateFacultyUI(u);break;case 3:assignUI(u);break;case 4:examUI(u);break;case 5:marksUI(u);break;case 6:coursesUI(u);break;case 7:capacityUI(u);break;case 8:facultyList(u);break;case 9:return 0;default:cout<<"Invalid choice.\n";}
    }
}
