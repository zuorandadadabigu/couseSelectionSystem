// Module registrar:teacher -- Class Teacher
// File: registrar.teacher.cppm   Version: 0.1.0   License: AGPLv3
// Created: zhouyuxin      2026-01-17
// Description:To realize the function of teachers.
//
export module registrar:teacher;
import std;
import :student;
import :course;

using std::string;
using std::print;
using std::cin;

export class Teacher {
public:
    Teacher(const string& id, const string& name);
    bool hasId(const string& id);

    void showCourseInfo() const;
    void showCourseStudents() const;
    void addCourseGrade(Student& student, float grade);

private:
    string m_id;
    string m_name;
    Course* m_course = nullptr;
};

Teacher::Teacher(const string& id, const string& name)
    : m_id(id), m_name(name), m_course(nullptr)
{}

void Teacher::showCourseInfo() const {
    if (!m_course) {
        print("暂无授课课程\n");
        return;
    }
    print("教师: {}\n课程信息:\n", m_name);
    print("课程号: {}  课程名: {}  学分: {}  容量: {}/{}\n",
        m_course->m_id, m_course->m_name, m_course->m_credit,
        m_course->_students.size(), m_course->m_maximum;
}

void Teacher::showCourseStudents() const {
    if (!m_course) {
        print("暂无授课课程\n");
        return;
    }
    print("已选课程《{}》的学生:\n", m_course->m_name());
    m_course->showStudents();
}

void Teacher::addCourseGrade(Student& student, float grade) {
    if (!m_course) {
        print("暂无授课课程\n");
        return;
    }

    if (grade < 0 || grade > 100) {
        print("成绩必须在0-100之间\n");
        return;
    }

    m_course->setStudentGrade(&student, grade);
    print("教师{}为学生{}的课程《{}》录入成绩: {}\n",
        m_name, student.m_name(), m_course->m_name(), grade);
    Registrar::getInstance().saveStudentGrade(student.m_id,m_course->m_id,grade);
}

bool Teacher::hasId(string id)
{
    return id == m_id;
}
