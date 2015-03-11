#include "readFile.h"
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

void readFile(vector<MyShape*> &ShapeList) {
    ifstream inFile("data.txt",std::ios::binary);
    string s;
    stringstream ss;
    ShapeType shapeType;
    bool setVertex, setMove, setVel, setAngVel, setFix,\
            setRotate, setMoveTo, setMass, setRestitution,setFriction,setRadius,setCircleCenter;
    vector<Vector2 > vt;
    Vector2 move, vel, moveTo,circleCenter;
    bool fix;
    double rotate, angVel, mass, restitution,friction,radius;
    while(readOneWord(inFile,s)){

        //read the beginning of a declaration: shapetype
        if (0==s.compare("Polygon")) {
            shapeType=POLYGON;
        }
        else if (0==s.compare("Circle")) {
            shapeType=CIRCLE;
        }
        else if (s.empty()) {
            continue;
        }
        else if(0==s.compare("stop")){
            return;
        }else{
            cout<<"Read file error: Unknown shape type!"<<endl;
        }
        //initialize properties
        vt.clear();
        move.x=move.y=0;
        vel.x=vel.y=0;
        moveTo.x=moveTo.y=0;
        fix=false;
        rotate=0;
        angVel=0;
        mass=1;
        restitution=0.5;
        friction=0.1;
        setVertex=setMove=setVel=setAngVel=setFix=setRotate=\
                setMoveTo=setRestitution=setFriction=setRadius=false;
        //start to read those properties
        while (readOneWord(inFile,s)) {
            //check if the declaration of current shape terminates
            if (s.empty()) {
                continue;
            }
            if (0==s.compare("end")) {
                switch (shapeType) {
                case POLYGON:
                {
                    ShapeList.push_back(new MyPolygon(mass,vt));

                    //create one Polygon
                    MyShape& s=*ShapeList.back();
                    if (setMove) s.move(move);
                    if (setMoveTo) s.moveTo(moveTo);
                    s.angVel=angVel;
                    if (setRotate) s.rotate(rotate);
                    s.vel=vel;
                    if(fix){
                        s.fixToGround=true;
                        s.invMass=0;
                        s.mass=0;
                        s.I=0;
                        s.invI=0;
                        s.vel.x=s.vel.y=0;
                    }
                    s.restitution=restitution;
                    s.friction=friction;
                    break;
                }
                case CIRCLE:
                {
                    ShapeList.push_back(new MyCircle(radius,circleCenter,mass));

                    //create one Polygon
                    MyShape& s=*ShapeList.back();
                    if (setMove) s.move(move);
                    if (setMoveTo) s.moveTo(moveTo);
                    s.angVel=angVel;
                    if (setRotate) s.rotate(rotate);
                    s.vel=vel;
                    if(fix){
                        s.fixToGround=true;
                        s.invMass=0;
                        s.mass=0;
                        s.I=0;
                        s.invI=0;
                        s.vel.x=s.vel.y=0;
                    }
                    s.restitution=restitution;
                    s.friction=friction;
                    break;
                }
                }//switch

                break;//quit the loop to start read a new Shape
            }

            else if (0==s.compare("vertex:")) {
                setVertex=true;
                double x,y;
                //read those vertice coordinates
                while (inFile>>x>>y){
                    vt.push_back(Vector2(x,y));
                }
                inFile.clear();
                if (vt.size()<3) {
                    cout<<"number of vertex is less than 3 !";
                    exit(1);
                }
            }
            else if (0==s.compare("vel:")) {
                setVel=true;
                readTwoNumberForProperty(inFile,"vel",vel.x,vel.y);
            }
            else if (0==s.compare("mass:")) {
                setMass=true;
                readOneNumberForProperty(inFile,"mass",mass);
            }
            else if (0==s.compare("move:")) {
                setMove=true;
                readTwoNumberForProperty(inFile,"move",move.x,move.y);
            }
            else if (0==s.compare("ang_vel:")) {
                setAngVel=true;
                readOneNumberForProperty(inFile,"ang_vel",angVel);
            }
            else if (0==s.compare("moveto:")) {
                setMoveTo=true;
                readTwoNumberForProperty(inFile,"moveto",moveTo.x,moveTo.y);
            }
            else if (0==s.compare("radius:")) {
                setRadius=true;
                readOneNumberForProperty(inFile,"radius",radius);
            }
            else if (0==s.compare("center:")) {
                setCircleCenter=true;
                readTwoNumberForProperty(inFile,"center",circleCenter.x,circleCenter.y);
            }
            else if (0==s.compare("rotate:")) {
                setRotate=true;
                readOneNumberForProperty(inFile,"rotate",rotate);
            }
            else if (0==s.compare("fix:")) {
                setFix=true;
                double x;
                readOneNumberForProperty(inFile,"fix",x);
                if (x==1) fix=true;
            }
            else if (0==s.compare("restitution:")) {
                setRestitution=true;
                readOneNumberForProperty(inFile,"restitution",restitution);
            }
            else if (0==s.compare("friction:")) {
                setFriction=true;
                readOneNumberForProperty(inFile,"friction",friction);
            }
            else {
                cout<<"Unknown property"<<endl;
                exit(1);
            }
        }
    }

}
