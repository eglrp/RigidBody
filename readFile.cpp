#include "readFile.h"
#include "shapemanager.h"
#include <stdexcept>
using namespace std;
bool readOneWord(ifstream& inFile,string& s);
bool readOneLine(ifstream& inFile, stringstream &ss);
void readTwoNumberForProperty(ifstream& inFile,const string& nameOfProperty,double& x,double& y){
    inFile>>x>>y;
    if(!inFile){
        cout<<"fail to load property: "<<nameOfProperty<<endl;
        throw logic_error("unknown shape type");
    }
}
void readOneNumberForProperty(ifstream& inFile,const string& nameOfProperty,double& x){
    inFile>>x;
    if(!inFile){
        cout<<"fail to load property: "<<nameOfProperty<<endl;
        throw logic_error("fail to load property: ");
    }
}
bool readOneWord(ifstream& inFile,string& s){
    s.clear();
    inFile>>s;
    size_t found=s.find("//");
    if(found!=string::npos){
        string ignore;
        getline(inFile,ignore);
        s=s.substr(0,found);
    }

    if(inFile.eof())return false;
    inFile.clear();
    return true;
}

void FileReader::readShape(ifstream &inFile, ShapeManager &shapeManager)
{
    string s;
    while(readOneWord(inFile,s)){
        FileShapeDefinition fDef;

        //read the beginning of a declaration: shapetype
        if (0==s.compare("Polygon")) {
            fDef.shapeType=FileShapeDefinition::F_POLYGON;
        }
        else if (0==s.compare("Circle")) {
            fDef.shapeType=FileShapeDefinition::F_CIRCLE;
        }else if (0==s.compare("Rect")){
            fDef.shapeType=FileShapeDefinition::F_RECT;
        }else if (0==s.compare("Square")){
            fDef.shapeType=FileShapeDefinition::F_SQUARE;
        }else if (s.empty()) {
            continue;
        }
        else if(0==s.compare("stop")){
            return;
        }else{
            cout<<"Unknown shape type:"<<s<<endl;
        }

        //start to read properties
        while (readOneWord(inFile,s)) {
            //check if the declaration of current shape terminates
            if (s.empty()) {
                continue;
            }
            if (0==s.compare("end")) {
                shapeManager.creatShape(fDef.translateDefinition());
                break;//quit the "while" loop to start read a new Shape
            }

            else if (0==s.compare("vertex:")) {
                double x,y;
                //read those vertice coordinates
                while (inFile>>x>>y){
                    fDef.vertexList.push_back(Vector2(x,y));
                }
                inFile.clear();
                if (fDef.vertexList.size()<3) {
                    cout<<"number of vertex is less than 3 !";
                    throw logic_error("number of vertex is less than 3 !");
                }
            }
            else if (0==s.compare("vel:")) {
                readTwoNumberForProperty(inFile,"vel",fDef.velocity.x,fDef.velocity.y);
            }
            else if (0==s.compare("mass:")) {
                readOneNumberForProperty(inFile,"mass",fDef.mass);
                if(fDef.mass<=0){
                    cout<<"mass should be positive"<<endl;
                    throw logic_error("mass should be positive");
                }
                fDef.density=0;//abandon density
            }else if (0==s.compare("density:")) {
                readOneNumberForProperty(inFile,"density",fDef.density);
                if(fDef.density<=0){
                    cout<<"density should be positive"<<endl;
                    throw logic_error("density should be positive");
                }
                fDef.mass=0;//abandon mass
            }
            else if (0==s.compare("move:")) {
                readTwoNumberForProperty(inFile,"move",fDef.move.x,fDef.move.y);
            }
            else if (0==s.compare("ang_vel:")) {
                readOneNumberForProperty(inFile,"ang_vel",fDef.angVel);
            }
            else if (0==s.compare("radius:")) {
                readOneNumberForProperty(inFile,"radius",fDef.radius);
            }
            else if (0==s.compare("center:")) {
                readTwoNumberForProperty(inFile,"center",fDef.center.x,fDef.center.y);
            }
            else if (0==s.compare("rotate:")) {
                readOneNumberForProperty(inFile,"rotate",fDef.rotate);
            }
            else if (0==s.compare("fix:")) {
                double x;
                readOneNumberForProperty(inFile,"fix",x);
                if (x==1) fDef.fixToGroud=true;
            }
            else if (0==s.compare("restitution:")) {
                readOneNumberForProperty(inFile,"restitution",fDef.restitution);
            }
            else if (0==s.compare("friction:")) {
                readOneNumberForProperty(inFile,"friction",fDef.friction);
            }
            else if (0==s.compare("side:")) {
                readOneNumberForProperty(inFile,"side",fDef.side);
            }
            else if (0==s.compare("width:")) {
                readOneNumberForProperty(inFile,"width",fDef.width);
            }
            else if (0==s.compare("height:")) {
                readOneNumberForProperty(inFile,"height",fDef.height);
            }
            else {
                cout<<"Unknown property:"<<s<<endl;
                throw logic_error("Unknown property:");
            }
        }
    }

}

