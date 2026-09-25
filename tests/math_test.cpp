#include "../src/math/math3d.h"
#include <cassert>
using namespace rsm;
int main(){Vector3 a{1,2,3},b{2,1,0};auto c=a+b;assert(c.x==3&&c.y==3&&c.z==3);assert(Vector3::dot(a,b)==4);auto n=a.normalized();assert(n.length()>0.99f&&n.length()<1.01f);return 0;}
