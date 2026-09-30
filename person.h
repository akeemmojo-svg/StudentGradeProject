#pragma once

#include <iostream>
#include <string>
#include <vector>

class Person
{
private:
    std::string name;
    std::string surname;
    std::vector<int> homeworkGrades;
    int examGrade;

public:
    // Constructors
    Person();
    Person(const std::string& name, const std::string& surname);

    // Rule of Three
    Person(const Person& other);
    Person& operator=(const Person& other);
    ~Person();

    // Getters
    std::string getName() const;
    std::string getSurname() const;
    std::vector<int> getHomeworkGrades() const;
    int getExamGrade() const;

    // Setters
    void setName(const std::string& name);
    void setSurname(const std::string& surname);
    void setHomeworkGrades(const std::vector<int>& grades);
    void setExamGrade(int grade);

    // Calculations
    double calculateAverage() const;
    double calculateMedian() const;
    double calculateFinalGradeAverage() const;
    double calculateFinalGradeMedian() const;

    // Stream operators
    friend std::istream& operator>>(std::istream& in, Person& person);
    friend std::ostream& operator<<(std::ostream& out, const Person& person);
};