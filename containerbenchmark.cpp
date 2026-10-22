
#include "ContainerBenchmark.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <deque>
#include <string>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <stdexcept>

struct BenchmarkStudent
{
    std::string name;
    std::string surname;
    std::vector<int> homework;
    int exam = 0;

    double finalGrade() const
    {
        if (homework.empty())
            return exam * 0.6;

        double sum = 0.0;

        for (int grade : homework)
            sum += grade;

        double average = sum / homework.size();

        return 0.4 * average + 0.6 * exam;
    }
};

namespace
{
    const int DATASET_SIZES[] = {
        1000, 10000, 100000, 1000000, 10000000
    };

    std::string findDataset(int size)
    {
        const std::string number = std::to_string(size);

        const std::vector<std::string> candidates = {
            "students_" + number + ".txt",
            "students" + number + ".txt",
            "students_" + number,
            "students" + number,
            "../students_" + number + ".txt",
            "../students" + number + ".txt",
            "../../students_" + number + ".txt",
            "../../students" + number + ".txt"
        };

        for (const std::string& filename : candidates)
        {
            std::ifstream file(filename);

            if (file.good())
                return filename;
        }

        throw std::runtime_error(
            "Cannot find dataset for " + number +
            " students. Check the dataset filename and working directory."
        );
    }

    BenchmarkStudent parseStudentLine(const std::string& line)
    {
        std::istringstream input(line);
        BenchmarkStudent student;

        if (!(input >> student.name >> student.surname))
            throw std::runtime_error("Invalid student record: " + line);

        std::vector<int> grades;
        int grade;

        while (input >> grade)
            grades.push_back(grade);

        if (grades.size() < 2)
            throw std::runtime_error(
                "Student record needs homework grades and an exam grade: " + line
            );

        student.exam = grades.back();
        grades.pop_back();
        student.homework = grades;

        return student;
    }

    template <typename Container>
    void readStudents(const std::string& filename, Container& students)
    {
        std::ifstream file(filename);

        if (!file.is_open())
            throw std::runtime_error("Cannot open input file: " + filename);

        std::string line;
        bool firstLine = true;

        while (std::getline(file, line))
        {
            if (line.empty())
                continue;

            // Skip the header row in the dataset.
            if (firstLine)
            {
                firstLine = false;

                if (line.find("Name") != std::string::npos &&
                    line.find("Surname") != std::string::npos)
                {
                    continue;
                }
            }

            students.push_back(parseStudentLine(line));
        }

        if (file.bad())
            throw std::runtime_error("Error reading input file: " + filename);

        if (students.empty())
            throw std::runtime_error(
                "Dataset contains no student records: " + filename
            );
    }

    template <typename Container>
    void sortByFinalGrade(Container& students)
    {
        std::sort(
            students.begin(),
            students.end(),
            [](const BenchmarkStudent& a, const BenchmarkStudent& b)
            {
                return a.finalGrade() > b.finalGrade();
            }
        );
    }

    void sortByFinalGrade(std::list<BenchmarkStudent>& students)
    {
        students.sort(
            [](const BenchmarkStudent& a, const BenchmarkStudent& b)
            {
                return a.finalGrade() > b.finalGrade();
            }
        );
    }

    template <typename Container>
    void writeResults(
        const Container& students,
        const std::string& containerName,
        int size)
    {
        const std::string suffix =
            containerName + "_" + std::to_string(size) + ".txt";

        std::ofstream passed("passed_" + suffix);
        std::ofstream failed("failed_" + suffix);

        if (!passed.is_open() || !failed.is_open())
            throw std::runtime_error(
                "Cannot create passed/failed output files for " + suffix
            );

        passed << std::fixed << std::setprecision(2);
        failed << std::fixed << std::setprecision(2);

        for (const BenchmarkStudent& student : students)
        {
            std::ostream& output =
                student.finalGrade() >= 5.0 ? passed : failed;

            output << student.name << ' '
                << student.surname << ' '
                << student.finalGrade() << '\n';
        }

        if (!passed.good() || !failed.good())
            throw std::runtime_error("Error writing benchmark output files.");
    }

    template <typename Container>
    double runOneBenchmark(
        const std::string& containerName,
        int size,
        const std::string& filename)
    {
        const auto start = std::chrono::steady_clock::now();

        Container students;

        // Read the dataset.
        readStudents(filename, students);

        // Sort by final grade, highest first.
        sortByFinalGrade(students);

        // Separate passed and failed students and write both files.
        writeResults(students, containerName, size);

        const auto finish = std::chrono::steady_clock::now();

        return std::chrono::duration<double>(finish - start).count();
    }
}

void ContainerBenchmark::runAllBenchmarks()
{
    std::ofstream results("container_benchmark_results.csv");

    if (!results.is_open())
        throw std::runtime_error(
            "Cannot create container_benchmark_results.csv"
        );

    results << "DatasetSize,Container,TotalSeconds\n";

    std::cout << "\n===== CONTAINER SPEED COMPARISON =====\n";
    std::cout << "Timing includes reading, sorting, splitting and writing.\n\n";

    std::cout << std::left
        << std::setw(14) << "Students"
        << std::setw(14) << "Vector (s)"
        << std::setw(14) << "List (s)"
        << std::setw(14) << "Deque (s)"
        << '\n';

    std::cout << std::string(56, '-') << '\n';

    for (int size : DATASET_SIZES)
    {
        try
        {
            const std::string filename = findDataset(size);

            const double vectorTime =
                runOneBenchmark<std::vector<BenchmarkStudent>>(
                    "vector", size, filename);

            const double listTime =
                runOneBenchmark<std::list<BenchmarkStudent>>(
                    "list", size, filename);

            const double dequeTime =
                runOneBenchmark<std::deque<BenchmarkStudent>>(
                    "deque", size, filename);

            std::cout << std::left
                << std::setw(14) << size
                << std::setw(14) << std::fixed << std::setprecision(3)
                << vectorTime
                << std::setw(14) << listTime
                << std::setw(14) << dequeTime
                << '\n';

            results << size << ",vector," << vectorTime << '\n';
            results << size << ",list," << listTime << '\n';
            results << size << ",deque," << dequeTime << '\n';
            results.flush();
        }
        catch (const std::exception& error)
        {
            std::cerr << "\nDataset " << size
                << " failed: " << error.what() << '\n';
        }
    }

    std::cout << "\nBenchmark finished.\n";
    std::cout << "Results saved to container_benchmark_results.csv\n";
    std::cout << "Output files: passed_<container>_<size>.txt and "
        "failed_<container>_<size>.txt\n";
}
