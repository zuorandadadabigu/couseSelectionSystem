// Module registrar:sectary -- Class Sectary
// File: registrar.sectary.cppm   Version: 0.1.0   License: AGPLv3
// Created:mingyatong       2026-01-17
// Description:To realize the function of sectary.
//
export module registrar:sectary;
import std;
import registrar;
import :course;
import :student;

using std::print;
using std::string;
using std::cin;

export class Sectary {
public:
    Sectary(Registrar& registrar, const string& id, const string& name);

    const string& getName() const { return m_name; }
    bool hasId(const string& id) const { return id == m_id; }

    void showAllStudent();
    void showAllCourse();
    void adjustCourseCapacity();

private:
    string m_id;
    string m_name;
    Registrar& m_registrar;
};

Sectary::Sectary(Registrar& registrar, const string& id, const string& name)
    : m_id(id), m_name(name), m_registrar(registrar)
{}

void Sectary::showAllStudent() {
    print("\n所有学生信息:\n");
    print("学号     姓名\n");
    for (const auto& student : m_registrar.getStudents()) {
        student->showInfo();
    }
}

void Sectary::showAllCourse() {
    print("\n所有课程信息:\n");
    for (const auto& course : m_registrar.getCourses()) {
        course->showInfo();
    }
}

void Sectary::adjustCourseCapacity() {
    showAllCourse();
    print("\n请输入你要修改课容量的课程的课程号: ");
    string courseId;
    cin >> courseId;

    Course* course = m_registrar.findCourseById(courseId);
    if (course) {
        print("当前课容量: {}，已选人数: {}，请输入新容量: ",
            course->getMaximum(), course->getStudentCount());
        int newCapacity;
        cin >> newCapacity;
        course->changeCapacityTo(newCapacity);
    } else {
        print("未找到该课程.\n");
    }
}
