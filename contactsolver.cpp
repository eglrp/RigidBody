#include "contactsolver.h"
#include <iostream>
#include "graphics.h"
#define MY_DEBUG
#define DEBUG_TIME_INTERVAL 20
using namespace std;
void ContactSolver::OneVelocitySolveIteration(vector<ContactConstraint> &allConstraints, std::vector<MyShape *> &shapes)
{
    for(vector<ContactConstraint>::iterator it=allConstraints.begin();it<allConstraints.end();it++){
        ContactConstraint& cc=*it;

        int indexA=cc.shapeIndexA;
        int indexB=cc.shapeIndexB;
        double iMA=cc.invMassA;
        double iMB=cc.invMassB;
        double iIA=cc.invIA;
        double iIB=cc.invIB;
        int pointCount=cc.pointCount;

        MyShape& a=*shapes[indexA];
        MyShape& b=*shapes[indexB];
        Vector2 vA=a.vel;
        Vector2 vB=b.vel;
        double avA=a.angVel;
        double avB=b.angVel;

        Vector2 n=cc.normal;
        Vector2 t=crossProd2D(n,1.0);

        double friction=cc.friction;
        //Solve tangent constraints

        for(int j=0;j<pointCount;j++){
            ContactPoint &cp=cc.points[j];
            Vector2 dv=vB+crossProd2D(avB,cp.rB)-vA-crossProd2D(avA,cp.rA);

            double vt=dv*t;
            double lambda=cp.tangentMass*(-vt);

            double maxF=friction*cp.normalImpulse;
            double newImpulse=max(-maxF,min(cp.tangentImpulse+lambda,maxF));
            lambda=newImpulse-cp.tangentImpulse;
            cp.tangentImpulse=newImpulse;
            Vector2 P=lambda*t;
            vA-=iMA*P;
            avA-=iIA*crossProd2D(cp.rA,P);

            vB+=iMB*P;
            avB+=iIB*crossProd2D(cp.rB,P);
        }


        //Solve normal constraints
        if(pointCount==1){

            ContactPoint& cp=cc.points[0];
            Vector2 dv=vB+crossProd2D(avB,cp.rB)-vA-crossProd2D(avA,cp.rA);
            double vn=dv*n;
            double lambda=-cp.normalMass*(vn-cp.separatingVel);

            //only separating Impulse is allowed
            double newImpulse=max(cp.normalImpulse+lambda,0.0);
            lambda=newImpulse-cp.normalImpulse;
            cp.normalImpulse=newImpulse;

            //apply impulse on shapes
            Vector2 P=lambda*n;
            vA-=iMA*P;
            avA-=iIA*crossProd2D(cp.rA,P);
            vB+=iMB*P;
            avB+=iIB*crossProd2D(cp.rB,P);
        }else{
            //pointCount==2, that means we have two contacts between two shapes
            //|---------------------------------------------------|
            //|Algorithm from open source 2D physic engine "Box2D"|
            //|---------------------------------------------------|
            //this is a LCP problem
            //
            // v = A * x + b
            // v,A,b are known;
            // v[1] >= 0, v[2] >= 0
            // x[1] >= 0, x[2] >= 0
            // v[1]*x[1]=0, v[2]*x[2]=0
            //
            // Substitute:
            //
            // x = f + d
            //
            // f := old total impulse
            // x := new total impulse
            // d := incremental impulse
            // vn = A * d + b
            //    = A * (x - a) + b
            //    = A * x + b - A * a
            //    = A * x + b'
            // b' = b - A * a;
            ContactPoint &cp1=cc.points[0];
            ContactPoint &cp2=cc.points[1];
            Vector2 f(cp1.normalImpulse,cp2.normalImpulse);
            Vector2 dv1=vB+crossProd2D(avB,cp1.rB)-vA-crossProd2D(avA,cp1.rA);
            Vector2 dv2=vB+crossProd2D(avB,cp2.rB)-vA-crossProd2D(avA,cp2.rA);
            //Relative normal velocity
            double vn1=dv1*n;
            double vn2=dv2*n;

            Vector2 b;
            b.x=vn1-cp1.separatingVel;
            b.y=vn2-cp2.separatingVel;
            b-=cc.A*f;
            while(1){//no loop
                //
                // Case 1: vn = 0
                //
                // 0 = A * x + b'
                //
                // Solve for x:
                //
                // x = - inv(A) * b'
                //
                Vector2 x=-(cc.invA*b);
                if(x.x>=0 && x.y>=0){
                    // Get the incremental impulse
                    Vector2 d = x - f;

                    // Apply incremental impulse
                    Vector2 P1 = d.x * n;
                    Vector2 P2 = d.y * n;
                    vA -= iMA * (P1 + P2);
                    avA -= iIA * (crossProd2D(cp1.rA, P1) + crossProd2D(cp2.rA, P2));

                    vB += iMB * (P1 + P2);
                    avB += iIB * (crossProd2D(cp1.rB, P1) + crossProd2D(cp2.rB, P2));

                    // Accumulate
                    cp1.normalImpulse = x.x;
                    cp2.normalImpulse = x.y;
                    break;
                }
                //
                // Case 2: vn1 = 0 and x2 = 0
                //
                //   0 = a11 * x1 + a12 * 0 + b1'
                // vn2 = a21 * x1 + a22 * 0 + b2'
                //
                x.x = - cp1.normalMass * b.x;
                x.y = 0.0f;
                vn1 = 0.0f;
                vn2 = cc.A.m21 * x.x + b.y;

                if (x.x >= 0.0f && vn2 >= 0.0f)
                {
                    // Get the incremental impulse
                    Vector2 d = x - f;

                    // Apply incremental impulse
                    Vector2 P1 = d.x * n;
                    Vector2 P2 = d.y * n;
                    vA -= iMA * (P1 + P2);
                    avA -= iIA * (crossProd2D(cp1.rA, P1) + crossProd2D(cp2.rA, P2));

                    vB += iMB * (P1 + P2);
                    avB += iIB * (crossProd2D(cp1.rB, P1) + crossProd2D(cp2.rB, P2));

                    // Accumulate
                    cp1.normalImpulse = x.x;
                    cp2.normalImpulse = x.y;
                    break;
                }
                //
                // Case 3: vn2 = 0 and x1 = 0
                //
                // vn1 = a11 * 0 + a12 * x2 + b1'
                //   0 = a21 * 0 + a22 * x2 + b2'
                //
                x.x = 0.0f;
                x.y = - cp2.normalMass * b.y;
                vn1 = cc.A.m12 * x.y + b.x;
                vn2 = 0.0f;

                if (x.y >= 0.0f && vn1 >= 0.0f)
                {
                    // Resubstitute for the incremental impulse
                    Vector2 d = x - f;

                    // Apply incremental impulse
                    Vector2 P1 = d.x * n;
                    Vector2 P2 = d.y * n;
                    vA -= iMA * (P1 + P2);
                    avA -= iIA * (crossProd2D(cp1.rA, P1) + crossProd2D(cp2.rA, P2));

                    vB += iMB * (P1 + P2);
                    avB += iIB * (crossProd2D(cp1.rB, P1) + crossProd2D(cp2.rB, P2));

                    // Accumulate
                    cp1.normalImpulse = x.x;
                    cp2.normalImpulse = x.y;
                    break;
                }
                //
                // Case 4: x1 = 0 and x2 = 0
                //
                // vn1 = b1
                // vn2 = b2;
                x.x = 0.0f;
                x.y = 0.0f;
                vn1 = b.x;
                vn2 = b.y;

                if (vn1 >= 0.0f && vn2 >= 0.0f )
                {
                    // Resubstitute for the incremental impulse
                    Vector2 d = x - f;

                    // Apply incremental impulse
                    Vector2 P1 = d.x * n;
                    Vector2 P2 = d.y * n;
                    vA -= iMA * (P1 + P2);
                    avA -= iIA * (crossProd2D(cp1.rA, P1) + crossProd2D(cp2.rA, P2));

                    vB += iMB * (P1 + P2);
                    avB += iIB * (crossProd2D(cp1.rB, P1) + crossProd2D(cp2.rB, P2));

                    // Accumulate
                    cp1.normalImpulse = x.x;
                    cp2.normalImpulse = x.y;

                    break;
                }
                cout<<"No Solution For The LCP Problem !"<<endl;
                break;
            }//no loop
        }//case pointContact=2;


        a.vel=vA;
        a.angVel=avA;
        b.vel=vB;
        b.angVel=avB;

#ifdef MY_DEBUG
        if(debugDraw){
            debugDrawer->myNoRefreshPush();
            debugDrawer->myDisplay(shapes);
            for(int j=0;j<pointCount;j++){
                ContactPoint& cp=cc.points[j];
                Vector2 po=cp.position;
                double lengthN=max(0.5,cp.normalImpulse);
                double lengthT=max(0.5,cp.tangentImpulse);
                Vector2 poNormal=po+n*lengthN/5;
                Vector2 poNormal2=po-n*lengthN/5;
                Vector2 poTangent=po+t*lengthT/5;
                Vector2 poTangent2=po-t*lengthT/5;
                debugDrawer->myDrawPoint(po,2,Imagine::GREEN);
                debugDrawer->myDrawLine(poNormal2.x,poNormal2.y,poNormal.x,poNormal.y,Imagine::GREEN,2);
                debugDrawer->myDrawLine(poTangent2.x,poTangent2.y,poTangent.x,poTangent.y,Imagine::RED,2);
            }
            debugDrawer->myNoRefreshPop();
            debugDrawer->myMilliSleep(DEBUG_TIME_INTERVAL);
        }
#endif

    }//loop for allContacts
}

