#include "contactsolver.h"
#include "graphics.h"
#include <iostream>
//include temps
#include "myworld.h"
//#define MY_DEBUG
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
            //DEBUG
            //myDrawPoint(cp.position);



            //~DEBUG
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
            //DEBUG
            //myDrawPoint(cp1.position);
            //myDrawPoint(cp2.position);



            //~DEBUG
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
        noRefreshPush();
        myDisplay(true);
        for(int j=0;j<pointCount;j++){
            ContactPoint& cp=cc.points[j];
            Vector2 po=cp.position;
            Vector2 poNormal=po+n*cp.normalImpulse/5;
            Vector2 poTangent=po+t*cp.tangentImpulse/5;
            myDrawPoint(po,5,GREEN);
            myDrawLine(po.x,po.y,poNormal.x,poNormal.y,GREEN,2);
            myDrawLine(po.x,po.y,poTangent.x,poTangent.y,RED,2);
        }
        noRefreshPop();
        milliSleep(5);
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
        Vector2 nImpulseA[2];
        Vector2 nImpulseB[2];

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
                sepPoint=transB.Apply(cc.localContact[j]);
                separation=(sepPoint-facePoint)*n;
            }
                break;
            case ContactConstraint::faceB:
            {

                n=transB.r.Apply(cc.localNormal);
                Vector2 facePoint=transB.Apply(cc.localFacePoint);
                sepPoint=transA.Apply(cc.localContact[j]);
                separation=(facePoint-sepPoint)*n;

            }
                break;
            case ContactConstraint::circles:
            {
                //
                //
                //Unfinished
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
            nImpulseA[j]=-iMA*P*50;
            nImpulseB[j]=iMB*P*50;

#endif
        }
        a.center=cA;
        b.center=cB;
        a.angle=avA;
        b.angle=avB;
#ifdef MY_DEBUG
        a.updateVertex();
        b.updateVertex();
        noRefreshPush();
        myDisplay(true);
        for(int j=0;j<pointCount;j++){
            ContactPoint& cp=cc.points[j];
            Vector2 po=sep[j];
            Vector2 poA=po+nImpulseA[j];
            Vector2 poB=po+nImpulseB[j];
            myDrawPoint(po,2,RED);
            myDrawLine(po.x,po.y,poA.x,poA.y,RED,2);
            myDrawLine(po.x,po.y,poB.x,poB.y,RED,2);
        }
        noRefreshPop();
        milliSleep(5);
        //cout<<a.ID<<"-"<<b.ID<<endl;
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
    //milliSleep(500);
    //~DEBUG
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

ContactSolver::ContactSolver(int VelIterations, int PosIterations)
{
    velIterations=VelIterations;
    posIterations=PosIterations;
}



ContactSolver::~ContactSolver()
{

}

