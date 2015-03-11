#include "readFile.h"
#include "shapemanager.h"
using namespace std;
bool readOneLine(ifstream& inFile, stringstream &ss);
void readTwoNumberForProperty(ifstream& inFile,const string& nameOfProperty,double& x,double& y){
    inFile>>x>>y;
    if(!inFile){
        cout<<"fail to load property: "<<nameOfProperty<<endl;
        exit(1);
    }
}
void readOneNumberForProperty(ifstream& inFile,const string& nameOfProperty,double& x){
    inFile>>x;
    if(!inFile){
        cout<<"fail to load property: "<<nameOfProperty<<endl;
        exit(1);
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

void FileReader::readFile(string fileName, ShapeManager &shapeManager){
    ifstream inFile(fileName.c_str(),std::ios::binary);

    string s;


    while(readOneWord(inFile,s)){
        ShapeDefinition shapeDef;

        //read the beginning of a declaration: shapetype
        if (0==s.compare("Polygon")) {
            shapeDef.shapeType=POLYGON;
        }
        else if (0==s.compare("Circle")) {
            shapeDef.shapeType=CIRCLE;
        }
        else if (s.empty()) {
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
                switch (shapeDef.shapeType) {
                case POLYGON:
                    shapeManager.creatPoly(shapeDef);
                    break;
                case CIRCLE:
                    shapeManager.creatCircle(shapeDef);
                }//switch

                break;//quit the "while" loop to start read a new Shape
            }

            else if (0==s.compare("vertex:")) {
                double x,y;
                //read those vertice coordinates
                while (inFile>>x>>y){
                    shapeDef.vertexList.push_back(Vector2(x,y));
                }
                inFile.clear();
                if (shapeDef.vertexList.size()<3) {
                    cout<<"number of vertex is less than 3 !";
                    exit(1);
                }
            }
            else if (0==s.compare("vel:")) {
                readTwoNumberForProperty(inFile,"vel",shapeDef.velocity.x,shapeDef.velocity.y);
            }
            else if (0==s.compare("mass:")) {
                readOneNumberForProperty(inFile,"mass",shapeDef.mass);
                if(shapeDef.mass<=0){
                    cout<<"mass should be positive"<<endl;
                    exit(0);
                }
            }
            else if (0==s.compare("move:")) {
                readTwoNumberForProperty(inFile,"move",shapeDef.move.x,shapeDef.move.y);
            }
            else if (0==s.compare("ang_vel:")) {
                readOneNumberForProperty(inFile,"ang_vel",shapeDef.angVel);
            }
            else if (0==s.compare("radius:")) {
                readOneNumberForProperty(inFile,"radius",shapeDef.radius);
            }
            else if (0==s.compare("center:")) {
                readTwoNumberForProperty(inFile,"center",shapeDef.circleCenter.x,shapeDef.circleCenter.y);
            }
            else if (0==s.compare("rotate:")) {
                readOneNumberForProperty(inFile,"rotate",shapeDef.rotate);
            }
            else if (0==s.compare("fix:")) {
                double x;
                readOneNumberForProperty(inFile,"fix",x);
                if (x==1) shapeDef.fixToGroud=true;
            }
            else if (0==s.compare("restitution:")) {
                readOneNumberForProperty(inFile,"restitution",shapeDef.restitution);
            }
            else if (0==s.compare("friction:")) {
                readOneNumberForProperty(inFile,"friction",shapeDef.friction);
            }
            else {
                cout<<"Unknown property:"<<s<<endl;
                exit(1);
            }
        }
    }

}