bool ContactSolver::OnePositionSolveIteration(std::vector<ContactConstraint> &allConstraints, std::vector<MyShape *> &shapes)
{
    double minSeparation=1e15;
    for(vector<ContactConstraint>::iterator it=allConstraints.begin();it<allConstraints.end();it++){
        ContactConstraint& cc=*it;

        int indexA=cc.shapeIndexA;
        int indexB=cc.shapeIndexB;
        double iMA=cc.invMassA;
        double iMB=cc.invMassB;
        double iIA=cc.invIA;
        double iIB=cc.invIB;
        int pointCount=cc.pointCount;
        MyShape& a=*shapes[indexA];
        MyShape& b=*shapes[indexB];
        Vector2 cA=a.center;
        Vector2 cB=b.center;
        double avA=a.angle;
        double avB=b.angle;

#ifdef MY_DEBUG
        Vector2 sep[2];
        Vector2 nImpulse[2];


#endif
        for(int j=0;j<pointCount;j++){
            Vector2 n,sepPoint;
            double separation;//negative if overlap
            Transform transA(avA,cA);
            Transform transB(avB,cB);
            switch (cc.type) {

            case ContactConstraint::faceA:
            {

                n=transA.r.Apply(cc.localNormal);
                Vector2 facePoint=transA.Apply(cc.localFacePoint);
                if(b.shapeType==CIRCLE){
                    sepPoint=b.center-n*b.radius;
                }else{
                    sepPoint=transB.Apply(cc.localContact[j]);
                }
                separation=(sepPoint-facePoint)*n;
                //separation < 0 if overlap
            }
                break;
            case ContactConstraint::faceB:
            {

                n=transB.r.Apply(cc.localNormal);
                Vector2 facePoint=transB.Apply(cc.localFacePoint);
                if(a.shapeType==CIRCLE){
                    sepPoint=a.center+n*b.radius;
                }else{
                    sepPoint=transA.Apply(cc.localContact[j]);
                }
                separation=(facePoint-sepPoint)*n;
                separation-=a.radius;

            }
                break;
            case ContactConstraint::circles:
            {

                n=b.center-a.center;
                double distance=sqrt(n*n);
                n.normalize();
                sepPoint=(a.center+b.center)/2;
                separation=distance-a.radius-b.radius;
            }
                break;
            default:
                break;
            }
            if(minSeparation>separation)minSeparation=separation;

            Vector2 rA=sepPoint-cA;
            Vector2 rB=sepPoint-cB;

            double C=max(-0.2,min(0.5*(separation+0.005),0.0));

            double rnA = crossProd2D(rA, n);
            double rnB = crossProd2D(rB, n);
            double K = iMA + iMB + iIA * rnA * rnA + iIB * rnB * rnB;
            double impulse=-C/K;
            Vector2 P=impulse*n;

            cA -= iMA * P;
            avA -= iIA * crossProd2D(rA, P);
            cB += iMB * P;
            avB += iIB * crossProd2D(rB, P);



#ifdef MY_DEBUG
            sep[j]=sepPoint;
            impulse=max(0.001,impulse);
            nImpulse[j]=-impulse*n*100;
#endif
        }
        a.center=cA;
        b.center=cB;
        a.angle=avA;
        b.angle=avB;

#ifdef MY_DEBUG
        if(debugDraw){
            a.updateVertex();
            b.updateVertex();
            debugDrawer->myNoRefreshPush();
            debugDrawer->myDisplay(shapes);
            for(int j=0;j<pointCount;j++){
                ContactPoint& cp=cc.points[j];
                Vector2 po=sep[j];
                Vector2 poA=po+nImpulse[j];
                Vector2 poB=po-nImpulse[j];
                debugDrawer->myDrawPoint(po,2,Imagine::BLUE);
                debugDrawer->myDrawLine(po.x,po.y,poA.x,poA.y,Imagine::BLUE,2);
                debugDrawer->myDrawLine(po.x,po.y,poB.x,poB.y,Imagine::BLUE,2);
            }
            debugDrawer->myNoRefreshPop();
            debugDrawer->myMilliSleep(DEBUG_TIME_INTERVAL);
            //cout<<a.ID<<"-"<<b.ID<<endl;
        }
#endif

    }
    return minSeparation>-0.01;
}



