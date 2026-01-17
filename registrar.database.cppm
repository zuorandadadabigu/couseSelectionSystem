// Module registrar:database
// File: registrar.database.cppm   Version: 0.1.0   License: AGPLv3
// Created: zhouyuxin      2026-01-17
// Description:To realize the function of database.
//

export module registrar:database;

import std;
import :student;
import :teacher;
import :course;
import :sectary;
import :registrar;

using std::string;
using std::vector;
using std::unique_ptr;
using std::print;
using std::format;

// 数据库配置（替换为实际环境）
constexpr auto DB_HOST = "127.0.0.1";
constexpr auto DB_PORT = "5432";
constexpr auto DB_NAME = "course_selection";
constexpr auto DB_USER = "postgres";
constexpr auto DB_PASS = "your_password";

export class Database {
public:
    // 单例模式（代管者全局唯一）
    static Database& getInstance() {
        static Database instance;
        return instance;
    }

    // 禁止拷贝/移动
    Database(const Database&) = delete;
    Database(Database&&) = delete;
    Database& operator=(const Database&) = delete;
    Database& operator=(Database&&) = delete;

    // ========== 学生数据操作（适配UI选课/退课） ==========
    // 保存学生（含选课/成绩，适配Ui::service_student）
    bool saveStudent(Student* student) {
        if (!student) return false;
        try {
            pqxx::work txn{_conn};
            // 1. 保存学生基础信息
            string sqlStudent = format(
                R"(INSERT INTO students (id, name)
                   VALUES ('{}', '{}')
                   ON CONFLICT (id) DO UPDATE SET name = '{}')",
                txn.esc(student->getId()), txn.esc(student->getName()), txn.esc(student->getName())
            );
            txn.exec(sqlStudent);

            // 2. 先删除该学生原有选课记录（避免脏数据）
            string sqlDelSC = format(
                "DELETE FROM student_course WHERE student_id = '{}'",
                txn.esc(student->getId())
            );
            txn.exec(sqlDelSC);

            // 3. 重新插入选课+成绩（匹配Student::_courses/_grades）
            for (const auto& course : student->_courses) {
                float grade = student->getGrade(course->getId());
                string sqlSC = format(
                    R"(INSERT INTO student_course (student_id, course_id, grade)
                       VALUES ('{}', '{}', {})
                       ON CONFLICT (student_id, course_id) DO UPDATE SET grade = {})",
                    txn.esc(student->getId()), txn.esc(course->getId()), grade, grade
                );
                txn.exec(sqlSC);
            }

            txn.commit();
            print("[DB] 学生[{}]选课/成绩已持久化\n", student->getId());
            return true;
        } catch (const pqxx::sql_error& e) {
            print("[DB] 保存学生失败: {}\nSQL: {}\n", e.what(), e.query());
            return false;
        }
    }

// 加载单个学生（适配Ui::identifyService case3）
    unique_ptr<Student> loadStudent(const string& studentId) {
        try {
            pqxx::read_transaction txn{_conn};
            // 1. 基础信息
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

            // 2. 加载选课+成绩
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
                registrar.addCourse(std::move(course));

                // 关联到学生
                student->addCourse(course.get());
                student->addGrade(course->getId(), row["grade"].as<float>());
            }

            txn.commit();
            return student;
        } catch (const pqxx::sql_error& e) {
            print("[DB] 加载学生[{}]失败: {}\nSQL: {}\n", studentId, e.what(), e.query());
            return nullptr;
        }
    }

    // 加载所有学生（适配Secretary::showAllStudent）
    vector<unique_ptr<Student>> loadAllStudents() {
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
            print("[DB] 加载完成{}名学生\n", students.size());
        } catch (const pqxx::sql_error& e) {
            print("[DB] 加载所有学生失败: {}\nSQL: {}\n", e.what(), e.query());
        }
        return students;
    }

// ========== 教师数据操作（适配UI录成绩） ==========
    // 保存教师（含授课课程，适配Ui::service_teacher case3）
    bool saveTeacher(Teacher* teacher) {
        if (!teacher) return false;
        try {
            pqxx::work txn{_conn};
            // 1. 保存教师基础信息
            string sqlTeacher = format(
                R"(INSERT INTO teachers (id, name)
                   VALUES ('{}', '{}')
                   ON CONFLICT (id) DO UPDATE SET name = '{}')",
                txn.esc(teacher->getId()), txn.esc(teacher->getName()), txn.esc(teacher->getName())
            );
            txn.exec(sqlTeacher);

            // 2. 同步授课课程的教师关联
            if (teacher->m_course) {
                string sqlCourse = format(
                    R"(UPDATE courses SET teacher_id = '{}' WHERE id = '{}')",
                    txn.esc(teacher->getId()), txn.esc(teacher->m_course->getId())
                );
                txn.exec(sqlCourse);
            }

            txn.commit();
            print("[DB] 教师[{}]已持久化\n", teacher->getId());
            return true;
        } catch (const pqxx::sql_error& e) {
            print("[DB] 保存教师失败: {}\nSQL: {}\n", e.what(), e.query());
            return false;
        }
    }

    // 保存教师录入的成绩（适配Teacher::addCourseGrade）
    bool saveStudentGrade(const string& studentId, const string& courseId, float grade) {
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

    // 加载单个教师（适配Ui::identifyService case1）
    unique_ptr<Teacher> loadTeacher(const string& teacherId) {
        try {
            pqxx::read_transaction txn{_conn};
            // 1. 基础信息
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

            // 2. 加载授课课程
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

                // 加载课程下的学生（适配Teacher::showCourseStudents）
                loadCourseStudents(course.get());
            }

            txn.commit();
            return teacher;
        } catch (const pqxx::sql_error& e) {
            print("[DB] 加载教师[{}]失败: {}\nSQL: {}\n", teacherId, e.what(), e.query());
            return nullptr;
        }
    }

    // 加载所有教师
    vector<unique_ptr<Teacher>> loadAllTeachers() {
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
            print("[DB] 加载完成{}名教师\n", teachers.size());
        } catch (const pqxx::sql_error& e) {
            print("[DB] 加载所有教师失败: {}\nSQL: {}\n", e.what(), e.query());
        }
        return teachers;
    }

