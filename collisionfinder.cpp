#include "collisionfinder.h"
using namespace std;


bool CollisionFinder::shouldContact(const MyShape &a, const MyShape &b)
{
    if(a.fixToGround&&b.fixToGround){
        return false;
    }else{
        return true;
    }
}

bool CollisionFinder::SATtwoShape(MyShape *a,MyShape *b, Vector2 &n, Vector2 &contactPoint1, Vector2 &contactPoint2, int &numOfContact,ContactConstraint::Type& contactType,Vector2& facePoint)
{
    if(a->shapeType==POLYGON&&b->shapeType==POLYGON){
        if(SATPolygon(static_cast<MyPolygon*>(a),static_cast<MyPolygon*>(b),n,contactPoint1,contactPoint2,numOfContact,contactType,facePoint)){
            return true;
        }else{
            return false;
        }
    }else if(a->shapeType==POLYGON&&b->shapeType==CIRCLE){
        if(SATPolygonCircle(static_cast<MyPolygon*>(a),static_cast<MyCircle*>(b),n,contactPoint1,facePoint)){
            contactType=ContactConstraint::faceA;
            numOfContact=1;
            return true;
        }else{
            return false;
        }
    }else if(a->shapeType==CIRCLE&&b->shapeType==POLYGON){
        if(SATPolygonCircle(static_cast<MyPolygon*>(b),static_cast<MyCircle*>(a),n,contactPoint1,facePoint)){
            contactType=ContactConstraint::faceB;
            numOfContact=1;
            n=-n;
            return true;
        }else{
            return false;
        }
    }else if(a->shapeType==CIRCLE&&b->shapeType==CIRCLE){
        if(SATCircle(static_cast<MyCircle*>(a),static_cast<MyCircle*>(b),n,contactPoint1)){
            contactType=ContactConstraint::circles;
            numOfContact=1;
            //facePoint not used here
            return true;
        }else{
            return false;
        }

    }else{
        cout<<"unknown shapetype"<<endl;
        exit(0);
    }
}

