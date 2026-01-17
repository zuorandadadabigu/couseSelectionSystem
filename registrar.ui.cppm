// Module registrar:ui
// File: registrar.ui.cppm   Version: 0.1.0   License: AGPLv3
// Created:       2026-01-17 11:26:34
// Description:
//
export module registrar:ui;
import std;
import registrar;
import :sectary;
import :teacher;
import :student;
import :course;

using std::print;
using std::string;

export class Ui {
public:
    Ui(Registrar& registrar);
    // 获取用户输入
    int getInput();
    // 输出总菜单
    void prompt();
    // 根据不同身份输出不同的功能菜单
    void prompt_student(Student* student);
    void prompt_teacher(Teacher* teacher);
    void prompt_sectary(Sectary* sectary);
    // 根据不同身份的菜单执行用户选择的功能
    void service_student(Student* student);
    void service_teacher(Teacher* teacher);
    void service_sectary(Sectary* sectary);
    // 主流程
    int exec();

private:
    Registrar& m_registrar;
    int identify(const string& input);
    void identifyService(int i, const string& input);
};

Ui::Ui(Registrar& registrar)
    : m_registrar(registrar)
{}

int Ui::getInput() {
    int cmd;
    std::cin >> cmd;
    return cmd;
}

int Ui::exec() {
    while (true) {
        prompt();
        int s = getInput();

        if (s == 0) {
            print("系统退出，感谢使用。\n");
            break;
        } else if (s == 1) {
            print("请输入学号/职工号/教工号：\n");
            string input;
            std::cin >> input;
            int i = identify(input);
            identifyService(i, input);
        } else {
            print("无效输入，请再试一次:\n");
        }
    }
    return 0;
}

int Ui::identify(const string& input) {
    if (input.empty())
        return 0;
    char firstChar = input[0];
    if (firstChar == 'T' || firstChar == 't') {
        return 1;
    } else if (firstChar == 'S' || firstChar == 's') {
        return 2;
    } else if (std::isdigit(firstChar)) {
        return 3;
    }
    return 0;
}

void Ui::identifyService(int i, const string& input) {
    switch(i) {
        case 1: {
            Teacher* t = m_registrar.findTeacherById(input);
            if (t) {
                while (true) {
                    prompt_teacher(t);
                    int choice = getInput();
                    if (choice == 0) break;
                    service_teacher(t, choice);
                }
            } else {
                print("此用户不存在.\n");
            }
            break;
        }
        case 2: {
            Sectary* a = m_registrar.findSectaryById(input);
            if (a) {
                while (true) {
                    prompt_sectary(a);
                    int choice = getInput();
                    if (choice == 0) break;
                    service_sectary(a, choice);
                }
            } else {
                print("此用户不存在.\n");
            }
            break;
        }
        case 3: {
            Student* s = m_registrar.findStudentById(input);
            if (s) {
                while (true) {
                    prompt_student(s);
                    int choice = getInput();
                    if (choice == 0) break;
                    service_student(s, choice);
                }
            } else {
                print("此用户不存在.\n");
            }
            break;
        }
        default:
            print("请输入有效ID\n");
            break;
    }
}

void Ui::prompt() {
    print("欢迎来到选课系统，请输入您所想要的服务对应数字:\n");
    print("1.登陆账号\n");
    print("0.退出系统\n");
}

void Ui::prompt_student(Student* student) {
    print("\nHello, {}! Please choose the service which you want:\n", student->getName());
    print("1.查看待选课程\n");
    print("2.选课\n");
    print("3.退课\n");
    print("4.查看课表\n");
    print("0.退出登陆\n");
}

void Ui::prompt_teacher(Teacher* teacher) {
    print("\nHello, {}! Please choose the service which you want:\n", teacher->getName());
    print("1.查看授课课程信息\n");
    print("2.查看已选课学生\n");
    print("3.为学生录入成绩\n");
    print("0.退出登陆\n");
}

void Ui::prompt_sectary(Sectary* sectary) {
    print("\nHello, {}! Please choose the service which you want:\n", sectary->getName());
    print("1.展示所有学生信息\n");
    print("2.展示所有课程信息\n");
    print("3.修改课容量\n");
    print("0.退出登陆\n");
}

void Ui::service_student(Student* student, int choice) {
    switch(choice) {
        case 1: {
            print("\n待选课程列表:\n");
            for (const auto& course : m_registrar._courses) {
                print("课程号: {}  课程名: {}  学分: {}  教师: {}  已选人数: {}/{}\n",
                    course->m_id, course->m_name, course->m_credit,
                    course->m_teacher, course->_students.size(), course->m_maximum);
            }
            break;
        }
        case 2: {
            print("请输入课程号选课: ");
            string courseId;
            std::cin >> courseId;
            Course* course = m_registrar.findCourseById(courseId);
            if (course) {
                if (student->addCourse(course)) {
                    print("选课成功!\n");
                } else {
                    print("选课失败!可能已达人数上限或已选此课程。\n");
                }
            } else {
                print("课程不存在!\n");
            }
            break;
        }
        case 3: {
            print("请输入课程号退课: ");
            string courseId;
            std::cin >> courseId;
            Course* course = m_registrar.findCourseById(courseId);
            if (course) {
                student->removeCourse(course);
                print("退课成功!\n");
            } else {
                print("课程不存在!\n");
            }
            break;
        }
        case 4: {
            student->showCourses();
            break;
        }
        default:
            print("无效选择\n");
    }
}

void Ui::service_teacher(Teacher* teacher, int choice) {
    switch(choice) {
        case 1:
            teacher->showCourseInfo();
            break;
        case 2:
            teacher->showCourseStudents();
            break;
        case 3: {
            print("请输入学生学号: ");
            string studentId;
            std::cin >> studentId;
            Student* student = m_registrar.findStudentById(studentId);
            if (student) {
                print("请输入成绩: ");
                float grade;
                std::cin >> grade;
                teacher->addCourseGrade(*student, grade);
            } else {
                print("学生不存在!\n");
            }
            break;
        }
        default:
            print("无效选择\n");
    }
}

void Ui::service_sectary(Sectary* sectary, int choice) {
    switch(choice) {
        case 1:
            sectary->showAllStudent();
            break;
        case 2:
            sectary->showAllCourse();
            break;
        case 3:
            sectary->adjustCourseCapacity();
            break;
        default:
            print("无效选择\n");
    }
}
