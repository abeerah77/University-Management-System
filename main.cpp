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

struct Faculty{string id,name,department,email,designation;optional<string> courseId;};
struct Course{string id,code,title,department;int capacity=0,enrolled=0;optional<string> facultyId;};
struct Exam{string id,title,courseId;int maxMarks=0;optional<string> scheduledAt;};
struct Student{string id,name;};
struct Mark{string examId,studentId,facultyId;int marks=0;};
struct Registration{string studentId,courseId;};

class UniversityManagementSystem{
 unordered_map<string,Faculty> faculty; unordered_map<string,Course> courses; unordered_map<string,Exam> exams; unordered_map<string,Student> students;
 vector<Mark> marks; vector<Registration> registrations; string dir;
 static vector<string> split(const string&s){vector<string>p;string x;stringstream ss(s);while(getline(ss,x,'|'))p.push_back(x);return p;}
 static string lower(string s){transform(s.begin(),s.end(),s.begin(),[](unsigned char c){return char(tolower(c));});return s;}
 bool saveFaculty()const{ofstream f(dir+"/faculty.txt");if(!f)return false;for(auto&[k,v]:faculty)f<<v.id<<'|'<<v.name<<'|'<<v.department<<'|'<<v.email<<'|'<<v.designation<<'|'<<(v.courseId?*v.courseId:"")<<'\n';return true;}
 bool saveCourses()const{ofstream f(dir+"/courses.txt");if(!f)return false;for(auto&[k,v]:courses)f<<v.id<<'|'<<v.code<<'|'<<v.title<<'|'<<v.department<<'|'<<v.capacity<<'|'<<v.enrolled<<'|'<<(v.facultyId?*v.facultyId:"")<<'\n';return true;}
 bool saveExams()const{ofstream f(dir+"/exams.txt");if(!f)return false;for(auto&[k,v]:exams)f<<v.id<<'|'<<v.title<<'|'<<v.courseId<<'|'<<v.maxMarks<<'|'<<(v.scheduledAt?*v.scheduledAt:"")<<'\n';return true;}
 bool saveStudents()const{ofstream f(dir+"/students.txt");if(!f)return false;for(auto&[k,v]:students)f<<v.id<<'|'<<v.name<<'\n';return true;}
 bool saveMarks()const{ofstream f(dir+"/marks.txt");if(!f)return false;for(auto&m:marks)f<<m.examId<<'|'<<m.studentId<<'|'<<m.facultyId<<'|'<<m.marks<<'\n';return true;}
 bool saveRegistrations()const{ofstream f(dir+"/registrations.txt");if(!f)return false;for(auto&r:registrations)f<<r.studentId<<'|'<<r.courseId<<'\n';return true;}
 static bool validDateTime(const string&s){static const regex r(R"(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}$)");if(!regex_match(s,r))return false;int y=stoi(s.substr(0,4)),m=stoi(s.substr(5,2)),d=stoi(s.substr(8,2)),h=stoi(s.substr(11,2)),n=stoi(s.substr(14,2));if(m<1||m>12||h>23||n>59||d<1)return false;int days[]={31,28,31,30,31,30,31,31,30,31,30,31};if(m==2&&(y%400==0||(y%4==0&&y%100!=0)))days[1]=29;return d<=days[m-1];}
 bool validFaculty(const Faculty&f,string&e)const{size_t a=f.email.find('@'),d=f.email.find('.',a==string::npos?0:a);if(f.id.empty()||f.name.empty()||f.department.empty()||f.email.empty()||f.designation.empty()){e="All faculty fields are required.";return false;}if(a==string::npos||d==string::npos||a==0||d<=a+1){e="Invalid email address.";return false;}return true;}
public:
 explicit UniversityManagementSystem(string dataDir="data"):dir(move(dataDir)){filesystem::create_directories(dir);}
 void seed(){if(courses.empty()){courses["C001"]={"C001","CS101","Programming Fundamentals","Computer Science",40,35,nullopt};courses["C002"]={"C002","DB201","Database Systems","Computer Science",30,30,nullopt};courses["C003"]={"C003","SE210","Software Engineering","Computer Science",35,21,nullopt};courses["C004"]={"C004","MTH101","Calculus I","Mathematics",50,44,nullopt};}if(students.empty()){students["S001"]={"S001","Ali Raza"};students["S002"]={"S002","Sara Khan"};students["S003"]={"S003","Hamza Ahmed"};}}
 bool load(string&e){
  auto read=[&](string file,auto fn){ifstream in(dir+"/"+file);if(!in)return true;string line;while(getline(in,line))if(!line.empty()&&!fn(split(line))){e="Malformed "+file;return false;}return true;};
  faculty.clear();courses.clear();exams.clear();students.clear();marks.clear();registrations.clear();
  if(!read("courses.txt",[&](auto p){if(p.size()!=7)return false;try{courses[p[0]]={p[0],p[1],p[2],p[3],stoi(p[4]),stoi(p[5]),p[6].empty()?nullopt:optional<string>(p[6])};}catch(...){return false;}return true;}))return false;
  if(!read("students.txt",[&](auto p){if(p.size()!=2)return false;students[p[0]]={p[0],p[1]};return true;}))return false;
  if(!read("faculty.txt",[&](auto p){if(p.size()!=6)return false;faculty[p[0]]={p[0],p[1],p[2],p[3],p[4],p[5].empty()?nullopt:optional<string>(p[5])};return true;}))return false;
  if(!read("exams.txt",[&](auto p){if(p.size()!=4&&p.size()!=5)return false;try{exams[p[0]]={p[0],p[1],p[2],stoi(p[3]),p.size()==5&&!p[4].empty()?optional<string>(p[4]):nullopt};}catch(...){return false;}return true;}))return false;
  if(!read("marks.txt",[&](auto p){if(p.size()!=4)return false;try{marks.push_back({p[0],p[1],p[2],stoi(p[3])});}catch(...){return false;}return true;}))return false;
  if(!read("registrations.txt",[&](auto p){if(p.size()!=2)return false;registrations.push_back({p[0],p[1]});return true;}))return false;
  if(courses.empty()||students.empty()){seed();save();}return true;
 }
 bool save()const{return saveFaculty()&&saveCourses()&&saveExams()&&saveStudents()&&saveMarks()&&saveRegistrations();}
 bool addFaculty(Faculty f,string&e){if(!validFaculty(f,e))return false;if(faculty.count(f.id)){e="Faculty ID already exists.";return false;}faculty[f.id]=move(f);return save();}
 bool updateFaculty(const string&id,Faculty f,string&e){auto it=faculty.find(id);if(it==faculty.end()){e="Faculty not found.";return false;}if(!validFaculty(f,e))return false;if(f.id!=id&&faculty.count(f.id)){e="Replacement faculty ID already exists.";return false;}f.courseId=it->second.courseId;string old=id,newId=f.id;faculty.erase(it);faculty[newId]=move(f);for(auto&[k,c]:courses)if(c.facultyId&&*c.facultyId==old)c.facultyId=newId;return save();}
 bool removeFaculty(const string&id,string&e){auto it=faculty.find(id);if(it==faculty.end()){e="Faculty not found.";return false;}for(auto&[k,c]:courses)if(c.facultyId&&*c.facultyId==id)c.facultyId.reset();faculty.erase(it);return save();}
 bool assignFacultyToCourse(const string&fid,const string&cid,string&e){auto fi=faculty.find(fid);auto ci=courses.find(cid);if(fi==faculty.end()){e="Faculty not found.";return false;}if(ci==courses.end()){e="Course not found.";return false;}if(ci->second.facultyId&&*ci->second.facultyId!=fid){e="Course already has another faculty member.";return false;}if(fi->second.courseId&&*fi->second.courseId!=cid)courses[*fi->second.courseId].facultyId.reset();fi->second.courseId=cid;ci->second.facultyId=fid;return save();}
 bool createExam(Exam x,string&e){if(x.id.empty()||x.title.empty()||x.courseId.empty()){e="Exam ID, title and course are required.";return false;}if(x.maxMarks<=0){e="Maximum marks must be positive.";return false;}if(exams.count(x.id)){e="Exam ID already exists.";return false;}if(!courses.count(x.courseId)){e="Course not found.";return false;}exams[x.id]=move(x);return save();}
 bool scheduleExam(const string&id,const string&when,string&e){auto it=exams.find(id);if(it==exams.end()){e="Examination not found.";return false;}if(!validDateTime(when)){e="Invalid date/time. Use YYYY-MM-DD HH:MM.";return false;}it->second.scheduledAt=when;return saveExams();}
 bool enterMarks(const string&eid,const string&sid,const string&fid,int value,string&e){auto ex=exams.find(eid);auto st=students.find(sid);auto fa=faculty.find(fid);if(ex==exams.end()){e="Examination not found.";return false;}if(st==students.end()){e="Student not found.";return false;}if(fa==faculty.end()){e="Faculty not found.";return false;}if(!fa->second.courseId||*fa->second.courseId!=ex->second.courseId){e="Faculty is not authorized for this exam course.";return false;}if(value<0||value>ex->second.maxMarks){e="Marks are outside the allowed range.";return false;}auto it=find_if(marks.begin(),marks.end(),[&](auto&m){return m.examId==eid&&m.studentId==sid;});if(it==marks.end())marks.push_back({eid,sid,fid,value});else{it->facultyId=fid;it->marks=value;}return saveMarks();}
 vector<Course> availableCourses(const string&q="")const{vector<Course>r;string x=lower(q);for(auto&[k,c]:courses)if(c.enrolled<c.capacity&&(x.empty()||lower(c.code).find(x)!=string::npos||lower(c.title).find(x)!=string::npos||lower(c.department).find(x)!=string::npos))r.push_back(c);sort(r.begin(),r.end(),[](auto&a,auto&b){return a.code<b.code;});return r;}
 bool capacity(const string&id,int&remaining,bool&full,string&e)const{auto it=courses.find(id);if(it==courses.end()){e="Course not found.";return false;}remaining=max(0,it->second.capacity-it->second.enrolled);full=it->second.enrolled>=it->second.capacity;return true;}
 bool registerCourse(const string&sid,const string&cid,string&e){if(!students.count(sid)){e="Student not found.";return false;}auto c=courses.find(cid);if(c==courses.end()){e="Course not found.";return false;}if(any_of(registrations.begin(),registrations.end(),[&](auto&r){return r.studentId==sid&&r.courseId==cid;})){e="Student is already registered in this course.";return false;}if(c->second.enrolled>=c->second.capacity){e="Course is full.";return false;}registrations.push_back({sid,cid});++c->second.enrolled;if(!save()){registrations.pop_back();--c->second.enrolled;e="Could not save registration.";return false;}return true;}
 bool dropCourse(const string&sid,const string&cid,string&e){auto r=find_if(registrations.begin(),registrations.end(),[&](auto&x){return x.studentId==sid&&x.courseId==cid;});if(r==registrations.end()){e="Registration not found.";return false;}auto c=courses.find(cid);if(c==courses.end()){e="Course not found.";return false;}registrations.erase(r);if(c->second.enrolled>0)--c->second.enrolled;if(!save()){registrations.push_back({sid,cid});++c->second.enrolled;e="Could not save course drop.";return false;}return true;}
 vector<Course> registeredCourses(const string&sid,string&e)const{if(!students.count(sid)){e="Student not found.";return{};}vector<Course>r;for(auto&x:registrations)if(x.studentId==sid){auto c=courses.find(x.courseId);if(c!=courses.end())r.push_back(c->second);}return r;}
 struct Result{string examId,examTitle,courseCode,scheduledAt;int marks,maxMarks;};
 vector<Result> resultsForStudent(const string&sid,string&e)const{if(!students.count(sid)){e="Student not found.";return{};}vector<Result>r;for(auto&m:marks)if(m.studentId==sid){auto ex=exams.find(m.examId);if(ex==exams.end())continue;auto c=courses.find(ex->second.courseId);r.push_back({ex->second.id,ex->second.title,c==courses.end()?"":c->second.code,ex->second.scheduledAt.value_or("Not scheduled"),m.marks,ex->second.maxMarks});}return r;}
 const Faculty* getFaculty(const string&id)const{auto it=faculty.find(id);return it==faculty.end()?nullptr:&it->second;}
 vector<Faculty> getFacultyList()const{vector<Faculty>r;for(auto&[k,v]:faculty)r.push_back(v);sort(r.begin(),r.end(),[](auto&a,auto&b){return a.id<b.id;});return r;}
 vector<Exam> getExamList()const{vector<Exam>r;for(auto&[k,v]:exams)r.push_back(v);return r;}
 vector<Student> getStudentList()const{vector<Student>r;for(auto&[k,v]:students)r.push_back(v);return r;}
 vector<Mark> getMarks()const{return marks;}
};