bool CollisionFinder::GetConstraint(int indexA, int indexB, MyShape *pa, MyShape *pb, ContactConstraint &constraint)
{
    Vector2 n,p[2];
    Vector2 facePoint;
    int numOfContact;
    ContactConstraint::Type contactType;
    if(SATtwoShape(pa,pb,n,p[0],p[1],numOfContact,contactType,facePoint)){
        //n : A->B
        const MyShape& a=*pa;
        const MyShape& b=*pb;
        double iMA=a.invMass;
        double iMB=b.invMass;
        double iIA=a.invI;
        double iIB=b.invI;
        Vector2 cA=a.center;
        Vector2 cB=b.center;
        Vector2 vA=a.vel;
        Vector2 vB=b.vel;
        double avA=a.angVel;
        double avB=b.angVel;


        //prepare for position correction
        Transform transA(a.angle,a.center);
        Transform transB(b.angle,b.center);
        constraint.type=contactType;
        switch (contactType) {
        case ContactConstraint::faceA :

            constraint.localNormal=transA.r.UnApply(n);
            constraint.localFacePoint=transA.UnApply(facePoint);
            constraint.localContact[0]=transB.UnApply(p[0]);
            constraint.localContact[1]=transB.UnApply(p[1]);
            break;
        case ContactConstraint::faceB :
            constraint.localNormal=transB.r.UnApply(n);
            constraint.localFacePoint=transB.UnApply(facePoint);
            constraint.localContact[0]=transA.UnApply(p[0]);
            constraint.localContact[1]=transA.UnApply(p[1]);
            break;
        case ContactConstraint::circles :
            //nothing to do here.
        default:
            break;
        }


        constraint.shapeIndexA=indexA;
        constraint.shapeIndexB=indexB;
        constraint.friction=sqrt(a.friction*b.friction);
        constraint.restitution=max(a.restitution,b.restitution);

        constraint.invMassA=iMA;
        constraint.invMassB=iMB;
        constraint.invIA=iIA;
        constraint.invIB=iIB;

        constraint.normal=n;//A->B
        Vector2 t=crossProd2D(n,1.0);

        constraint.pointCount=numOfContact;

        for(int j=0;j<numOfContact;j++){
            ContactPoint& cp=constraint.points[j];

            cp.position=p[j];
            cp.rA=p[j]-cA;
            cp.rB=p[j]-cB;

            double rnA=crossProd2D(cp.rA,n);
            double rnB=crossProd2D(cp.rB,n);
            double kNormal = iMA+iMB+iIA*rnA*rnA+iIB*rnB*rnB;
            cp.normalMass=1/kNormal;

            double rtA=crossProd2D(cp.rA,t);
            double rtB=crossProd2D(cp.rB,t);
            double kTangent = iMA+iMB+iIA*rtA*rtA+iIB*rtB*rtB;
            cp.tangentMass=1/kTangent;

            cp.tangentImpulse=0.0;
            cp.normalImpulse=0.0;

            double velRelative=n*(vB+crossProd2D(avB,cp.rB)-vA-crossProd2D(avA,cp.rA));
            if(1){//velRelative<-0.1
                cp.separatingVel=-constraint.restitution*velRelative;
            }else{
                cp.separatingVel=0.0;
            }
        }
        //prepare the matrix A for the small LCP problem
        if(numOfContact==2){
            ContactPoint& cp1=constraint.points[0];
            ContactPoint& cp2=constraint.points[1];
            double rn1A=crossProd2D(cp1.rA,constraint.normal);
            double rn1B=crossProd2D(cp1.rB,constraint.normal);
            double rn2A=crossProd2D(cp2.rA,constraint.normal);
            double rn2B=crossProd2D(cp2.rB,constraint.normal);

            double a11 = iMA + iMB + iIA * rn1A * rn1A + iIB * rn1B * rn1B;
            double a22 = iMA + iMB + iIA * rn2A * rn2A + iIB * rn2B * rn2B;
            double a12 = iMA + iMB + iIA * rn1A * rn2A + iIB * rn1B * rn2B;
            const double aMaxNumber=10000.0;
            if(a11*a11<aMaxNumber*(a11*a22-a12*a12)){
                //A must be a PSD, invA exist
                constraint.A=Matrix2(a11,a12,a12,a22);
                constraint.invA=constraint.A.GetInverse();
            }else{
                cout<<"Matrix singular:"<<endl;
                cout<<a11*a11/(a11*a22-a12*a12)<<endl;
                //abandon the 2nd point, avoid the singularity.
                constraint.pointCount=1;
            }
        }
        return true;
    }else{
        return false;
    }
}

vector<ContactConstraint> CollisionFinder::FindCollisions(vector<MyShape*> &shapes)
{
    int i=-1,j;
    std::vector<ContactConstraint> constraints;
    for(vector<MyShape *>::const_iterator it_i=shapes.begin();it_i<shapes.end()-1;it_i++){
        i++;
        j=i;
        for(vector<MyShape *>::const_iterator it_j=it_i+1;it_j<shapes.end();it_j++){
            j++;
            MyShape* a=*it_i;
            MyShape* b=*it_j;
            if(!shouldContact(*a,*b))continue;
            if(TestAABBAABB(a->aabb,b->aabb))
            {
                ContactConstraint cc;
                if(GetConstraint(i,j,a,b,cc)){
                    constraints.push_back(cc);
                }
            }

        }

    }
    return constraints;
}

void CollisionFinder::setDebugDrawer(GraphicManager *debugDrawer)
{
    this->debugDrawer=debugDrawer;
}

CollisionFinder::CollisionFinder()
{
    this->debugDrawer=0;
}

void CollisionFinder::turnOnDebugDraw()
{
    debugDraw=true;
}

void CollisionFinder::clipSegment(Vector2& l1,Vector2& l2,Vector2 clipPoint,Vector2 n){
    double a,b,c;
    Vector2 t=l2-l1;
    a=n*clipPoint;b=l1*n;c=l2*n;
    if(a>b){
        l1+=(a-b)/(c-b)*t;
    }
    if(a>c){
        l2-=(a-c)/(b-c)*t;
    }
}

