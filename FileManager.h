
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <vector>
#include "Person.h"

class FileManager
{
public:
    static void saveStudents(const std::vector<Person>& students);
    static void loadStudents(std::vector<Person>& students);
};

#endif