static void header(const string&s){cout<<"\n========================================\n"<<s<<"\n========================================\n";}
static string line(const string&p){cout<<p;string s;getline(cin,s);return s;}
static int number(const string&p){for(;;){cout<<p;int n;if(cin>>n){cin.ignore(numeric_limits<streamsize>::max(),'\n');return n;}cin.clear();cin.ignore(numeric_limits<streamsize>::max(),'\n');cout<<"Invalid number.\n";}}
static void facultyList(const UniversityManagementSystem&u){header("FACULTY RECORDS");for(auto&f:u.getFacultyList())cout<<f.id<<" | "<<f.name<<" | "<<f.department<<" | "<<f.email<<" | "<<f.designation<<(f.courseId?" | Course: "+*f.courseId:"")<<'\n';}
static void addFacultyUI(UniversityManagementSystem&u){Faculty f{line("Faculty ID: "),line("Name: "),line("Department: "),line("Email: "),line("Designation: "),nullopt};string e;cout<<(u.addFaculty(f,e)?"Faculty added successfully.\n":"[ERROR] "+e+"\n");}
static void updateFacultyUI(UniversityManagementSystem&u){string id=line("Faculty ID: ");auto old=u.getFaculty(id);if(!old){cout<<"[ERROR] Faculty not found.\n";return;}Faculty f=*old;string s=line("Name ["+f.name+"]: ");if(!s.empty())f.name=s;s=line("Department ["+f.department+"]: ");if(!s.empty())f.department=s;s=line("Email ["+f.email+"]: ");if(!s.empty())f.email=s;s=line("Designation ["+f.designation+"]: ");if(!s.empty())f.designation=s;string e;cout<<(u.updateFaculty(id,f,e)?"Faculty updated successfully.\n":"[ERROR] "+e+"\n");}
static void removeFacultyUI(UniversityManagementSystem&u){string e;cout<<(u.removeFaculty(line("Faculty ID: "),e)?"Faculty removed successfully.\n":"[ERROR] "+e+"\n");}
static void assignUI(UniversityManagementSystem&u){string e;cout<<(u.assignFacultyToCourse(line("Faculty ID: "),line("Course ID: "),e)?"Assignment saved.\n":"[ERROR] "+e+"\n");}
static void examUI(UniversityManagementSystem&u){Exam x{line("Exam ID: "),line("Exam title: "),line("Course ID: "),number("Maximum marks: "),nullopt};string e;cout<<(u.createExam(x,e)?"Examination created.\n":"[ERROR] "+e+"\n");}
static void scheduleUI(UniversityManagementSystem&u){for(auto&e:u.getExamList())cout<<e.id<<" | "<<e.title<<" | "<<(e.scheduledAt?*e.scheduledAt:"Not scheduled")<<'\n';string e;cout<<(u.scheduleExam(line("Exam ID: "),line("Date/time (YYYY-MM-DD HH:MM): "),e)?"Examination scheduled successfully.\n":"[ERROR] "+e+"\n");}
static void marksUI(UniversityManagementSystem&u){for(auto&e:u.getExamList())cout<<e.id<<" | "<<e.title<<" | Course "<<e.courseId<<" | Max "<<e.maxMarks<<'\n';string e;cout<<(u.enterMarks(line("Exam ID: "),line("Student ID: "),line("Faculty ID: "),number("Marks: "),e)?"Marks saved.\n":"[ERROR] "+e+"\n");}
static void coursesUI(const UniversityManagementSystem&u){auto r=u.availableCourses(line("Search (Enter for all): "));for(auto&c:r)cout<<c.id<<" | "<<c.code<<" | "<<c.title<<" | "<<c.enrolled<<"/"<<c.capacity<<'\n';if(r.empty())cout<<"No available courses found.\n";}
static void capacityUI(const UniversityManagementSystem&u){int rem;bool full;string e;if(u.capacity(line("Course ID: "),rem,full,e))cout<<"Remaining seats: "<<rem<<"\n"<<(full?"Course is FULL.\n":"Capacity available.\n");else cout<<"[ERROR] "<<e<<'\n';}
static void registerUI(UniversityManagementSystem&u){string e;cout<<(u.registerCourse(line("Student ID: "),line("Course ID: "),e)?"Registration saved successfully.\n":"[ERROR] "+e+"\n");}
static void dropUI(UniversityManagementSystem&u){string sid=line("Student ID: "),e;auto r=u.registeredCourses(sid,e);for(auto&c:r)cout<<c.id<<" | "<<c.code<<" | "<<c.title<<'\n';if(e.empty())cout<<(u.dropCourse(sid,line("Course ID: "),e)?"Course dropped successfully.\n":"[ERROR] "+e+"\n");else cout<<"[ERROR] "<<e<<'\n';}
static void resultsUI(const UniversityManagementSystem&u){string e;auto r=u.resultsForStudent(line("Student ID: "),e);if(!e.empty()){cout<<"[ERROR] "<<e<<'\n';return;}for(auto&x:r)cout<<x.examId<<" | "<<x.examTitle<<" | "<<x.courseCode<<" | "<<x.marks<<"/"<<x.maxMarks<<" | "<<x.scheduledAt<<'\n';if(r.empty())cout<<"No examination results found.\n";}

int main(){UniversityManagementSystem u;string e;if(!u.load(e)){cerr<<"[ERROR] "<<e<<'\n';return 1;}while(true){header("UNIVERSITY MANAGEMENT SYSTEM");cout<<"1. Add Faculty\n2. Update Faculty\n3. Remove Faculty\n4. Assign Faculty to Course\n5. Create Examination\n6. Schedule Examination\n7. Enter Student Marks\n8. View Examination Results\n9. View Available Courses\n10. Check Course Capacity\n11. Register for Course\n12. Drop Registered Course\n13. View Faculty Records\n14. Exit\n";switch(number("Choose: ")){case 1:addFacultyUI(u);break;case 2:updateFacultyUI(u);break;case 3:removeFacultyUI(u);break;case 4:assignUI(u);break;case 5:examUI(u);break;case 6:scheduleUI(u);break;case 7:marksUI(u);break;case 8:resultsUI(u);break;case 9:coursesUI(u);break;case 10:capacityUI(u);break;case 11:registerUI(u);break;case 12:dropUI(u);break;case 13:facultyList(u);break;case 14:return 0;default:cout<<"Invalid choice.\n";}}}
