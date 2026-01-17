export module registrar;

import std;
import :ui;
import :sectary;
import :teacher;
import :student;
import :course;

using std::vector;
using std::string;
using std::unique_ptr;
using std::make_unique;
using std::print;

export class Registrar {
public:
    Registrar();
    ~Registrar();
    int exec();
    void initialize();
    Student *findStudentById(const string &id);
    Course *findCourseById(const string &id);
    Sectary *findSectaryById(const string &id);
    Teacher *findTeacherById(const string &id);

    // 关联课程和教师
    void assignCourseToTeacher(Course* course, Teacher* teacher);

private:
    vector<unique_ptr<Student>> _students;
    vector<unique_ptr<Teacher>> _teachers;
    vector<unique_ptr<Course>> _courses;
    vector<unique_ptr<Sectary>> _sectary;
    Ui *m_ui;
};

Registrar::Registrar()
    : m_ui(new Ui(*this))
{
    initialize();
}

Registrar::~Registrar() {
    delete m_ui;
}

void Registrar::initialize() {
    // 创建学生
    _students.push_back(make_unique<Student>("1001", "Thomas"));
    _students.push_back(make_unique<Student>("1002", "Jerry"));
    _students.push_back(make_unique<Student>("1003", "Baker"));
    _students.push_back(make_unique<Student>("1004", "Tom"));
    _students.push_back(make_unique<Student>("1005", "Musk"));

    // 创建教师
    auto teacher1 = make_unique<Teacher>("T001", "艾琳");
    auto teacher2 = make_unique<Teacher>("T002", "冯玉");
    auto teacher3 = make_unique<Teacher>("T003", "成临");

    // 创建课程并关联教师
    auto course1 = make_unique<Course>("CS101", "C Programming", 3, teacher1.get(), 30);
    auto course2 = make_unique<Course>("CS201", "Data structure", 4, teacher2.get(), 30);
    auto course3 = make_unique<Course>("MATH101", "Advanced Math", 5, teacher3.get(), 25);

    // 为教师设置课程
    teacher1->setCourse(course1.get());
    teacher2->setCourse(course2.get());
    teacher3->setCourse(course3.get());

    // 存储课程
    _courses.push_back(std::move(course1));
    _courses.push_back(std::move(course2));
    _courses.push_back(std::move(course3));

    // 存储教师
    _teachers.push_back(std::move(teacher1));
    _teachers.push_back(std::move(teacher2));
    _teachers.push_back(std::move(teacher3));

    // 秘书
    _sectary.push_back(make_unique<Sectary>(*this, "S001", "王秘书"));

    print("初始化完成.\n");
}

int Registrar::exec() {
    return m_ui->exec();
}

Student *Registrar::findStudentById(const string &id) {
    for (auto& student : _students) {
        if (student->hasId(id))
            return student.get();
    }
    return nullptr;
}

Course *Registrar::findCourseById(const string &id) {
    for (auto& course : _courses) {
        if (course->hasId(id))
            return course.get();
    }
    return nullptr;
}

Sectary *Registrar::findSectaryById(const string &id) {
    for (auto& sectary : _sectary) {
        if (sectary->hasId(id))
            return sectary.get();
    }
    return nullptr;
}

Teacher *Registrar::findTeacherById(const string &id) {
    for (auto& teacher : _teachers) {
        if (teacher->hasId(id))
            return teacher.get();
    }
    return nullptr;
}

void Registrar::assignCourseToTeacher(Course* course, Teacher* teacher) {
    teacher->setCourse(course);
}
