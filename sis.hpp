#pragma once

#include <string>
#include <vector>
#include <optional>
#include <pqxx/pqxx>

namespace sis {

// -------------------- Data model --------------------
struct Student {
  int id{};
  std::string name;
  std::string surname;
  std::string department;
  std::string email;
};

// -------------------- Connection validator --------------------
struct Conn {
  pqxx::connection c;
  explicit Conn(const std::string& conninfo);
};

// -------------------- Validation --------------------
bool is_valid_email(const std::string& email);
bool is_valid_student(const Student& s, std::string& error_out);

// -------------------- Schema --------------------
void init_schema(pqxx::connection& c);

// -------------------- Prepared statements --------------------
void ensure_prepared(pqxx::connection& c);

// -------------------- CRUD operations --------------------
void insert_student(pqxx::connection& c, const Student& s);
std::vector<Student> list_students(pqxx::connection& c);
std::optional<Student> get_student(pqxx::connection& c, int id);
bool update_student(pqxx::connection& c, const Student& s);
bool delete_student(pqxx::connection& c, int id);

} // namespace sis
