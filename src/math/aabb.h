#pragma once
#include "vector3.h"
#include "ray.h"
#include <algorithm>
#include <cmath>
namespace rsm {struct AABB{Vector3 min,max;bool Contains(Vector3 p)const{return p.x>=min.x&&p.y>=min.y&&p.z>=min.z&&p.x<=max.x&&p.y<=max.y&&p.z<=max.z;}bool Intersects(const AABB&o)const{return min.x<=o.max.x&&max.x>=o.min.x&&min.y<=o.max.y&&max.y>=o.min.y&&min.z<=o.max.z&&max.z>=o.min.z;}bool IntersectRay(const Ray&r,float&t)const{float lo=0,hi=1e30f;for(int a=0;a<3;++a){float o=a==0?r.origin.x:a==1?r.origin.y:r.origin.z,d=a==0?r.direction.x:a==1?r.direction.y:r.direction.z,mn=a==0?min.x:a==1?min.y:min.z,mx=a==0?max.x:a==1?max.y:max.z;if(std::abs(d)<1e-8f){if(o<mn||o>mx)return false;continue;}float t1=(mn-o)/d,t2=(mx-o)/d;if(t1>t2)std::swap(t1,t2);lo=std::max(lo,t1);hi=std::min(hi,t2);if(lo>hi)return false;}t=lo;return true;}};}