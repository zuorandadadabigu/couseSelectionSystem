export module registrar:student;

export class Student{
public:
    addCourse();//选课
    removeCourse();//退课
    showCourse();//展示课表
    showAvaliableCourse();//展示待选课程
private:
    int m_id;//学号
    string m_name;//名字
    int m_grade;//成绩
    int m_credit;//学分
};

