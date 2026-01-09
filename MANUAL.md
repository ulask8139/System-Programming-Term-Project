# System-Programming-Term-Project

# Usage Instructions

To use the Student Information System, follow these steps:

1. Enter the Application Container:
docker exec -it sis-app-container /bin/sh

2. Available CRUD Operations:
- Save: Enter student ID, name, surname, deparmant, and email.
- List: View all currently stored student info.
- Update: Modify existing information using the student ID.
- Delete: Removes a student's record from the database.

# Troubleshooting 

- Make sure the PostgreSQL volume is mapped correctly.
- Check the interval network if the application cant reach the database.
