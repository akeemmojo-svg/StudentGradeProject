
#include "Person.h"
#include "FileManager.h"
#include "BenchmarkProcessor.h"
#include "ContainerBenchmark.h"

#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <algorithm>

int readGrade(const std::string& message)
{
    int grade;

    while (true)
    {
        std::cout << message;

        if (std::cin >> grade && grade >= 0 && grade <= 10)
            return grade;

        std::cout << "Invalid grade. Enter a number from 0 to 10.\n";

        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n'
        );
    }
}

void addStudent(std::vector<Person>& students)
{
    std::string name;
    std::string surname;

    std::cout << "Enter first name: ";
    std::cin >> name;

    std::cout << "Enter surname: ";
    std::cin >> surname;

    int numberOfHomework;

    std::cout << "How many homework grades? ";
    while (!(std::cin >> numberOfHomework) || numberOfHomework < 0)
    {
        std::cout << "Enter a valid non-negative number: ";
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n'
        );
    }

    std::vector<int> homework;

    for (int i = 0; i < numberOfHomework; i++)
    {
        homework.push_back(readGrade("Enter homework grade: "));
    }

    int exam = readGrade("Enter exam grade: ");

    Person student(name, surname);
    student.setHomeworkGrades(homework);
    student.setExamGrade(exam);

    students.push_back(student);

    std::cout << "Student added successfully!\n";
}

void listStudents(const std::vector<Person>& students)
{
    if (students.empty())
    {
        std::cout << "No students found.\n";
        return;
    }

    std::cout << "\n===== STUDENT LIST =====\n";

    for (const Person& student : students)
    {
        std::cout << student << '\n';
    }
}

void findStudent(const std::vector<Person>& students)
{
    std::string name;

    std::cout << "Enter student first name: ";
    std::cin >> name;

    bool found = false;

    for (const Person& student : students)
    {
        if (student.getName() == name)
        {
            std::cout << student << '\n';
            found = true;
        }
    }

    if (!found)
        std::cout << "Student not found.\n";
}

void deleteStudent(std::vector<Person>& students)
{
    std::string name;

    std::cout << "Enter first name of student to delete: ";
    std::cin >> name;

    auto it = std::find_if(
        students.begin(),
        students.end(),
        [&name](const Person& student)
        {
            return student.getName() == name;
        }
    );

    if (it != students.end())
    {
        students.erase(it);
        std::cout << "Student deleted successfully.\n";
    }
    else
    {
        std::cout << "Student not found.\n";
    }
}

void sortStudentsByName(std::vector<Person>& students)
{
    std::sort(
        students.begin(),
        students.end(),
        [](const Person& a, const Person& b)
        {
            if (a.getName() == b.getName())
                return a.getSurname() < b.getSurname();

            return a.getName() < b.getName();
        }
    );

    std::cout << "Students sorted by name.\n";
}

void generateRandomStudents(std::vector<Person>& students)
{
    int count;

    std::cout << "How many students should be generated? ";

    while (!(std::cin >> count) || count <= 0)
    {
        std::cout << "Enter a valid positive number: ";
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n'
        );
    }

    const std::vector<std::string> names = {
        "John", "Mary", "Alex", "Peter", "Anna",
        "James", "Laura", "David", "Emma", "Michael"
    };

    const std::vector<std::string> surnames = {
        "Smith", "Brown", "Johnson", "Wilson", "Taylor",
        "Anderson", "Thomas", "Moore", "Martin", "White"
    };

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    for (int i = 0; i < count; i++)
    {
        std::string name = names[std::rand() % names.size()];
        std::string surname = surnames[std::rand() % surnames.size()];

        std::vector<int> homework;

        for (int j = 0; j < 5; j++)
            homework.push_back(std::rand() % 11);

        int exam = std::rand() % 11;

        Person student(name, surname);
        student.setHomeworkGrades(homework);
        student.setExamGrade(exam);

        students.push_back(student);
    }

    std::cout << count << " random students generated successfully!\n";
}

int main()
{
    std::vector<Person> students;

    FileManager::loadStudents(students);

    int choice;

    while (true)
    {
        std::cout << "\n========== STUDENT GRADE SYSTEM ==========\n";
        std::cout << "1. Add student\n";
        std::cout << "2. List students\n";
        std::cout << "3. Find student\n";
        std::cout << "4. Delete student\n";
        std::cout << "5. Generate random students\n";
        std::cout << "6. Sort students by name\n";
        std::cout << "7. Generate datasets and benchmark vector\n";
        std::cout << "8. Original container benchmark\n";
        std::cout << "9. Strategy 1 benchmark (copy)\n";
        std::cout << "10. Strategy 2 benchmark (move)\n";
        std::cout << "11. Exit\n";
        std::cout << "Choose an option: ";

        if (!(std::cin >> choice))
        {
            std::cout << "Invalid input. Enter a number.\n";
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            continue;
        }

        if (choice < 1 || choice > 11)
        {
            std::cout << "Invalid option. Choose from 1 to 11.\n";
            continue;
        }

        try
        {
            switch (choice)
            {
            case 1:
                addStudent(students);
                break;

            case 2:
                listStudents(students);
                break;

            case 3:
                findStudent(students);
                break;

            case 4:
                deleteStudent(students);
                break;

            case 5:
                generateRandomStudents(students);
                break;

            case 6:
                sortStudentsByName(students);
                break;

            case 7:
                BenchmarkProcessor::runVectorBenchmark();
                break;

            case 8:
                ContainerBenchmark::runAllBenchmarks();
                break;

            case 9:
                ContainerBenchmark::runStrategy1();
                break;

            case 10:
                ContainerBenchmark::runStrategy2();
                break;

            case 11:
                FileManager::saveStudents(students);
                std::cout << "\nGoodbye!\n";
                return 0;
            }
        }
        catch (const std::exception& error)
        {
            std::cerr << "\nError: " << error.what() << '\n';
        }
    }

    return 0;
}
