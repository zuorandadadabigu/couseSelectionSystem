// Module registrar:database
// File: registrar.database.cppm   Version: 0.1.0   License: AGPLv3
// Created: zhouyuxin      2026-01-17
// Description:To realize the function of database.
//

export module registrar:database;

import std;
import :base;
import :registrar;

using std::string;
using std::vector;
using std::unique_ptr;
using std::print;
using std::format;

constexpr auto DB_HOST = "127.0.0.1";
constexpr auto DB_PORT = "5432";
constexpr auto DB_NAME = "course_selection";
constexpr auto DB_USER = ;
constexpr auto DB_PASS = ;

export class Database {
public:
    static Database& getInstance();

    Database(const Database&) = delete;
    Database(Database&&) = delete;
    Database& operator=(const Database&) = delete;
    Database& operator=(Database&&) = delete;
    // ========== 学生 ==========
    bool saveStudent(Student* student);// 保存学生
    unique_ptr<Student> loadStudent(const string& studentId);// 加载单个学生
    vector<unique_ptr<Student>> loadAllStudents();// 加载所有学生
    // ========== 教师 ==========
    bool saveTeacher(Teacher* teacher);// 保存教师
    bool saveStudentGrade(const string& studentId, const string& courseId, float grade);// 保存教师录入的成绩
    unique_ptr<Teacher> loadTeacher(const string& teacherId);// 加载单个教师
    vector<unique_ptr<Teacher>> loadAllTeachers();// 加载所有教师
    // ========== 课程 ==========
   bool saveCourse(Course* course);// 保存课程
   bool updateCourseCapacity(const string& courseId, int newCapacity);// 更新课程容量
   vector<unique_ptr<Course>> loadAllCourses();// 加载所有课程
    // ========== 秘书 ==========
    bool saveSecretary(Secretary* secretary);// 保存秘书
    unique_ptr<Secretary> loadSecretary(const string& secretaryId);// 加载秘书
    // ========== 通用工具 ==========
    bool isConnected();
private:
    Database();
    ~Database();
    void loadCourseStudents(Course* course);
    pqxx::connection _conn; // PostgreSQL连接
};

static Database::Database& getInstance() {
    static Database instance;
    return instance;
}

Database::Database() {
    try {
        string connStr = format(
            "host={} port={} dbname={} user={} password={}",
            DB_HOST, DB_PORT, DB_NAME, DB_USER, DB_PASS
        );
        _conn = pqxx::connection{connStr};
        if (_conn.is_open()) {
            print("[DB] 连接成功: {}\n", _conn.dbname());
        } else {
            throw std::runtime_error("连接失败");
        }
    } catch (const std::exception& e) {
        print("[DB] 初始化异常: {}\n", e.what());
        std::exit(1);
    }
}

Database::~Database() {
    if (_conn.is_open()) {
        _conn.close();
        print("[DB] 连接关闭\n");
    }
}

bool Database::saveStudent(Student* student) {
    if (!student)
        return false;
    try {
        pqxx::work txn{_conn};
        //保存学生信息
        string sqlStudent = format(
            R"(INSERT INTO students (id, name)
               VALUES ('{}', '{}')
               ON CONFLICT (id) DO UPDATE SET name = '{}')",
            txn.esc(student->m_id), txn.esc(student->m_name), txn.esc(student->m_name)
        );
        txn.exec(sqlStudent);
        string sqlDelSC = format(
            "DELETE FROM student_course WHERE student_id = '{}'",
            txn.esc(student->m_id)
        );
        txn.exec(sqlDelSC);

        //重新插入选课+成绩
        for (const auto& course : student->_courses) {
            float grade = student->getGrade(course->m_id);
            string sqlSC = format(
                R"(INSERT INTO student_course (student_id, course_id, grade)
                   VALUES ('{}', '{}', {})
                   ON CONFLICT (student_id, course_id) DO UPDATE SET grade = {})",
                txn.esc(student->m_id), txn.esc(course->m_id), grade, grade
            );
            txn.exec(sqlSC);
        }

        txn.commit();
        print("插入学生成功\n", student->m_id);
        return true;
    } catch (const pqxx::sql_error& e) {
        print("插入学生失败: {}\nSQL: {}\n", e.what(), e.query());
        return false;
    }
}

