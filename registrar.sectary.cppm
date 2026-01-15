export module registrar:sectary;

export class Student{
public:
    initialMessage();//初始所有信息，包括学生、课程、教师
    showAllStudent();//展示所有学生信息
    showAllCourse();//展示所有课程信息
    adjustCourseCapacity();//修改课容量
private:
    int m_id;//秘书号
    string m_name;//名字
};
