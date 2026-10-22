
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <random>
#include "Person.h"
#include "FileManager.h"
#include <exception>
#include "BenchmarkProcessor.h"

void sortStudentsByName(std::vector<Person>& students);

int readGrade(const std::string& message)
{
    int grade;

    while (true)
    {
        std::cout << message;

        if (!(std::cin >> grade))
        {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input. Please enter a number from 1 to 10.\n";
            continue;
        }

        if (grade >= 1 && grade <= 10)
        {
            return grade;
        }

        std::cout << "Invalid grade. Please enter a grade from 1 to 10.\n";
    }
}

void addStudent(std::vector<Person>& students)
{
    std::string name;
    std::string surname;

    std::cout << "\n===== ADD STUDENT =====\n";

    std::cout << "Enter name: ";
    std::cin >> name;

    std::cout << "Enter surname: ";
    std::cin >> surname;

    Person person(name, surname);
    std::vector<int> grades;

    std::cout << "\nEnter homework grades.\n";
    std::cout << "Enter 0 when you have finished entering homework grades.\n";

    while (true)
    {
        int grade;

        std::cout << "Enter homework grade: ";

        if (!(std::cin >> grade))
        {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        if (grade == 0)
        {
            break;
        }

        if (grade >= 1 && grade <= 10)
        {
            grades.push_back(grade);
        }
        else
        {
            std::cout << "Invalid grade. Please enter 1-10 or 0 to finish.\n";
        }
    }

    if (grades.empty())
    {
        std::cout << "At least one homework grade is required.\n";
        return;
    }

    person.setHomeworkGrades(grades);

    int examGrade = readGrade("Enter exam grade: ");
    person.setExamGrade(examGrade);
    students.push_back(person);

    sortStudentsByName(students);

    FileManager::saveStudents(students);
   

    std::cout << "\nStudent added successfully!\n";
}

void listStudents(const std::vector<Person>& students)
{
    if (students.empty())
    {
        std::cout << "\nNo students found.\n";
        return;
    }

    int method;

    std::cout << "\n===== CALCULATION METHOD =====\n";
    std::cout << "1. Average\n";
    std::cout << "2. Median\n";
    std::cout << "Choose calculation method: ";

    if (!(std::cin >> method))
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Invalid input.\n";
        return;
    }

    while (method != 1 && method != 2)
    {
        std::cout << "Invalid choice. Please enter 1 or 2: ";

        if (!(std::cin >> method))
        {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input.\n";
            return;
        }
    }

    std::cout << "\n===== STUDENT RESULTS =====\n\n";

    if (method == 1)
    {
        std::cout << std::left
            << std::setw(15) << "Name"
            << std::setw(15) << "Surname"
            << std::setw(18) << "Final (Avg.)"
            << "\n";

        std::cout << "-----------------------------------------------\n";

        for (const Person& person : students)
        {
            std::cout << std::left
                << std::setw(15) << person.getName()
                << std::setw(15) << person.getSurname()
                << std::setw(18)
                << std::fixed << std::setprecision(2)
                << person.calculateFinalGradeAverage()
                << "\n";
        }
    }
    else
    {
        std::cout << std::left
            << std::setw(15) << "Name"
            << std::setw(15) << "Surname"
            << std::setw(18) << "Final (Med.)"
            << "\n";

        std::cout << "-----------------------------------------------\n";

        for (const Person& person : students)
        {
            std::cout << std::left
                << std::setw(15) << person.getName()
                << std::setw(15) << person.getSurname()
                << std::setw(18)
                << std::fixed << std::setprecision(2)
                << person.calculateFinalGradeMedian()
                << "\n";
        }
    }
}

