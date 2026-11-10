#include "BenchmarkProcessor.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
    struct BenchmarkStudent
    {
        std::uint32_t id{};
        std::array<int, 5> homework{};
        int examGrade{};
        double finalGrade{};
    };

    void generateDataset(
        std::size_t count,
        std::mt19937& generator)
    {
        const std::string filename =
            "students" + std::to_string(count) + ".txt";

        std::ofstream file(filename);

        if (!file.is_open())
        {
            throw std::runtime_error(
                "Could not open " + filename + " for writing.");
        }

        file << "Name Surname HW1 HW2 HW3 HW4 HW5 Exam\n";

        std::uniform_int_distribution<int> gradeDistribution(1, 10);

        for (std::size_t i = 1; i <= count; ++i)
        {
            file << "Student" << i
                << " Surname" << i;

            for (int j = 0; j < 5; ++j)
            {
                file << ' ' << gradeDistribution(generator);
            }

            file << ' ' << gradeDistribution(generator) << '\n';
        }

        if (!file)
        {
            throw std::runtime_error(
                "An error occurred while writing " + filename + ".");
        }

        std::cout << "Generated " << filename
            << " (" << count << " students).\n";
    }

    std::uint32_t getStudentId(const std::string& name)
    {
        if (name.rfind("Student", 0) != 0)
        {
            throw std::runtime_error(
                "Invalid student name in dataset: " + name);
        }

        std::size_t charactersRead = 0;
        unsigned long id = std::stoul(name.substr(7), &charactersRead);

        if (charactersRead != name.size() - 7 ||
            id == 0 ||
            id > UINT32_MAX)
        {
            throw std::runtime_error(
                "Invalid student ID in dataset: " + name);
        }

        return static_cast<std::uint32_t>(id);
    }

    void writeStudent(
        std::ostream& file,
        const BenchmarkStudent& student)
    {
        file << "Student" << student.id
            << " Surname" << student.id;

        for (std::size_t i = 0; i < student.homework.size(); ++i)
        {
            file << ' ' << student.homework.at(i);
        }

        file << ' ' << student.examGrade
            << ' ' << std::fixed << std::setprecision(2)
            << student.finalGrade << '\n';
    }

    void processDataset(std::size_t expectedCount)
    {
        const std::string inputFilename =
            "students" + std::to_string(expectedCount) + ".txt";

        const std::string passedFilename =
            "passed_" + std::to_string(expectedCount) + ".txt";

        const std::string failedFilename =
            "failed_" + std::to_string(expectedCount) + ".txt";

        const auto start = std::chrono::steady_clock::now();

        std::ifstream inputFile(inputFilename);

        if (!inputFile.is_open())
        {
            throw std::runtime_error(
                "Could not open " + inputFilename + " for reading.");
        }

        std::string header;

        if (!std::getline(inputFile, header))
        {
            throw std::runtime_error(
                "Could not read the header from " + inputFilename + ".");
        }

        std::vector<BenchmarkStudent> students;
        students.reserve(expectedCount);

        std::string name;
        std::string surname;

        while (inputFile >> name >> surname)
        {
            BenchmarkStudent student;
            student.id = getStudentId(name);

            for (std::size_t i = 0; i < student.homework.size(); ++i)
            {
                int grade;

                if (!(inputFile >> grade))
                {
                    throw std::runtime_error(
                        "Missing homework grade in " + inputFilename + ".");
                }

                if (grade < 1 || grade > 10)
                {
                    throw std::runtime_error(
                        "Invalid homework grade in " + inputFilename + ".");
                }

                student.homework.at(i) = grade;
            }

            if (!(inputFile >> student.examGrade))
            {
                throw std::runtime_error(
                    "Missing exam grade in " + inputFilename + ".");
            }

            if (student.examGrade < 1 || student.examGrade > 10)
            {
                throw std::runtime_error(
                    "Invalid exam grade in " + inputFilename + ".");
            }

            double totalHomework = 0.0;

            for (std::size_t i = 0; i < student.homework.size(); ++i)
            {
                totalHomework += student.homework.at(i);
            }

            const double homeworkAverage =
                totalHomework / student.homework.size();

            student.finalGrade =
                homeworkAverage * 0.4 + student.examGrade * 0.6;

            students.push_back(student);
        }

        if (inputFile.bad())
        {
            throw std::runtime_error(
                "An error occurred while reading " + inputFilename + ".");
        }

        if (students.size() != expectedCount)
        {
            throw std::runtime_error(
                "The number of students read from " + inputFilename +
                " does not match the expected count.");
        }

        std::sort(
            students.begin(),
            students.end(),
            [](const BenchmarkStudent& first,
                const BenchmarkStudent& second)
            {
                return first.finalGrade > second.finalGrade;
            });

        std::ofstream passedFile(passedFilename);
        std::ofstream failedFile(failedFilename);

        if (!passedFile.is_open() || !failedFile.is_open())
        {
            throw std::runtime_error(
                "Could not open the passed or failed output file.");
        }

        passedFile
            << "Name Surname HW1 HW2 HW3 HW4 HW5 Exam FinalGrade\n";

        failedFile
            << "Name Surname HW1 HW2 HW3 HW4 HW5 Exam FinalGrade\n";

        std::size_t passedCount = 0;
        std::size_t failedCount = 0;

        for (const BenchmarkStudent& student : students)
        {
            if (student.finalGrade >= 5.0)
            {
                writeStudent(passedFile, student);
                ++passedCount;
            }
            else
            {
                writeStudent(failedFile, student);
                ++failedCount;
            }
        }

        if (!passedFile || !failedFile)
        {
            throw std::runtime_error(
                "An error occurred while writing the result files.");
        }

        const auto finish = std::chrono::steady_clock::now();

        const double seconds =
            std::chrono::duration<double>(finish - start).count();

        std::cout << "\nDataset: " << inputFilename
            << "\nStudents processed: " << students.size()
            << "\nPassed: " << passedCount
            << "\nFailed: " << failedCount
            << "\nPassed file: " << passedFilename
            << "\nFailed file: " << failedFilename
            << "\nProcessing time: " << std::fixed
            << std::setprecision(3) << seconds << " seconds\n";
    }
}

void BenchmarkProcessor::runVectorBenchmark()
{
    const std::vector<std::size_t> datasetSizes =
    {
        1000,
        10000,
        100000,
        1000000,
        10000000
    };

    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());

    std::cout << "\n===== VECTOR DATASET BENCHMARK =====\n";

    for (std::size_t count : datasetSizes)
    {
        std::cout << "\nPreparing " << count << " students...\n";

        try
        {
            generateDataset(count, generator);
            processDataset(count);
        }
        catch (const std::exception& error)
        {
            std::cerr << "\nCould not process " << count
                << " students: " << error.what() << '\n';
        }
    }

    std::cout << "\nDataset benchmark finished.\n";
}