unique_ptr<Student> Database::loadStudent(const string& studentId) {
    pqxx::read_transaction txn{_conn};
    string sql = format(
        "SELECT id, name FROM students WHERE id = '{}'",
        txn.esc(studentId)
    );
    pqxx::result res = txn.exec(sql);
    if (res.empty()) return nullptr;

    auto student = std::make_unique<Student>(
        res[0]["id"].as<string>(),
        res[0]["name"].as<string>()
    );

    // 加载选课和成绩
    string sqlSC = format(
        R"(SELECT c.id, c.name, c.credit, c.maximum, t.id as teacher_id, t.name as teacher_name, sc.grade
            FROM student_course sc
           JOIN courses c ON sc.course_id = c.id
           LEFT JOIN teachers t ON c.teacher_id = t.id
           WHERE sc.student_id = '{}')",
        txn.esc(studentId)
    );
    pqxx::result resSC = txn.exec(sqlSC);

    auto& registrar = Registrar::getInstance();
    for (const auto& row : resSC) {
        // 新建教师
        unique_ptr<Teacher> teacher = nullptr;
        if (!row["teacher_id"].is_null()) {
            teacher = std::make_unique<Teacher>(
                row["teacher_id"].as<string>(),
                row["teacher_name"].as<string>()
            );
            registrar.addTeacher(std::move(teacher));
        }

        // 新建课程
        auto course = std::make_unique<Course>(
            row["id"].as<string>(),
            row["name"].as<string>(),
            row["credit"].as<int>(),
            teacher.get(),
            row["maximum"].as<int>()
        );
        registrar.addCourse(std::move(course));

        // 关联学生
        student->addCourse(course.get());
        student->addGrade(course->m_id, row["grade"].as<float>());
    }

    txn.commit();
    return student;
}

vector<unique_ptr<Student>> Database::loadAllStudents() {
    vector<unique_ptr<Student>> students;
    try {
        pqxx::read_transaction txn{_conn};
        pqxx::result res = txn.exec("SELECT id FROM students");
        for (const auto& row : res) {
            string id = row["id"].as<string>();
            auto student = loadStudent(id);
            if (student) students.push_back(std::move(student));
        }
        txn.commit();
        print("共{}名学生\n", students.size());
    } catch (const pqxx::sql_error& e) {
        print("加载学生信息失败: {}\nSQL: {}\n", e.what(), e.query());
    }
    return students;
}

//===========================================================================
bool Database::saveTeacher(Teacher* teacher) {
    if (!teacher) return false;
    try {
        pqxx::work txn{_conn};
        //保存教师信息
        string sqlTeacher = format(
            R"(INSERT INTO teachers (id, name)
               VALUES ('{}', '{}')
               ON CONFLICT (id) DO UPDATE SET name = '{}')",
            txn.esc(teacher->getId()), txn.esc(teacher->m_name), txn.esc(teacher->m_name)
        );
        txn.exec(sqlTeacher);

        //同步课程和教师关联
        if (teacher->m_course) {
            string sqlCourse = format(
                R"(UPDATE courses SET teacher_id = '{}' WHERE id = '{}')",
                txn.esc(teacher->getId()), txn.esc(teacher->m_course->m_id)
            );
            txn.exec(sqlCourse);
        }

        txn.commit();
        print("插入教师成功\n", teacher->m_id);
        return true;
    } catch (const pqxx::sql_error& e) {
        print("插入教师失败: {}\nSQL: {}\n", e.what(), e.query());
        return false;
    }
}

