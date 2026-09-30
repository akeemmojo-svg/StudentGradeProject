#include "Person.h"
#include <algorithm>
#include <numeric>
#include <iomanip>

Person::Person()
{
    examGrade = 0;
}

Person::Person(const std::string& name, const std::string& surname)
{
    this->name = name;
    this->surname = surname;
    examGrade = 0;
}

// Rule of Three: Copy Constructor
Person::Person(const Person& other)
{
    name = other.name;
    surname = other.surname;
    homeworkGrades = other.homeworkGrades;
    examGrade = other.examGrade;
}

// Rule of Three: Copy Assignment Operator
Person& Person::operator=(const Person& other)
{
    if (this != &other)
    {
        name = other.name;
        surname = other.surname;
        homeworkGrades = other.homeworkGrades;
        examGrade = other.examGrade;
    }

    return *this;
}

// Rule of Three: Destructor
Person::~Person()
{
}

// Getters
std::string Person::getName() const
{
    return name;
}

std::string Person::getSurname() const
{
    return surname;
}

std::vector<int> Person::getHomeworkGrades() const
{
    return homeworkGrades;
}

int Person::getExamGrade() const
{
    return examGrade;
}

// Setters
void Person::setName(const std::string& name)
{
    this->name = name;
}

void Person::setSurname(const std::string& surname)
{
    this->surname = surname;
}

void Person::setHomeworkGrades(const std::vector<int>& grades)
{
    homeworkGrades = grades;
}

void Person::setExamGrade(int grade)
{
    examGrade = grade;
}

// Average
double Person::calculateAverage() const
{
    if (homeworkGrades.empty())
    {
        return 0.0;
    }

    double sum = std::accumulate(
        homeworkGrades.begin(),
        homeworkGrades.end(),
        0.0
    );

    return sum / homeworkGrades.size();
}

// Median
double Person::calculateMedian() const
{
    if (homeworkGrades.empty())
    {
        return 0.0;
    }

    std::vector<int> sortedGrades = homeworkGrades;

    std::sort(sortedGrades.begin(), sortedGrades.end());

    size_t size = sortedGrades.size();

    if (size % 2 == 0)
    {
        return (sortedGrades[size / 2 - 1] +
            sortedGrades[size / 2]) / 2.0;
    }

    return sortedGrades[size / 2];
}

// Final grade using Average
// Assignment formula: 0.4 * HW + 0.6 * Exam
double Person::calculateFinalGradeAverage() const
{
    return calculateAverage() * 0.4 + examGrade * 0.6;
}

// Final grade using Median
// Assignment formula: 0.4 * HW + 0.6 * Exam
double Person::calculateFinalGradeMedian() const
{
    return calculateMedian() * 0.4 + examGrade * 0.6;
}

// Input operator
std::istream& operator>>(std::istream& in, Person& person)
{
    in >> person.name;
    in >> person.surname;

    int numberOfHomework;
    in >> numberOfHomework;

    person.homeworkGrades.clear();

    for (int i = 0; i < numberOfHomework; i++)
    {
        int grade;
        in >> grade;
        person.homeworkGrades.push_back(grade);
    }

    in >> person.examGrade;

    return in;
}

// Output operator
std::ostream& operator<<(std::ostream& out, const Person& person)
{
    out << person.name << " "
        << person.surname << " ";

    out << "Final (Avg.): "
        << std::fixed << std::setprecision(2)
        << person.calculateFinalGradeAverage();

    out << " | Final (Med.): "
        << person.calculateFinalGradeMedian();

    return out;
}