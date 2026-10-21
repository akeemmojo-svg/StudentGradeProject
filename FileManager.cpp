
#include "FileManager.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>

void FileManager::saveStudents(const std::vector<Person>& students)
{
    std::ofstream file("Students.txt");
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open Students.txt for writing.");
    }

    file << "Name Surname HW1 HW2 HW3 HW4 HW5 Exam\n";

    for (const Person& person : students)
    {
        file << person.getName() << " ";
        file << person.getSurname() << " ";

        std::vector<int> grades = person.getHomeworkGrades();

        for (int grade : grades)
        {
            file << grade << " ";
        }

        file << person.getExamGrade() << "\n";
    }
}

void FileManager::loadStudents(std::vector<Person>& students)
{
    std::ifstream file("Students.txt");
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open Students.txt for reading.");
    }
    

    std::string header;
    std::getline(file, header);

    std::string name;
    std::string surname;

    while (file >> name >> surname)
    {
        std::vector<int> grades;

        for (int i = 0; i < 5; i++)
        {
            int grade;

            if (!(file >> grade))
            {
                std::cerr << "Error: Invalid homework grades in Students.txt.\n";
                return;
            }

            grades.push_back(grade);
        }

        int examGrade;

        if (!(file >> examGrade))
        {
            std::cerr << "Error: Invalid exam grade in Students.txt.\n";
            return;
        }

        Person person(name, surname);
        person.setHomeworkGrades(grades);
        person.setExamGrade(examGrade);

        students.push_back(person);
    }
}
