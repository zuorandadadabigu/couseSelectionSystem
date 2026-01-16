export module registrar:sectary;

import std;
import registrar;

using std::vector;
using std::print;
using std::string;

export class Sectary{
public:
    Sectray(Registrar& registrar,string id,string name);
    bool hasId(string id);
    void showAllStudent();//展示所有学生信息
    void showAllCourse();//展示所有课程信息
    void adjustCourseCapacity();//修改课容量
private:
    int m_id;//秘书号
    string m_name;//名字
    Registrar& m_registrar;
};

Sectray(Registrar& registrar,string id,string name)
    :m_id(id)
    ,m_name(name)
    ,m_registrar(registrar)
{}



void Sectary::adjustCourseCapacity(){
    showAllCourse();
    print("请输入你要修改课容量的课程的课程号:\n");
    string courseId;
    cin >> courseId;
    for(const auto& course : registrar._courses){
        if(course->hasId(courseId)){
            int newCapacity;
            print("当前课容量：{}，请输入新容量：",course->maximum);
            cin >> newCapacity;
            if(newCapacity > 0){
                course->maximum = newCapacity;
                print("课程{}课容量已修改为{}\n",course->name,newCapacity);
            }else {
                print("课容量须为正数\n");
            }
            return;
        }
    }
    print("未找到该课程.\n");
}


void Sectary::showAllStudent(){
    print("学号          学生姓名\n");
    for(const auto& student : registrar._students){
        student->info();
    }
    return;
}

void Sectary::showAllCourse(){
    print("课程号          课程名\n");
    for(const auto& course : registrar._courses){
        course->info();
    }
    return;
}

bool Sectary::hasId(string id)
{
    return id == m_id;
}