// ========== 课程数据操作（适配UI选课/秘书改容量） ==========
   // 保存课程（适配Secretary::adjustCourseCapacity）
   bool saveCourse(Course* course) {
       if (!course) return false;
       try {
           pqxx::work txn{_conn};
           string teacherId = course->m_teacher ? course->m_teacher->getId() : "";
           string sql = format(
               R"(INSERT INTO courses (id, name, credit, maximum, teacher_id)
                  VALUES ('{}', '{}', {}, {}, '{}')
                  ON CONFLICT (id) DO UPDATE
                  SET name = '{}', credit = {}, maximum = {}, teacher_id = '{}')",
               txn.esc(course->getId()), txn.esc(course->getName()), course->getCredit(),
               course->getMaximum(), txn.esc(teacherId),
               txn.esc(course->getName()), course->getCredit(), course->getMaximum(), txn.esc(teacherId)
           );
           txn.exec(sql);
           txn.commit();
           print("[DB] 课程[{}]容量[{}]已持久化\n", course->getId(), course->getMaximum());
           return true;
       } catch (const pqxx::sql_error& e) {
           print("[DB] 保存课程失败: {}\nSQL: {}\n", e.what(), e.query());
           return false;
       }
   }

   // 更新课程容量（适配Ui::service_sectary case3）
   bool updateCourseCapacity(const string& courseId, int newCapacity) {
       try {
           pqxx::work txn{_conn};
           string sql = format(
               R"(UPDATE courses SET maximum = {} WHERE id = '{}')",
               newCapacity, txn.esc(courseId)
           );
           pqxx::result res = txn.exec(sql);
           if (res.affected_rows() == 0) {
               txn.abort();
               print("[DB] 课程[{}]不存在，容量更新失败\n", courseId);
               return false;
           }
           txn.commit();
           print("[DB] 课程[{}]容量更新为{}\n", courseId, newCapacity);
           return true;
       } catch (const pqxx::sql_error& e) {
           print("[DB] 更新课程容量失败: {}\nSQL: {}\n", e.what(), e.query());
           return false;
       }
   }

   // 加载所有课程（适配Ui::service_student case1/Secretary::showAllCourse）
   vector<unique_ptr<Course>> loadAllCourses() {
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
            print("[DB] 加载完成{}门课程\n", courses.size());
        } catch (const pqxx::sql_error& e) {
            print("[DB] 加载所有课程失败: {}\nSQL: {}\n", e.what(), e.query());
        }
        return courses;
    }

    // ========== 秘书数据操作（适配UI登录） ==========
    // 保存秘书
    bool saveSecretary(Secretary* secretary) {
        if (!secretary) return false;
        try {
            pqxx::work txn{_conn};
            string sql = format(
                R"(INSERT INTO secretaries (id, name)
                   VALUES ('{}', '{}')
                   ON CONFLICT (id) DO UPDATE SET name = '{}')",
                txn.esc(secretary->getId()), txn.esc(secretary->getName()), txn.esc(secretary->getName())
            );
            txn.exec(sql);
            txn.commit();
            print("[DB] 秘书[{}]已持久化\n", secretary->getId());
            return true;
        } catch (const pqxx::sql_error& e) {
            print("[DB] 保存秘书失败: {}\nSQL: {}\n", e.what(), e.query());
            return false;
        }
    }

    // 加载单个秘书（适配Ui::identifyService case2）
    unique_ptr<Secretary> loadSecretary(const string& secretaryId) {
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
            print("[DB] 加载秘书[{}]失败: {}\nSQL: {}\n", secretaryId, e.what(), e.query());
            return nullptr;
        }
    }

    // 加载所有秘书
    vector<unique_ptr<Secretary>> loadAllSecretaries() {
        vector<unique_ptr<Secretary>> secretaries;
        try {
            pqxx::read_transaction txn{_conn};
            pqxx::result res = txn.exec("SELECT id FROM secretaries");
            for (const auto& row : res) {
                string id = row["id"].as<string>();
                auto secretary = loadSecretary(id);
                if (secretary) secretaries.push_back(std::move(secretary));
            }
            txn.commit();
            print("[DB] 加载完成{}名秘书\n", secretaries.size());
        } catch (const pqxx::sql_error& e) {
            print("[DB] 加载所有秘书失败: {}\nSQL: {}\n", e.what(), e.query());
        }
        return secretaries;
    }

    // ========== 通用工具 ==========
    bool isConnected() const { return _conn.is_open(); }

private:
    // 私有构造：初始化连接
    Database() {
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

// 析构：关闭连接
    ~Database() {
        if (_conn.is_open()) {
            _conn.close();
            print("[DB] 连接已关闭\n");
        }
    }

    // 辅助：加载课程下的学生（适配Teacher::showCourseStudents）
    void loadCourseStudents(Course* course) {
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
            print("[DB] 加载课程[{}]学生失败: {}\nSQL: {}\n", course->getId(), e.what(), e.query());
        }
    }

    pqxx::connection _conn; // PostgreSQL连接
};

// 全局快捷访问
export auto& g_database = Database::getInstance();
