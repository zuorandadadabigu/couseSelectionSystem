export module registrar:ui;
//ui交互类，负责菜单显示和用户输入
import std;
import registrar;
import :sectary;

using std::print;

export class Ui{
public:
    Ui(Registrar& registrar);
    //获取用户输入
    int getInput();
    //输出总菜单
    void prompt();
    //根据不同身份输出不同的功能菜单
    void prompt_student();
    void prompt_teacher();
    void prompt_sectary();
    //根据不同身份的菜单执行用户选择的功能
    void service_student();
    void service_teacher();
    void service_sectary();
    //主流程
    int exec();
private:
    Registrar& m_registrar;
    int identifyconst(string& input);
};

//===========================================基本功能=========================================
//构造函数
Ui::Ui(Registrar& registrar)
    :m_registrar(registrar)
{}

//接收用户输入
int Ui::getInput(){
        int cmd;
        std::cin >> cmd;
        return cmd;
}
//主流程
int Ui::exec(){
    int s = -1;
    while(true){
        //输出主菜单
        prompt();

        //接收用户输入
        s = getInput();

        //执行服务
        if(s == 0){
            print("系统退出，感谢使用。\n");
            break;
        }else if(s == 1){
            print("请输入学号/职工号/教工号：\n");
            string input;
            std::cin >> input;
            int i = identify(input);
            identifyService(i,input); //还要添加一个循环结束标志，返回上一级菜单
        } else {
            print（"无效输入，请再试一次:\n");
        }
    }
    return 0;
}

//判断身份
int Ui::identify(const string& input){
    if(input.empty())
        return 0;
    char firstChar = input[0];
    if(firstChar == 'T' || firstChar == 't'){       //老师教工号开头为T
        return 1;
    }else if(firstChar == 'S' || firstChar == 's'){ //秘书职工号为S
        return 2;
    }else if(std::isdigit(firstChar)){              //学生学号为纯数字
        return 3;
    }
    return 0;
}

//分配服务
void Ui::identifyService(int i,const string input)
{
    switch(i){
        case 1:
            //教师服务
            const Teacher* t = m_registrar.findTeacherById(input);
            if(t){
                prompt_teacher();
                service_teacher();
            }else{
                print("此用户不存在.\n");
            }
            break;
        case 2:
            //秘书服务
            const Sectary* a = m_registrar.findSectaryById(input);
            if(a){
                prompt_sectary();
                service_sectary();
            }else{
                print("此用户不存在.\n");
            }
            break;
        case 3:
            //学生服务
            const Student* s = m_registrar.findStudentById(input);
            if(s){
                prompt_student();
                service_student();
            }else{
                print("此用户不存在.\n");
            }
            break;
        default:
            print("请输入有效ID\n");
            break;
    }
}
//==================================================菜单=============================================================
void Ui::prompt(){
    print("欢迎来到选课系统，请输入您所想要的服务对应数字:\n");
    print("1.登陆账号\n");
    print("0.退出系统\n");
}

void Ui::prompt_student(Student* student){
    print("Hello,{}! Please choose the service which you want:\n",student.name);
    print("1.查看待选课程\n");
    print("2.选课\n");
    print("3.退课\n");
    print("4.查看课表\n");
    print("0.退出登陆\n");
}

void Ui::prompt_teacher(Teacher* teacher){
    print("Hello,{}! Please choose the service which you want:\n",teacher.name);
    print("1.查看授课课程学生信息\n");
    print("2.上传成绩\n");
    print("3.查看学生成绩\n");
    print("0.退出登陆\n");
}

void Ui::prompt_sectary(Sectary* sectary){
    print("Hello,{}! Please choose the service which you want:\n",sectary.name);
    print("1.展示所有学生信息\n");
    print("2.展示所有课程信息\n");
    print("3.修改课容量\n");
    print("0.退出登陆\n");
}
//=========================================================服务============================================
void Ui::service_student(){
    int service = getInput;
    switch(service){
        case 1:
            //查看待选课程
            break;
        case 2:
            //选课
            break;
        case 3:
            //退课
            break;
        case 4:
            //查看课表
            break;
    }
}

void Ui::service_teacher(){
    int service = getInput;
    switch(service){
        case 1:
            //查看授课课程学生信息
            break;
        case 2:
            //上传成绩
            break;
        case 3:
            //查看学生成绩
            break;
    }
}

void Ui::service_sectary(){
    int service = getInput();
    switch(service){
        case 1:
            //展示所有学生信息
            sectary.showAllStudent();
            break;
        case 2:
            //展示所有课程信息
            sectary.showAllCourse();
            break;
        case 3:
            //修改课容量
            sectary.adjustCapacity();
            break;
    }
}