bool Database::saveStudentGrade(const string& studentId, const string& courseId, float grade) {
    try {
        pqxx::work txn{_conn};
        string sql = format(
            R"(INSERT INTO student_course (student_id, course_id, grade)
               VALUES ('{}', '{}', {})
               ON CONFLICT (student_id, course_id) DO UPDATE SET grade = {})",
            txn.esc(studentId), txn.esc(courseId), grade, grade
        );
        txn.exec(sql);
        txn.commit();
        print("[DB] 学生[{}]课程[{}]成绩[{}]已持久化\n", studentId, courseId, grade);
        return true;
    } catch (const pqxx::sql_error& e) {
        print("[DB] 保存成绩失败: {}\nSQL: {}\n", e.what(), e.query());
        return false;
    }
}
unique_ptr<Teacher> Database::loadTeacher(const string& teacherId) {
    try {
        pqxx::read_transaction txn{_conn};
        //基础信息
        string sql = format(
            "SELECT id, name FROM teachers WHERE id = '{}'",
            txn.esc(teacherId)
        );
        pqxx::result res = txn.exec(sql);
        if (res.empty()) return nullptr;

        auto teacher = std::make_unique<Teacher>(
            res[0]["id"].as<string>(),
            res[0]["name"].as<string>()
        );

        //加载授课课程
        string sqlCourse = format(
            R"(SELECT c.id, c.name, c.credit, c.maximum
               FROM courses c WHERE c.teacher_id = '{}')",
            txn.esc(teacherId)
        );
        pqxx::result resCourse = txn.exec(sqlCourse);

        auto& registrar = Registrar::getInstance();
        for (const auto& row : resCourse) {
            auto course = std::make_unique<Course>(
                row["id"].as<string>(),
                row["name"].as<string>(),
                row["credit"].as<int>(),
                teacher.get(),
                row["maximum"].as<int>()
            );
            teacher->m_course = course.get();
            registrar.addCourse(std::move(course));

            // 加载课程下的学生
            loadCourseStudents(course.get());
        }
        txn.commit();
        return teacher;
    } catch (const pqxx::sql_error& e) {
        print("加载教师{}失败: {}\nSQL: {}\n", teacherId, e.what(), e.query());
        return nullptr;
    }
}
vector<unique_ptr<Teacher>> Database::loadAllTeachers() {
    vector<unique_ptr<Teacher>> teachers;
    try {
        pqxx::read_transaction txn{_conn};
        pqxx::result res = txn.exec("SELECT id FROM teachers");
        for (const auto& row : res) {
            string id = row["id"].as<string>();
            auto teacher = loadTeacher(id);
            if (teacher) teachers.push_back(std::move(teacher));
        }
        txn.commit();
        print("加载{}名教师\n", teachers.size());
    } catch (const pqxx::sql_error& e) {
        print("加载教师失败: {}\nSQL: {}\n", e.what(), e.query());
    }
    return teachers;
}

bool Database::saveCourse(Course* course) {
    if (!course) return false;
    try {
        pqxx::work txn{_conn};
        string teacherId = course->m_teacher ? course->m_teacher->m_id : "";
        string sql = format(
            R"(INSERT INTO courses (id, name, credit, maximum, teacher_id)
               VALUES ('{}', '{}', {}, {}, '{}')
               ON CONFLICT (id) DO UPDATE
               SET name = '{}', credit = {}, maximum = {}, teacher_id = '{}')",
            txn.esc(course->m_id), txn.esc(course->m_name), course->m_credit,
            course->getMaximum(), txn.esc(teacherId),
            txn.esc(course->m_name), course->m_credit, course->m_maximum, txn.esc(teacherId)
        );
        txn.exec(sql);
        txn.commit();
        print("课程[{}]容量[{}]成功插入\n", course->m_id, course->m_maximum);
        return true;
    } catch (const pqxx::sql_error& e) {
        print("插入课程失败: {}\nSQL: {}\n", e.what(), e.query());
        return false;
    }
}
bool Database::updateCourseCapacity(const string& courseId, int newCapacity) {
    try {
        pqxx::work txn{_conn};
        string sql = format(
            R"(UPDATE courses SET maximum = {} WHERE id = '{}')",
            newCapacity, txn.esc(courseId)
        );
        pqxx::result res = txn.exec(sql);
        if (res.affected_rows() == 0) {
            txn.abort();
            print("课程[{}]不存在，容量更新失败\n", courseId);
            return false;
        }
        txn.commit();
        print("课程[{}]容量更新为{}\n", courseId, newCapacity);
        return true;
    } catch (const pqxx::sql_error& e) {
        print("更新课程容量失败: {}\nSQL: {}\n", e.what(), e.query());
        return false;
    }
}
// 加载所有课程
vector<unique_ptr<Course>> Database::loadAllCourses() {
    vector<unique_ptr<Course>> courses;
    try {
        pqxx::read_transaction txn{_conn};
        pqxx::result res = txn.exec(R"(
            SELECT c.id, c.name, c.credit, c.maximum, t.id as teacher_id, t.name as teacher_name
            FROM courses c
            LEFT JOIN teachers t ON c.teacher_id = t.id
        )");

        auto& registrar = Registrar::getInstance();
        for (const auto& row : res) {
            // 重建教师
            unique_ptr<Teacher> teacher = nullptr;
            if (!row["teacher_id"].is_null()) {
                teacher = std::make_unique<Teacher>(
                    row["teacher_id"].as<string>(),
                    row["teacher_name"].as<string>()
                );
                registrar.addTeacher(std::move(teacher));
            }

            // 重建课程
            auto course = std::make_unique<Course>(
                row["id"].as<string>(),
                row["name"].as<string>(),
                row["credit"].as<int>(),
                teacher.get(),
                row["maximum"].as<int>()
            );

            // 加载课程学生
             loadCourseStudents(course.get());
             courses.push_back(std::move(course));
         }

         txn.commit();
         print("加载完成{}门课程\n", courses.size());
     } catch (const pqxx::sql_error& e) {
         print("加载所有课程失败: {}\nSQL: {}\n", e.what(), e.query());
     }
     return courses;
 }

