export module registrar:student;
import std;
import :course;

using std::vector;
using std::string;
using std::print;
using std::format;

export class Student {
public:
    Student(const string& id, const string& name);

    bool hasId(const string& id);
    bool addCourse(Course* course);
    void removeCourse(Course* course);
    void showCourses() const;
    void showInfo() const;

    void addGrade(const string& courseId, float grade);
    float getGrade(const string& courseId) const;
    void showGrades() const;

private:
    string m_id;
    string m_name;
    vector<Course*> _courses;
    vector<std::pair<string, float>> _grades; // 课程ID-成绩对
};

Student::Student(const string& id, const string& name)
    : m_id(id), m_name(name)
{}

bool Student::addCourse(Course* course) {
    // 检查是否已选此课程
    for (auto c : _courses) {
        if (c == course) {
            return false;
        }
    }

    if (course->addStudent(this)) {
        _courses.push_back(course);
        return true;
    }
    return false;
}

void Student::removeCourse(Course* course) {
    for (auto it = _courses.begin(); it != _courses.end(); ++it) {
        if (*it == course) {
            course->removeStudent(this);
            _courses.erase(it);
            break;
        }
    }
}

void Student::showCourses() const {
    if (_courses.empty()) {
        print("暂无课程\n");
        return;
    }

    print("学生{}的课程表:\n", m_name);
    for (auto course : _courses) {
        print("课程号: {}  课程名: {}  学分: {}  教师: {}\n",
            course->m_id, course->m_name, course->m_credit, course->m_teacher);
    }
}

void Student::showInfo() const {
    print("{}   {}\n", m_id, m_name);
}

void Student::addGrade(const string& courseId, float grade) {
    for (auto& g : _grades) {
        if (g.first == courseId) {
            g.second = grade;
            return;
        }
    }
    m_grades.emplace_back(courseId, grade);
}

float Student::getGrade(const string& courseId) const {
    for (const auto& g : _grades) {
        if (g.first == courseId) {
            return g.second;
        }
    }
    return -1; // 没有成绩
}

void Student::showGrades() const {
    if (_grades.empty()) {
        print("暂无成绩\n");
        return;
    }

    print("学生{}的成绩单:\n", m_name);
    for (const auto& g : _grades) {
        print("课程号: {}  成绩: {}\n", g.first, g.second);
    }
}

bool Student::hasId(string id)
{
    return id == m_id;
}