void FileReader::readSetting(ifstream &inFile, FileWorldDefinition &fSettings)
{
    string s;
    while (readOneWord(inFile,s)) {
        if (s.empty()) {
            continue;
        }
        if (0==s.compare("endsetting")) {
            return;
        }else if (0==s.compare("gravity:")) {
            readTwoNumberForProperty(inFile,"gravity",fSettings.gravity.x,fSettings.gravity.y);
        }
        else if (0==s.compare("debug:")) {
            double flag;
            readOneNumberForProperty(inFile,"debug",flag);
            if(flag!=0)fSettings.debugMode=true;
        }else if (0==s.compare("speed:")) {
            readOneNumberForProperty(inFile,"speed",fSettings.speed);
        }else if (0==s.compare("frequency:")) {
            readOneNumberForProperty(inFile,"frequency",fSettings.frequency);
        }
        else {
            cout<<"Unknown setting:"<<s<<endl;

            throw logic_error("Unknown setting:");
        }
    }
}

void FileReader::readFile(string fileName, FileWorldDefinition &fSettings, ShapeManager &shapeManager){
    ifstream inFile(fileName.c_str(),std::ios::binary);
    if(!inFile.is_open()){
        cout<<"can't find file : "<<fileName<<endl;
        throw logic_error("Can't find file");
    }
    readSetting(inFile,fSettings);
    readShape(inFile,shapeManager);

}



FileShapeDefinition::FileShapeDefinition()
{
    //default values
    move=Vector2(0,0);
    velocity=Vector2(0,0);
    move=Vector2(0,0);
    center=Vector2(0,0);
    fixToGroud=false;
    rotate=0;
    angVel=0;
    mass=1;
    restitution=0.5;
    friction=0.5;
    radius=1;
    side=1;
    width=2;
    height=1;
    density=0;//=0 means we use mass
}

ShapeDefinition FileShapeDefinition::translateDefinition()
{
    ShapeDefinition sDef;
    sDef.angVel=angVel;
    sDef.friction=friction;
    sDef.mass=mass;
    sDef.density=density;
    sDef.move=move;
    sDef.restitution=restitution;
    sDef.rotate=rotate;
    sDef.velocity=velocity;
    sDef.fixToGroud=fixToGroud;

    switch (shapeType) {
    case F_POLYGON:
        sDef.shapeType=POLYGON;
        sDef.vertexList=vertexList;
        break;
    case F_CIRCLE:
        sDef.shapeType=CIRCLE;
        sDef.radius=radius;
        sDef.circleCenter=center;
        break;
    case F_SQUARE:{
        sDef.shapeType=POLYGON;
        vertexList.clear();
        vertexList.push_back(Vector2(-side/2,side/2)+center);
        vertexList.push_back(Vector2(-side/2,-side/2)+center);
        vertexList.push_back(Vector2(side/2,-side/2)+center);
        vertexList.push_back(Vector2(side/2,side/2)+center);
        sDef.vertexList=vertexList;
    }
        break;
    case F_RECT:{
        sDef.shapeType=POLYGON;
        vertexList.clear();
        vertexList.push_back(Vector2(-width/2,height/2)+center);
        vertexList.push_back(Vector2(-width/2,-height/2)+center);
        vertexList.push_back(Vector2(width/2,-height/2)+center);
        vertexList.push_back(Vector2(width/2,height/2)+center);
        sDef.vertexList=vertexList;
    }

        break;
    default:
        break;
    }
    return sDef;

}


FileWorldDefinition::FileWorldDefinition()
{
    gravity=Vector2(0,-10);
    frequency=50.0;
    speed=1;
    debugMode=false;
}