void CollisionFinder::getMinFromProjection(const vector<Vector2> &v, Vector2 n, double &min, vector<Vector2>::const_iterator &min_i)
{
    min=1e15;
    double proj;
    for(vector<Vector2>::const_iterator i=v.begin();i<v.end();i++){
        proj=n*(*i);
        if(proj<min){
            min=proj;
            min_i=i;
        }
    }
}
bool CollisionFinder::SATPolygon(MyPolygon *pa, MyPolygon *pb, Vector2& n, Vector2& contactPoint1, Vector2& contactPoint2, int& numOfContact, ContactConstraint::Type& contactType, Vector2& facePoint){
    MyPolygon &a=*pa;
    MyPolygon &b=*pb;
    double mina,minb,maxa,maxb,d1,d2,temp;
    double depth=1e15;
    bool referenceIsA=false;
    Vector2 refP1,refP2;
    Vector2 incP1,incP2;
    Vector2 normal;
    //reference A
    vector<Vector2>::const_iterator min_i,p,p_prev,p_next;
    vector<Vector2>::const_iterator i,iN,j;

    a.updateNormal();
    for(i=a.vertex.begin(),iN=a.normal.begin();i<a.vertex.end()-1;i++,iN++){
        normal=*iN;
        getMinFromProjection(b.vertex,normal,minb,min_i);
        maxa=normal*(*i);
        temp=maxa-minb;
        if(temp<0)return false;
        else if(temp<depth){
            n=normal;
            depth=temp;
            referenceIsA=true;
            refP1=*i;refP2=*(i+1);
            p=min_i;
        }

    }
    //last edge of shapeA: i=vertex.end()-1----->vertex.begin()
    //in fact here we have
    //i=a.vertex.end()-1
    //iN=a.normal.end()-1
    j=a.vertex.begin();
    normal=*iN;
    getMinFromProjection(b.vertex,normal,minb,min_i);
    maxa=normal*(*i);
    temp=maxa-minb;
    if(temp<0)return false;
    else if(temp<depth){
        n=normal;
        depth=temp;
        referenceIsA=true;
        refP1=*i;refP2=*j;
        p=min_i;
    }

    //------------reference B---------------------------
    b.updateNormal();
    for(i=b.vertex.begin(),iN=b.normal.begin();i<b.vertex.end()-1;i++,iN++){
        normal=*iN;
        getMinFromProjection(a.vertex,normal,mina,min_i);
        maxb=normal*(*i);
        temp=maxb-mina;
        if(temp<0)return false;
        else if(temp<depth){
            n=normal;
            depth=temp;
            referenceIsA=false;
            refP1=*i;refP2=*(i+1);
            p=min_i;
        }

    }
    //last edge of shapeB: i=vertex.end()-1----->vertex.begin()
    //in fact here we have
    //i=b.vertex.end()-1
    //iN=b.normal.end()-1
    j=b.vertex.begin();
    normal=*iN;
    getMinFromProjection(a.vertex,normal,mina,min_i);
    maxb=normal*(*i);
    temp=maxb-mina;
    if(temp<0)return false;
    else if(temp<depth){
        n=normal;
        depth=temp;
        referenceIsA=false;
        refP1=*i;refP2=*j;
        p=min_i;
    }

    //p_prev,p_next
    if(referenceIsA){
        if(p==b.vertex.end()-1){
            p_next=b.vertex.begin();
            p_prev=p-1;
        }
        else if(p==b.vertex.begin()){
            p_prev=b.vertex.end()-1;
            p_next=p+1;
        }else{
            p_prev=p-1;
            p_next=p+1;
        }
    }else{
        if(p==a.vertex.end()-1){
            p_next=a.vertex.begin();
            p_prev=p-1;
        }
        else if(p==a.vertex.begin()){
            p_prev=a.vertex.end()-1;
            p_next=p+1;
        }
        else{
            p_prev=p-1;
            p_next=p+1;
        }
    }
    //
    Vector2 p1=*p-*p_next;
    Vector2 p2=*p-*p_prev;
    double temp1=(p1*n)*(p1*n)/(p1*p1);
    double temp2=(p2*n)*(p2*n)/(p2*p2);
    if(temp1>temp2){
        incP1=*p;
        incP2=*p_prev;
    }else{
        incP1=*p;
        incP2=*p_next;
    }
    //incP1 incP2 refP1 refP2 n
    clipSegment(incP1,incP2,refP1,refP2-refP1);
    clipSegment(incP1,incP2,refP2,refP1-refP2);
    n=n/sqrt(n*n);
    contactType=referenceIsA?ContactConstraint::faceA : ContactConstraint::faceB;
    facePoint=refP1;
    if(incP1*n>refP1*n){
        contactPoint1=incP2;
        numOfContact=1;
        //n: A->B
        n=referenceIsA?n:-n;
        return true;
    }else if(incP2*n>refP1*n){
        contactPoint1=incP1;
        numOfContact=1;
        n=referenceIsA?n:-n;
        return true;
    }else{
        contactPoint1=incP1;
        contactPoint2=incP2;
        numOfContact=2;
        n=referenceIsA?n:-n;
        return true;
    }


}

