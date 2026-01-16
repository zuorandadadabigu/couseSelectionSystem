export module registrar;

import std;
import :ui;
import :sectary;

using std::vector;
using std::string;
using std::print;

export class Registrar{
public:
    Registrar();
    ~Registrar();
    int exec();
    void initialize();
private:
    vector<unique_ptr<Student>> _students;
    vector<unique_ptr<Teacher>> _teachers;
    vector<unique_ptr<Course>> _courses;
    vector<unique_ptr<Sectary>> _sectary;
    Ui *m_ui;
    Student *findStudentById(const string &id);
    Course *findCourseById(const string &id);
};

Registrar::Registrar(){
    m_ui = new Ui(*this);
    initialize();
}

Registrar::~Registrar(){
    delete ui;
}

void Registrar::initialize(){
    _students.push_back(std::make_unique<Student>("1001", "Thomas"));
    _students.push_back(std::make_unique<Student>("1002", "Jerry"));
    _students.push_back(std::make_unique<Student>("1003", "Baker"));
    _students.push_back(std::make_unique<Student>("1004", "Tom"));
    _students.push_back(std::make_unique<Student>("1005", "Musk"));

    _courses.push_back(std::make_unique<Course>("CS101", "C Programming"));
    _courses.push_back(std::make_unique<Course>("CS201", "Data structure"));
    _courses.push_back(std::make_unique<Course>("MATH101", "Advanced Math"));

    _teachers.push_back(std::make_unique<Teacher>("T001", "艾琳"));
    _teachers.push_back(std::make_unique<Teacher>("T002", "冯玉"));
    _teachers.push_back(std::make_unique<Teacher>("T003", "成临"));//写完student和course再来改这里的初始化

    _sectary.push_back(std::make_unique<Sectary>("S001", "王秘书"));

    print("初始化完成.\n");
}

int Registrar::exec()
{
    return ui->exec();
}

Student *Registrar::findStudentById(const string &id){
    for (auto& student :_students) {
        if (student->hasId(id))
            return student;
    }
    return nullptr;
}

Course *Registrar::findCourseById(const string &id){
    for (auto& course : _courses) {
        if (course->hasId(id) )
            return course;
    }
    return nullptr;
}

Sectary *Registrar::findSectaryById(const string &id){
    for (auto& sectary : _sectary) {
        if (sectary->hasId(id) )
            return sectary;
    }
    return nullptr;
}




