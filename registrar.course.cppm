export module registrar:course;

import std;
using std::print;
using std::vector;
using std::string;

export class Course{
public:
    Course(string id, string name,int credit,string teacher,int maximum,int totalCount);
    bool addStudent(class Student* student);//被选课
    removeStudent();//被退课
    showStudent();//展示已选课学生信息
    showGrade();//展示学生成绩表
    bool hasId(string id);
    string roster();
    string info();
private:
    string m_id;//课程号
    string m_name;//课程名
    int m_credit;//课程学分
    string m_teacher;//任课老师
    int m_maximum;//最大选课人数
    vector<class Student*> _students;
    //成绩
    static int cm_totalCount;//当前选课人数
};

int Course::cm_totalCount = 0; // initialize static data memeber

Course::Course(string id, string name)
    : m_name(name)
    , m_id(id)
    , m_credit(credit)
    , m_teacher(teacher)
    , m_maximum(maximum)
{
    cm_totalCount++;
}

bool Course::addStudent(Student *student){
    if(_students.size() < 80){  // 假定某个课程的最大人数为80
        _students.push_back(student);
        print("\"{}\" 选课成功！目前选择该课程的人数: {}\n",
              m_name, _students.size());
        return true;
    }
    return false;
}

string Course::info(){
    return format("{}   {}\n", m_id, m_name);
}
//getCapacity

bool Course::hasId(string id){
    return id == m_id;
}
