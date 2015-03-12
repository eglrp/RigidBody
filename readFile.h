

#ifndef READFILE_H
#define READFILE_H
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include "algebra.h"
#include <vector>
class ShapeManager;
struct ShapeDefinition;
struct FileShapeDefinition{
    enum FileShapeType {F_POLYGON,F_CIRCLE,F_RECT,F_SQUARE};
    FileShapeType shapeType;
    std::vector<Vector2 > vertexList;//for polygon only
    Vector2 move, velocity;
    Vector2 center;//used for circle, rectangle and square.
    bool fixToGroud;

    double side;//for square only
    double width,height;//for rectangle only
    double radius;//for circle only

    double rotate, angVel, mass, restitution,friction,density;
    FileShapeDefinition();
    ShapeDefinition translateDefinition();
};
struct FileWorldDefinition{
    Vector2 gravity;
    bool debugMode;
    double frequency;
    double speed;
    FileWorldDefinition();
};

class FileReader{
    void readShape(std::ifstream& inFile,ShapeManager& shapeManager);
    void readSetting(std::ifstream& inFile,FileWorldDefinition& fSettings);
public:
    void readFile(std::string fileName,FileWorldDefinition& fSettings,ShapeManager& shapeManager) ;

};



#endif /* READFILE_H*/
