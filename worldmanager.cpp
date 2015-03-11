#include "worldmanager.h"
#include "readFile.h"

WorldManager::WorldManager(double dt, Vector2 gravity):
    simulator(8,8)
{
    this->gravity=gravity;
    this->dt=dt;
}

void WorldManager::OneStep()
{
    shapeManager.applyGravity(gravity);
    std::vector<MyShape*> shapeList=shapeManager.getShapeList();
    simulator.solve(shapeList,dt);
    shapeManager.clearAllForce();

}

void WorldManager::readFile(std::string fileName)
{
    FileReader reader;
    reader.readFile(fileName,shapeManager);

}

std::vector<MyShape *> WorldManager::getShapeList()
{
    return shapeManager.getShapeList();
}

