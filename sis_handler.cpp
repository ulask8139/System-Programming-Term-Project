#include "sis.hpp"

#include <regex>
#include <stdexcept>

namespace sis {

// ---------- Connection validator ----------
Conn::Conn(const std::string& conninfo) : c(conninfo) {
  if (!c.is_open()) {
    throw std::runtime_error("Failed to connect to PostgreSQL database.");
  }
}

// ---------- Validation ----------
bool is_valid_email(const std::string& email) {
  static const std::regex re(R"(^[A-Za-z0-9._%+\-]+@[A-Za-z0-9.\-]+\.[A-Za-z]{2,}$)");
  return std::regex_match(email, re);
}

bool is_valid_student(const Student& s, std::string& error_out) {
  if (s.id <= 0) { error_out = "ID must be positive."; return false; }
  if (s.name.empty()) { error_out = "Name cannot be empty."; return false; }
  if (s.surname.empty()) { error_out = "Surname cannot be empty."; return false; }
  if (s.department.empty()) { error_out = "Department cannot be empty."; return false; }
  if (!is_valid_email(s.email)) { error_out = "Invalid email format."; return false; }
  return true;
}

// ---------- Prepared statements ----------
void ensure_prepared(pqxx::connection& c) {
  // If already prepared, pqxx may throw. We'll ignore.
  try {
    c.prepare("insert_student",
      "INSERT INTO students(id, name, surname, department, email) "
      "VALUES($1,$2,$3,$4,$5)");

    c.prepare("list_students",
      "SELECT id, name, surname, department, email FROM students ORDER BY id");

    c.prepare("get_student",
      "SELECT id, name, surname, department, email FROM students WHERE id=$1");

    c.prepare("update_student",
      "UPDATE students SET name=$2, surname=$3, department=$4, email=$5 WHERE id=$1");

    c.prepare("delete_student",
      "DELETE FROM students WHERE id=$1");
  } catch (...) {
    // ignore (already prepared)
  }
}

// ---------- Schema ----------
void init_schema(pqxx::connection& c) {
  pqxx::work tx(c);
  tx.exec(R"(
    CREATE TABLE IF NOT EXISTS students (
      id INTEGER PRIMARY KEY,
      name TEXT NOT NULL,
      surname TEXT NOT NULL,
      department TEXT NOT NULL,
      email TEXT NOT NULL
    );
  )");
  tx.commit();

  ensure_prepared(c);
}

// ---------- CRUD ----------
void insert_student(pqxx::connection& c, const Student& s) {
  ensure_prepared(c);
  pqxx::work tx(c);
  tx.exec_prepared("insert_student", s.id, s.name, s.surname, s.department, s.email);
  tx.commit();
}

std::vector<Student> list_students(pqxx::connection& c) {
  ensure_prepared(c);
  pqxx::read_transaction tx(c);
  auto r = tx.exec_prepared("list_students");

  std::vector<Student> out;
  out.reserve(r.size());

  for (const auto& row : r) {
    Student s;
    s.id = row["id"].as<int>();
    s.name = row["name"].as<std::string>();
    s.surname = row["surname"].as<std::string>();
    s.department = row["department"].as<std::string>();
    s.email = row["email"].as<std::string>();
    out.push_back(std::move(s));
  }
  return out;
}

std::optional<Student> get_student(pqxx::connection& c, int id) {
  ensure_prepared(c);
  pqxx::read_transaction tx(c);
  auto r = tx.exec_prepared("get_student", id);

  if (r.empty()) return std::nullopt;

  const auto& row = r[0];
  Student s;
  s.id = row["id"].as<int>();
  s.name = row["name"].as<std::string>();
  s.surname = row["surname"].as<std::string>();
  s.department = row["department"].as<std::string>();
  s.email = row["email"].as<std::string>();
  return s;
}

bool update_student(pqxx::connection& c, const Student& s) {
  ensure_prepared(c);
  pqxx::work tx(c);
  auto r = tx.exec_prepared("update_student", s.id, s.name, s.surname, s.department, s.email);
  tx.commit();
  return r.affected_rows() == 1;
}

bool delete_student(pqxx::connection& c, int id) {
  ensure_prepared(c);
  pqxx::work tx(c);
  auto r = tx.exec_prepared("delete_student", id);
  tx.commit();
  return r.affected_rows() == 1;
}

} // namespace sis