bool Database::saveSecretary(Secretary* secretary) {
    if (!secretary) return false;
    try {
        pqxx::work txn{_conn};
        string sql = format(
            R"(INSERT INTO secretaries (id, name)
               VALUES ('{}', '{}')
               ON CONFLICT (id) DO UPDATE SET name = '{}')",
            txn.esc(secretary->m_id), txn.esc(secretary->m_name), txn.esc(secretary->m_name)
        );
        txn.exec(sql);
        txn.commit();
        print("秘书[{}]已持久化\n", secretary->m_id);
        return true;
    } catch (const pqxx::sql_error& e) {
        print("保存秘书失败: {}\nSQL: {}\n", e.what(), e.query());
        return false;
    }
}
unique_ptr<Secretary> Database::loadSecretary(const string& secretaryId) {
    try {
        pqxx::read_transaction txn{_conn};
        string sql = format(
            "SELECT id, name FROM secretaries WHERE id = '{}'",
            txn.esc(secretaryId)
        );
        pqxx::result res = txn.exec(sql);
        if (res.empty()) return nullptr;

        auto& registrar = Registrar::getInstance();
        auto secretary = std::make_unique<Secretary>(
            registrar,
            res[0]["id"].as<string>(),
            res[0]["name"].as<string>()
        );

        txn.commit();
        return secretary;
    } catch (const pqxx::sql_error& e) {
        print("加载秘书[{}]失败: {}\nSQL: {}\n", secretaryId, e.what(), e.query());
        return nullptr;
    }
}
bool isConnected()
{
    return _conn.is_open();
}

void Database::loadCourseStudents(Course* course) {
    if (!course) return;
    try {
        pqxx::read_transaction txn{_conn};
        string sql = format(
            R"(SELECT s.id, s.name, sc.grade
               FROM student_course sc
               JOIN students s ON sc.student_id = s.id
               WHERE sc.course_id = '{}')",
            txn.esc(course->getId())
        );
        pqxx::result res = txn.exec(sql);

        auto& registrar = Registrar::getInstance();
        for (const auto& row : res) {
            auto student = std::make_unique<Student>(
                row["id"].as<string>(),
                row["name"].as<string>()
            );
            course->addStudent(student.get());
            course->setStudentGrade(student.get(), row["grade"].as<float>());
            registrar.addStudent(std::move(student));
        }
        txn.commit();
    } catch (const pqxx::sql_error& e) {
        print("加载课程[{}]学生失败: {}\nSQL: {}\n", course->m_id, e.what(), e.query());
    }
}

export auto& g_database = Database::getInstance();
