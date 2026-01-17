// Module registrar:course --Class Course
// File: registrar.course.cppm   Version: 0.1.0   License: AGPLv3
// Created: zhouyuxin      2026-01-17
// Description: To realize the function of courses.
//
export module registrar:course;
import std;
import :student;
import :teacher;

using std::print;
using std::vector;
using std::string;
using std::format;

export class Course {
public:
    Course(const string& id, const string& name, int credit, Teacher* teacher, int maximum);

    const string& getTeacherName() const;
    bool hasId(const string& id);
    bool addStudent(Student* student);
    void removeStudent(Student* student);
    void showStudents() const;
    void setStudentGrade(Student* student, float grade);
    void showInfo() const;

    void changeCapacityTo(int newCapacity);

private:
    string m_id;
    string m_name;
    int m_credit;
    Teacher* m_teacher;
    int m_maximum;
    vector<Student*> _students;
    vector<std::pair<Student*, float>> _grades; // 学生-成绩对
    void changeCapacity(int newCapacity);
};

Course::Course(const string& id, const string& name, int credit, Teacher* teacher, int maximum)
    : m_id(id), m_name(name), m_credit(credit), m_teacher(teacher), m_maximum(maximum)
{}

const string& Course::getTeacherName() const {
    static string empty;
    return m_teacher ? m_teacher->getName() : empty;
}

bool Course::addStudent(Student* student) {
    if (_students.size() >= m_maximum) {
        print("课程《{}》已达人数上限!\n", m_name);
        return false;
    }

    // 检查是否已选
    for (auto s : _students) {
        if (s == student) {
            print("学生{}已选此课程!\n", student->m_name);
            return false;
        }
    }

    _students.push_back(student);
    print("学生{}成功选择课程《{}》\n", student->m_name, m_name);
    return true;
}

void Course::removeStudent(Student* student) {
    for (auto it = _students.begin(); it != _students.end(); ++it) {
        if (*it == student) {
            _students.erase(it);
            print("学生{}已从课程《{}》中移除\n", student->m_name, m_name);

            // 同时移除成绩
            for (auto g = _grades.begin(); g != _grades.end(); ++g) {
                if (g->first == student) {
                    _grades.erase(g);
                    break;
                }
            }
            break;
        }
    }
}

void Course::showStudents() const {
    if (_students.empty()) {
        print("暂无学生选课\n");
        return;
    }

    for (auto student : _students) {
        print("学号: {}  姓名: {}\n", student->m_id(), student->m_name);
    }
}

void Course::setStudentGrade(Student* student, float grade) {
    for (auto& g : _grades) {
        if (g.first == student) {
            g.second = grade;
            student->addGrade(m_id, grade);
            return;
        }
    }
    _grades.emplace_back(student, grade);
    student->addGrade(m_id, grade);
}

void Course::showInfo() const {
    print("{}   {}  学分:{}  教师:{}  容量:{}/{}\n",
        m_id, m_name, m_credit, m_teacher, _students.size(), m_maximum);
}

void Course::changeCapacityTo(int newCapacity) {
    if (newCapacity <= 0) {
        print("课容量必须为正数\n");
        return;
    }
    changeCapacity(newCapacity);
    print("课程《{}》容量已修改为{}\n", m_name, newCapacity);
}

bool Course::hasId(string id){
    return id == m_id;
}

void Course::changeCapacity(int newCapacity){
    m_maximum = newCapacity;
}
