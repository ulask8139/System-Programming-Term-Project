#include <iostream>
#include <limits>
#include <cstdlib>
#include <string>

#include "sis.hpp"

static std::string getenv_or(const char* key, const std::string& fallback) {
  const char* v = std::getenv(key);
  return (v && *v) ? std::string(v) : fallback;
}

static sis::Student read_student(bool with_id) {
  sis::Student s;

  if (with_id) {
    std::cout << "ID: ";
    std::cin >> s.id;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }

  std::cout << "Name: ";
  std::getline(std::cin, s.name);

  std::cout << "Surname: ";
  std::getline(std::cin, s.surname);

  std::cout << "Department: ";
  std::getline(std::cin, s.department);

  std::cout << "Email: ";
  std::getline(std::cin, s.email);

  return s;
}

int main() {
  try {
    const std::string conninfo =
      getenv_or("DB_CONNINFO",
                "host=db port=5432 dbname=studentdb user=postgres password=postgres");

    sis::Conn conn(conninfo);
    sis::init_schema(conn.c);

    while (true) {
      std::cout << "\n--- Student Information System ---\n"
                << "1) Insert\n"
                << "2) List\n"
                << "3) Update\n"
                << "4) Delete\n"
                << "5) Exit\n"
                << "Choose: ";

      int choice = 0;
      std::cin >> choice;
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

      if (choice == 5) break;

      if (choice == 1) {
        auto s = read_student(true);
        std::string err;
        if (!sis::is_valid_student(s, err)) {
          std::cout << "Error: " << err << "\n";
          continue;
        }
        sis::insert_student(conn.c, s);
        std::cout << "Inserted.\n";
      }
      else if (choice == 2) {
        auto list = sis::list_students(conn.c);
        if (list.empty()) {
          std::cout << "(No students)\n";
        } else {
          for (const auto& s : list) {
            std::cout << s.id << " | " << s.name << " " << s.surname
                      << " | " << s.department << " | " << s.email << "\n";
          }
        }
      }
      else if (choice == 3) {
        std::cout << "ID to update: ";
        int id = 0;
        std::cin >> id;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        auto existing = sis::get_student(conn.c, id);
        if (!existing) {
          std::cout << "Student not found.\n";
          continue;
        }

        std::cout << "Enter new values:\n";
        auto s = read_student(false);
        s.id = id;

        std::string err;
        if (!sis::is_valid_student(s, err)) {
          std::cout << "Error: " << err << "\n";
          continue;
        }

        bool ok = sis::update_student(conn.c, s);
        std::cout << (ok ? "Updated.\n" : "Update failed.\n");
      }
      else if (choice == 4) {
        std::cout << "ID to delete: ";
        int id = 0;
        std::cin >> id;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        bool ok = sis::delete_student(conn.c, id);
        std::cout << (ok ? "Deleted.\n" : "Student not found.\n");
      }
      else {
        std::cout << "Invalid option.\n";
      }
    }

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Fatal error: " << e.what() << "\n";
    return 1;
  }
}