void findStudent(const std::vector<Person>& students)
{
    if (students.empty())
    {
        std::cout << "\nNo students found.\n";
        return;
    }

    std::string searchName;

    std::cout << "\nEnter student name to find: ";
    std::cin >> searchName;

    bool found = false;

    for (const Person& person : students)
    {
        if (person.getName() == searchName)
        {
            std::cout << "\n===== STUDENT FOUND =====\n";

            std::cout << "Name: "
                << person.getName() << " "
                << person.getSurname() << "\n";

            std::cout << "Homework average: "
                << std::fixed << std::setprecision(2)
                << person.calculateAverage() << "\n";

            std::cout << "Homework median: "
                << person.calculateMedian() << "\n";

            std::cout << "Exam grade: "
                << person.getExamGrade() << "\n";

            std::cout << "Final grade (Average): "
                << person.calculateFinalGradeAverage() << "\n";

            std::cout << "Final grade (Median): "
                << person.calculateFinalGradeMedian() << "\n";

            found = true;
        }
    }

    if (!found)
    {
        std::cout << "\nStudent not found.\n";
    }
}

void deleteStudent(std::vector<Person>& students)
{
    if (students.empty())
    {
        std::cout << "\nNo students found.\n";
        return;
    }

    std::string searchName;

    std::cout << "\nEnter student name to delete: ";
    std::cin >> searchName;

    for (auto it = students.begin(); it != students.end(); ++it)
    {
        if (it->getName() == searchName)
        {
            std::cout << "\nStudent found: "
                << it->getName() << " "
                << it->getSurname() << "\n";

            students.erase(it);

            FileManager::saveStudents(students);

            std::cout << "Student deleted successfully.\n";
            return;
        }
    }

    std::cout << "\nStudent not found.\n";
}

void sortStudentsByName(std::vector<Person>& students)
{
    std::sort(
        students.begin(),
        students.end(),
        [](const Person& a, const Person& b)
        {
            return a.getName() < b.getName();
        }
    );
}

void generateRandomStudents(std::vector<Person>& students)
{
    int numberOfStudents;

    std::cout << "\nHow many random students do you want to generate? ";

    if (!(std::cin >> numberOfStudents))
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Invalid input.\n";
        return;
    }

    if (numberOfStudents <= 0)
    {
        std::cout << "Invalid number of students.\n";
        return;
    }

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> gradeDistribution(1, 10);

    for (int i = 1; i <= numberOfStudents; i++)
    {
        Person person(
            "Student" + std::to_string(i),
            "Surname" + std::to_string(i)
        );

        std::vector<int> grades;

        for (int j = 0; j < 5; j++)
        {
            grades.push_back(gradeDistribution(generator));
        }

        person.setHomeworkGrades(grades);
        person.setExamGrade(gradeDistribution(generator));

        students.push_back(person);
    }

    FileManager::saveStudents(students);

    std::cout << numberOfStudents
        << " random students generated successfully.\n";
}

int main()
{
    std::vector<Person> students;
    try
    {
        FileManager::loadStudents(students);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error loading students: " << e.what() << '\n';
    }

    sortStudentsByName(students);

    int choice;
    while (true)
    {
        std::cout << "\n===== STUDENT GRADE PROJECT =====\n";
        std::cout << "1. Add student\n";
        std::cout << "2. List students\n";
        std::cout << "3. Find student\n";
        std::cout << "4. Delete student\n";
        std::cout << "5. Generate random students\n";
        std::cout << "6. Sort students by name\n";
        std::cout << "7. Generate datasets and benchmark\n";
        std::cout << "8. Exit\n";
        std::cout << "Choose an option: ";

        if (!(std::cin >> choice))
        {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "\nInvalid input. Please choose 1-8.\n";
            continue;
        }

        try
        {
            if (choice == 1)
            {
                addStudent(students);
            }
            else if (choice == 2)
            {
                listStudents(students);
            }
            else if (choice == 3)
            {
                findStudent(students);
            }
            else if (choice == 4)
            {
                deleteStudent(students);
            }
            else if (choice == 5)
            {
                generateRandomStudents(students);
            }
            else if (choice == 6)
            {
                sortStudentsByName(students);
                std::cout << "\nStudents sorted by name.\n";
            }
            else if (choice == 7)
            {
                BenchmarkProcessor::runVectorBenchmark();
            }
            else if (choice == 8)
            {
                FileManager::saveStudents(students);
                std::cout << "\nGoodbye!\n";
                break;
            }
            else
            {
                std::cout << "\nInvalid option. Please choose 1-8.\n";
            }
        }
        catch (const std::exception& error)
        {
            std::cerr << "\nError: " << error.what() << '\n';
        }
    }

    return 0;
}
  
