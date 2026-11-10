
#include "ContainerBenchmark.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <deque>
#include <string>
#include <algorithm>
#include <iterator>
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

        return 0.4 * (sum / homework.size()) + 0.6 * exam;
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
            "Cannot find dataset for " + number + " students."
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
            throw std::runtime_error("Invalid student grades: " + line);

        student.exam = grades.back();
        grades.pop_back();
        student.homework = grades;

        return student;
    }

    template <typename Container>
    void readStudents(
        const std::string& filename,
        Container& students)
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

            if (firstLine)
            {
                firstLine = false;

                if (line.find("Name") != std::string::npos &&
                    line.find("Surname") != std::string::npos)
                    continue;
            }

            students.push_back(parseStudentLine(line));
        }

        if (file.bad())
            throw std::runtime_error("Error reading input file: " + filename);

        if (students.empty())
            throw std::runtime_error("Dataset is empty: " + filename);
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
        const Container& passed,
        const Container& failed,
        const std::string& containerName,
        int size)
    {
        const std::string suffix =
            containerName + "_" + std::to_string(size) + ".txt";

        std::ofstream passedFile("passed_" + suffix);
        std::ofstream failedFile("failed_" + suffix);

        if (!passedFile.is_open() || !failedFile.is_open())
            throw std::runtime_error("Cannot create original benchmark output files.");

        passedFile << std::fixed << std::setprecision(2);
        failedFile << std::fixed << std::setprecision(2);

        for (const auto& student : passed)
        {
            passedFile << student.name << ' '
                << student.surname << ' '
                << student.finalGrade() << '\n';
        }

        for (const auto& student : failed)
        {
            failedFile << student.name << ' '
                << student.surname << ' '
                << student.finalGrade() << '\n';
        }

        if (!passedFile.good() || !failedFile.good())
            throw std::runtime_error("Error writing original benchmark output files.");
    }

    template <typename Container>
    double runOriginalBenchmark(
        const std::string& containerName,
        int size,
        const std::string& filename)
    {
        const auto start = std::chrono::steady_clock::now();

        Container students;
        Container passed;
        Container failed;

        // Read students.
        readStudents(filename, students);

        // Sort students by final grade.
        sortByFinalGrade(students);

        // Copy passing students into the passed container.
        std::copy_if(
            students.begin(),
            students.end(),
            std::back_inserter(passed),
            [](const BenchmarkStudent& student)
            {
                return student.finalGrade() >= 5.0;
            }
        );

        // Copy failing students into the failed container.
        std::copy_if(
            students.begin(),
            students.end(),
            std::back_inserter(failed),
            [](const BenchmarkStudent& student)
            {
                return student.finalGrade() < 5.0;
            }
        );

        // Write the results.
        writeResults(passed, failed, containerName, size);

        const auto finish = std::chrono::steady_clock::now();

        return std::chrono::duration<double>(
            finish - start
        ).count();
    }

    template <typename Container>
    double runStrategy1Benchmark(
        const std::string& containerName,
        int size,
        const std::string& filename)
    {
        const auto start = std::chrono::steady_clock::now();

        Container students;
        Container passed;
        Container failed;

        // Load students into the original container.
        readStudents(filename, students);

        // Sort students by final grade.
        sortByFinalGrade(students);

        // Copy passing students.
        std::copy_if(
            students.begin(),
            students.end(),
            std::back_inserter(passed),
            [](const BenchmarkStudent& student)
            {
                return student.finalGrade() >= 5.0;
            }
        );

        // Copy failing students.
        std::copy_if(
            students.begin(),
            students.end(),
            std::back_inserter(failed),
            [](const BenchmarkStudent& student)
            {
                return student.finalGrade() < 5.0;
            }
        );

        const std::string suffix =
            containerName + "_" + std::to_string(size) + ".txt";

        std::ofstream passedFile("strategy1_passed_" + suffix);
        std::ofstream failedFile("strategy1_failed_" + suffix);

        if (!passedFile.is_open() || !failedFile.is_open())
            throw std::runtime_error("Cannot create Strategy 1 output files.");

        passedFile << std::fixed << std::setprecision(2);
        failedFile << std::fixed << std::setprecision(2);

        for (const auto& student : passed)
        {
            passedFile << student.name << ' '
                << student.surname << ' '
                << student.finalGrade() << '\n';
        }

        for (const auto& student : failed)
        {
            failedFile << student.name << ' '
                << student.surname << ' '
                << student.finalGrade() << '\n';
        }

        if (!passedFile.good() || !failedFile.good())
            throw std::runtime_error("Error writing Strategy 1 output files.");

        const auto finish = std::chrono::steady_clock::now();

        return std::chrono::duration<double>(
            finish - start
        ).count();
    }
}

void ContainerBenchmark::runAllBenchmarks()
{
    std::ofstream results("container_benchmark_results.csv");

    if (!results.is_open())
        throw std::runtime_error("Cannot create container benchmark results CSV.");

    results << "DatasetSize,Container,TotalSeconds\n";

    std::cout << "\n===== ORIGINAL V0.25 CONTAINER BENCHMARK =====\n";
    std::cout << "Timing includes reading, sorting, copying and writing.\n\n";

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
                runOriginalBenchmark<std::vector<BenchmarkStudent>>(
                    "vector", size, filename);

            const double listTime =
                runOriginalBenchmark<std::list<BenchmarkStudent>>(
                    "list", size, filename);

            const double dequeTime =
                runOriginalBenchmark<std::deque<BenchmarkStudent>>(
                    "deque", size, filename);

            std::cout << std::left
                << std::setw(14) << size
                << std::setw(14) << std::fixed << std::setprecision(4)
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

    std::cout << "\nOriginal benchmark completed.\n";
    std::cout << "Results saved in container_benchmark_results.csv\n";
}

void ContainerBenchmark::runStrategy1()
{
    std::ofstream results("strategy1_benchmark_results.csv");

    if (!results.is_open())
        throw std::runtime_error("Cannot create Strategy 1 results CSV.");

    results << "DatasetSize,Container,TotalSeconds\n";

    std::cout << "\n===== STRATEGY 1 BENCHMARK =====\n";
    std::cout << "Original students container remains unchanged.\n\n";

    for (int size : DATASET_SIZES)
    {
        try
        {
            const std::string filename = findDataset(size);

            std::cout << "\nDataset: " << size << " students\n";

            const double vectorTime =
                runStrategy1Benchmark<std::vector<BenchmarkStudent>>(
                    "vector", size, filename);

            const double listTime =
                runStrategy1Benchmark<std::list<BenchmarkStudent>>(
                    "list", size, filename);

            const double dequeTime =
                runStrategy1Benchmark<std::deque<BenchmarkStudent>>(
                    "deque", size, filename);

            std::cout << std::fixed << std::setprecision(4);
            std::cout << "Vector: " << vectorTime << " seconds\n";
            std::cout << "List:   " << listTime << " seconds\n";
            std::cout << "Deque:  " << dequeTime << " seconds\n";

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

    std::cout << "\nStrategy 1 benchmark completed.\n";
    std::cout << "Results saved in strategy1_benchmark_results.csv\n";
}
