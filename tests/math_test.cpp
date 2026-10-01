#include "../src/math/math3d.h"
#include "../src/math/cframe.h"
#include <cassert>
#include <cmath>
using namespace rsm;
static bool near(float a,float b){return std::fabs(a-b)<1e-4f;}
int main(){
 Vector3 a{1,2,3},b{2,1,0};auto c=a+b;assert(c.x==3&&c.y==3&&c.z==3);assert(Vector3::dot(a,b)==4);auto n=a.normalized();assert(n.length()>0.99f&&n.length()<1.01f);
 auto parent=CFrame({10,0,0},Quaternion::FromAxisAngle({0,1,0},3.14159265358979323846f/2));
 auto child=CFrame({2,0,0}); auto world=parent*child; assert(near(world.position.x,10)); assert(near(world.position.z,-2));
 auto local=parent.Inverse()*world; assert(near(local.position.x,2)); assert(near(local.position.y,0)); assert(near(local.position.z,0));
 auto pworld=parent.PointToWorldSpace({0,0,-2}); assert(near(pworld.x,8)); assert(near(pworld.z,0));
 auto back=parent.PointToObjectSpace(pworld); assert(near(back.x,0)); assert(near(back.z,-2));
 return 0;
}
