export module registrar:teacher;

export class Teacher{
public:
    courseMessage();//展示自己教的课的学生信息
    addGrade();//上传学生成绩
    showGrade();//展示学生成绩
private:
    int m_id;//教工号
    string m_name;//名字
};