bool CollisionFinder::SATCircle(MyCircle *pa, MyCircle *pb, Vector2 &n, Vector2 &contactPoint)
{
    MyCircle &cirA=*pa;
    MyCircle &cirB=*pb;
    Vector2 centerAB=cirB.center-cirA.center;
    double radius2=cirA.radius+cirB.radius;
    if(centerAB*centerAB>radius2*radius2){
        return false;
    }
    double distance=sqrt(centerAB*centerAB);
    n=centerAB/distance;
    contactPoint=cirA.center+(cirA.radius-cirB.radius+distance)/2*n;
}

bool CollisionFinder::SATPolygonCircle(MyPolygon *pa, MyCircle *pb, Vector2 &n, Vector2 &contactPoint, Vector2 &facePoint)
{
    MyPolygon &poly=*pa;
    MyCircle &cir=*pb;
    double temp;
    double depth=1e15;
    bool referenceIsA=false;
    Vector2 refP1,refP2;
    Vector2 normal;
    double radius=cir.radius;
    Vector2 cCenter=cir.center;

    //reference Polygon
    vector<Vector2>::const_iterator i,iN,j;

    poly.updateNormal();
    for(i=poly.vertex.begin(),iN=poly.normal.begin();i<poly.vertex.end()-1;i++,iN++){
        normal=*iN;
        temp=radius-normal*(cCenter-*i);
        if(temp<0)return false;
        else if(temp<depth){
            n=normal;
            depth=temp;
            referenceIsA=true;
            refP1=*i;refP2=*(i+1);
        }
    }
    //last edge of Poly: i=vertex.end()-1----->vertex.begin()
    //in fact here we have
    //i=poly.vertex.end()-1
    //iN=poly.normal.end()-1
    j=poly.vertex.begin();
    normal=*iN;
    temp=radius-normal*(cCenter-*i);
    if(temp<0)return false;
    else if(temp<depth){
        n=normal;
        depth=temp;
        referenceIsA=true;
        refP1=*i;refP2=*j;
    }

    //here we find one collision and the normal of edge

    //if circle center is in the Poly
    if(depth>radius){
        facePoint=refP1;
        contactPoint=cCenter;
        return true;
    }
    Vector2 P1_center=cCenter-refP1;
    Vector2 P2_center=cCenter-refP2;
    double limit1=P1_center*(refP2-refP1);
    double limit2=P2_center*(refP1-refP2);

    if(limit1<=0){
        //Circle - Vertex contact
        if(P1_center*P1_center>radius*radius){
            return false;
        }
        n=P1_center;
        n.normalize();
        facePoint=refP1;
        contactPoint=refP1;
        return true;
    }else if(limit2<=0){
        //Circle - Vertex contact
        if(P2_center*P2_center>radius*radius){
            return false;
        }
        n=P2_center;
        n.normalize();
        facePoint=refP2;
        contactPoint=refP2;
        return true;
    }else{
        //Circle - Edge contact
        facePoint=refP1;//or refP2, the same after facepoint*normal
        contactPoint=cCenter-radius*n;
    }


}
