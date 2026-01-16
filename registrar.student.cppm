export module registrar:student;

using std::vector;
using std::string;

export class Student{
public:
    Student(string id,string name,int grade,int credit);
    void addCourse(class Course* course);//选课
    void removeCourse();//退课
    string showCourse();//展示课表
    string showAvaliableCourse();//展示待选课程
    string info();
    bool hasId(string id);
private:
    string m_id;//学号
    string m_name;//名字
    int m_grade;//成绩
    int m_credit;//学分

    vector<class Course*> _courses;
};

Student::Student(string id, string name)
    : m_name(name)
    , m_id(id)
    ,m_grade(grade)
    ,m_credit(credit)
{}


string Student::info()
{
    return format("{}   {}\n", m_id, m_name);
}

bool Student::hasId(string id)
{
    return id == m_id;
}
