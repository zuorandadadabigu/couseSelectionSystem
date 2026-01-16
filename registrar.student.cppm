// File: student.cppm  Version: 1.0
// Created: Yuxin Zhou   1946296724@qq.com   2026-1-16
// Description:
// Change Log:
//     [v0.1.1] Yuxin Zhou 1946296724@qq.com  2026-1-16

export module registrar:student; // 关联到Student模块
import course;

private:
    std::string name;
    double credit;
    int currentStudentNumber;
    int maximum;

public:
    std::string getName() const { return name; }
    double getCredit() const { return credit; }
    int getCurrentStudentNumber() const { return currentStudentNumber; }
    int getMaximum() const { return maximum; }
    bool addStudent()
    bool removeStudent()
};

bool addStudent() {
    if (currentStudentNumber < maximum) {
        currentStudentNumber++;
        return true;
    }
    return false;
}
bool removeStudent() {
    if (currentStudentNumber > 0) {
        currentStudentNumber--;
        return true;
    }
    return false;
}

void Student::addCourse(Course& course) {
    const std::string courseName = course.getName();
    if (courseGrade.find(courseName) == courseGrade.end()) {
        if (course.addStudent()) {
            courseGrade[courseName] = 0;
            currentCredit += course.getCredit();
            print("{} 成功选课程：{}，当前学分：{:.1f}", name, courseName, currentCredit);
        } else {
            print("{} 选课失败：{} 已达最大容量！", name, courseName);
        }
    } else {
        print("{} 选课失败：已选过课程 {}！", name, courseName);
    }
}

// 退课方法
void Student::cancelCourse(Course& course) {
    const std::string courseName = course.getName();
    auto it = courseGrade.find(courseName);
    if (it != courseGrade.end()) {
        if (course.removeStudent()) {
            currentCredit -= course.getCredit();
            courseGrade.erase(it);
            print("{} 成功退课：{}，当前学分：{:.1f}", name, courseName, currentCredit);
        } else {
            print("{} 退课失败：课程 {} 无学生可移除！", name, courseName);
        }
    } else {
        print("{} 退课失败：未选过课程 {}！", name, courseName);
    }
}

// 展示已选课
void Student::showSelectedCourse() const {
    print("\n【{}（学号：{}）已选课程】", name, id);
    if (courseGrade.empty()) {
        print("暂无已选课程");
        return;
    }
    for (const auto& [courseName, grade] : courseGrade) {
        std::string gradeStr = (grade == 0) ? "未录入" : std::to_string(grade);
        print("课程名：{} | 成绩：{}", courseName, gradeStr);
    }
    print("当前总学分：{:.1f}", currentCredit);
}

// 展示可选课
void Student::showAvailableCourse(const std::vector<Course>& allCourses) const {
    print("\n【{}（学号：{}）可选课程】", name, id);
    bool hasAvailable = false;
    const auto selectedCourses = getSelectedCourseNames();
    for (const auto& course : allCourses) {
        if (std::find(selectedCourses.begin(), selectedCourses.end(), course.getName()) == selectedCourses.end() &&
            course.getCurrentStudentNumber() < course.getMaximum()) {
            int remaining = course.getMaximum() - course.getCurrentStudentNumber();
            print("课程名：{} | 学分：{:.1f} | 剩余容量：{}",
                  course.getName(), course.getCredit(), remaining);
            hasAvailable = true;
        }
    }
    if (!hasAvailable) {
        print("暂无可选课程");
    }
}

