
#include "FileManager.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

void FileManager::saveStudents(const std::vector<Person>& students)
{
    std::ofstream file("Students.txt");

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Could not open Students.txt for writing."
        );
    }

    file << "Name Surname HW1 HW2 HW3 HW4 HW5 Exam\n";

    for (const Person& person : students)
    {
        file << person.getName() << ' ';
        file << person.getSurname() << ' ';

        const std::vector<int> grades = person.getHomeworkGrades();

        for (int grade : grades)
        {
            file << grade << ' ';
        }

        file << person.getExamGrade() << '\n';
    }

    if (!file)
    {
        throw std::runtime_error(
            "An error occurred while writing Students.txt."
        );
    }
}

void FileManager::loadStudents(std::vector<Person>& students)
{
    std::ifstream file("Students.txt");

    if (!file.is_open())
    {
        // A missing file means the program has no saved students yet.
        // Other file-opening errors should be reported.
        std::ifstream check("Students.txt");

        if (check.bad())
        {
            throw std::runtime_error(
                "Could not open Students.txt for reading."
            );
        }

        return;
    }

    std::string header;

    if (!std::getline(file, header))
    {
        if (file.bad())
        {
            throw std::runtime_error(
                "Error reading Students.txt."
            );
        }

        return;
    }

    std::string name;
    std::string surname;

    while (file >> name >> surname)
    {
        std::vector<int> grades;

        for (int i = 0; i < 5; ++i)
        {
            int grade;

            if (!(file >> grade))
            {
                throw std::runtime_error(
                    "Invalid or incomplete homework grades in Students.txt."
                );
            }

            if (grade < 1 || grade > 10)
            {
                throw std::runtime_error(
                    "A homework grade in Students.txt is outside 1-10."
                );
            }

            grades.push_back(grade);
        }

        int examGrade;

        if (!(file >> examGrade))
        {
            throw std::runtime_error(
                "Invalid or missing exam grade in Students.txt."
            );
        }

        if (examGrade < 1 || examGrade > 10)
        {
            throw std::runtime_error(
                "An exam grade in Students.txt is outside 1-10."
            );
        }

        Person person(name, surname);
        person.setHomeworkGrades(grades);
        person.setExamGrade(examGrade);

        students.push_back(person);
    }

    if (file.bad())
    {
        throw std::runtime_error(
            "An error occurred while reading Students.txt."
        );
    }
}
