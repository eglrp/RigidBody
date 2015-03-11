

#ifndef READFILE_H
#define READFILE_H
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>

class ShapeManager;
class FileReader{
public:
    void readFile(std::string fileName,ShapeManager& shapeManager) ;
};



#endif /* READFILE_H*/
