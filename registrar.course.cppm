export module registrar:course;

export class Course{
public:
    addStudent();//被选课
    removeStudent();//被退课
    showStudent();//展示已选课学生信息
    showGrade();//展示学生成绩表
private:
    int m_id;//课程号
    string m_name;//课程名
    int m_credit;//课程学分
    string m_teacher;//任课老师
    int m_maximum;//最大选课人数
    int m_currentStudentNumber;//当前选课人数
    //成绩
};
