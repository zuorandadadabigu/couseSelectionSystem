export module registrar:teacher;
import std;

using std::string;
using std::print;

export class Teacher {
public:
    Teacher(int id, string name,Course* course);
    //展示课程信息
    void courseMessage() const;
    //为学生添加课程成绩
    void addCourseGrade(Student& student, float grade);
    //展示课程成绩
    void showCourseGrade(const Student& student) const;

private:
    int m_id;
    string m_name;
    Course* m_course;
};

Teacher::Teacher(int id, string name,string course)
    : m_id(id)
    , m_name(name)
    , m_course(course)
{}

void Teacher::courseMessage() const {
    print("教师:{}\n课程名称:{} 学分:{} 容量:{}\n",m_name, m_course->m_name(), m_course.m_credit(), m_course.m_maximum());
}

void Teacher::addCourseGrade(Student& student, float grade) {
    if (grade < 0 || grade > 100) {
        print("成绩必须在0-100之间\n");
        return;
    }
  //  student.m_grade=用私密函数
    print("教师{}为学生{}的课程《{}》录入成绩：{}",m_name, student.name, m_course.name, grade);
}

/*void Teacher::showCourseGrade(const Student& student) const {
    if (!m_course) {
        std::print("教师{}暂无授课课程\n");
        return;
    }

    auto it = student.m_grades.find(m_course->m_id);

    if (it == student.m_grades.end()) {
        std::print("学生{}的课程《{}》暂无成绩\n",
            student.m_name, m_course->m_name);
    } else {
        std::print("学生{}的课程《{}》成绩为：{}\n",
            student.m_name, m_course->m_name, it->second);
    }
}
*/