void ContactSolver::SolveVel(vector<ContactConstraint> &allConstraints, std::vector<MyShape *> &shapes)
{
    for(int i=0;i<velIterations;i++){
        OneVelocitySolveIteration(allConstraints,shapes);
    }

#ifdef MY_DEBUG
    if(debugDraw){
        debugDrawer->myMilliSleep(50);
    }
#endif
}

void ContactSolver::SolvePos(std::vector<ContactConstraint> &allConstraints, std::vector<MyShape *> &shapes)
{
    for(int i=0;i<posIterations;i++){
        if(OnePositionSolveIteration(allConstraints,shapes)){
            //the correction is very small, don't need to continue.
            cout<<"Early out from SolvePos"<<endl;
            cout<<"Iteration used: "<<i<<endl;
            break;
        }
    }
}

void ContactSolver::integratePosAndVel(std::vector<MyShape *> &shapes, double dt)
{
    for(std::vector<MyShape*>::iterator i=shapes.begin();i!=shapes.end();i++){
        MyShape* p=*i;
        p->makeMove(dt);
    }
}

void ContactSolver::turnOnDebugDraw()
{
    debugDraw=true;
}

void ContactSolver::FindAndSolve(vector<MyShape*>& shapeList, double dt)
{
    vector<ContactConstraint> c;

    CollisionFinder colliFinder;
    if(debugDraw){
        colliFinder.setDebugDrawer(debugDrawer);
        colliFinder.turnOnDebugDraw();
    }
    c=colliFinder.FindCollisions(shapeList);

    SolveVel(c,shapeList);
    integratePosAndVel(shapeList,dt);
    SolvePos(c,shapeList);
    for(vector<MyShape*>::iterator i=shapeList.begin();i<shapeList.end();i++){
        MyShape* p=*i;
        p->updateVertex();
    }
}

void ContactSolver::setDebugDrawer(GraphicManager *debugDrawer)
{
    this->debugDrawer=debugDrawer;
}



ContactSolver::ContactSolver(int VelIterations, int PosIterations)
{
    velIterations=VelIterations;
    posIterations=PosIterations;
    debugDrawer=0;
    debugDraw=false;
}



