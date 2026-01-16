// Module partition teacher : define teacher class
// File: teacher.cppm  Version: 1.0
// Created: Yuxin Zhou   1946296724@qq.com   2026-1-16
// Description:
// Change Log:
//     [v0.1.1] Yuxin Zhou 1946296s724@qq.com  2026-1-16

export module registrar:teacher;
import course;
import student;

// 私有属性id/name，公有方法
export class Teacher {
private:
    int id;
    std::string name;

public:
    // 构造函数（初始化id和name）
    Teacher(int teacherId, std::string_view teacherName)
        : id(teacherId), name(teacherName) {}

    // 展示课程信息
    void courseMessage(const Course& course) const;

    // 为学生添加课程成绩
    void addCourseGrade(Student& student, const Course& course, float grade);

    // 展示课程成绩
    void showCourseGrade(const Student& student, const Course& course) const;

    int getId() const { return id; }
    const std::string& getName() const { return name; }
};

// 实现courseMessage：使用print函数输出课程信息
void Teacher::courseMessage(const Course& course) const {
    print("【教师{}】课程信息：名称={} 学分={} 容量={}",
          name, course.getName(), course.getCredit(), course.getMaximum());
}

// 实现addCourseGrade：使用print函数输出成绩录入信息
void Teacher::addCourseGrade(Student& student, const Course& course, float grade) {
    if (grade < 0.0f || grade > 100.0f) {
        throw std::invalid_argument("成绩必须在0-100之间");
    }
    student.setCourseGrade(course.getId(), grade);
    print("【教师{}】为学生{}的课程《{}》录入成绩：{}",
          name, student.getName(), course.getName(), grade);
}

// 实现showCourseGrade：使用print函数输出成绩展示信息
void Teacher::showCourseGrade(const Student& student, const Course& course) const {
    try {
        float grade = student.getCourseGrade(course.getId());
        print("【教师{}】学生{}的课程《{}》成绩：{}",
              name, student.getName(), course.getName(), grade);
    } catch (const std::out_of_range& e) {
        print("【教师{}】学生{}未修读课程《{}》",
              name, student.getName(), course.getName());
    }
}




