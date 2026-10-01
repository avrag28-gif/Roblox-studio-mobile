#pragma once
#include "../core/base_part.h"
#include "../core/world_bounds.h"
#include "../core/transform_hierarchy.h"
#include "../math/aabb.h"
#include <cmath>
#include <unordered_map>
#include <vector>
#include <algorithm>

namespace rsm {
struct RaycastHit{BasePart*part=nullptr;float distance=0;Vector3 position{};};

class PhysicsWorld{
    Vector3 gravity_;
    std::unordered_map<const BasePart*,Vector3> velocity_;
    std::unordered_map<const BasePart*,float> sleep_;
    float restitution_=.05f,friction_=.8f,sleepThreshold_=.05f;

    static AABB Bounds(const BasePart* p) { return WorldBounds(*p); }

    static Vector3 InverseRotate(const Quaternion& q, Vector3 v) {
        const Quaternion n=q.Normalized();
        return Quaternion{-n.x,-n.y,-n.z,n.w}.Rotate(v);
    }

    static bool RaycastPart(const BasePart& p,const Ray& worldRay,float& t) {
        const CFrame cf=WorldCFrame(p);
        const Vector3 localOrigin=InverseRotate(cf.rotation,worldRay.origin-cf.position);
        const Vector3 localDirection=InverseRotate(cf.rotation,worldRay.direction);
        const Vector3 h=p.Size()*.5f;
        const AABB local{{-h.x,-h.y,-h.z},{h.x,h.y,h.z}};
        Ray localRay{localOrigin,localDirection};
        return local.IntersectRay(localRay,t);
    }

public:
    explicit PhysicsWorld(Vector3 g={0,-196.2f,0}):gravity_(g){}
    void SetRestitution(float v){restitution_=std::clamp(v,0.f,1.f);}
    void SetFriction(float v){friction_=std::clamp(v,0.f,1.f);}

    void Step(float dt,const Instance& root) {
        if(dt<=0)return;
        std::vector<BasePart*> bodies;
        for(auto* x:root.GetDescendants()) if(auto* p=dynamic_cast<BasePart*>(x)) {
            bodies.push_back(p);
            if(p->Anchored()){velocity_.erase(p);sleep_.erase(p);}
            else {
                velocity_[p]+=gravity_*dt;
                if(sleep_.count(p)){sleep_[p]+=dt;if(sleep_[p]>=.5f)continue;sleep_.erase(p);}
            }
        }

        for(auto* p:bodies) if(!p->Anchored()) {
            auto& v=velocity_[p];
            Vector3 pos=WorldPosition(*p)+v*dt;
            const AABB bounds=WorldBounds(*p);
            const float halfHeight=std::max(.0001f,bounds.max.y-bounds.min.y)*.5f;
            if(pos.y-halfHeight<0) {
                pos.y=halfHeight;
                if(v.y<0)v.y=-v.y*restitution_;
                v.x*=friction_;v.z*=friction_;
            }
            if(v.Length()<sleepThreshold_ && std::abs(pos.y-halfHeight)<.001f){v={0,0,0};sleep_[p]=0;}
            SetWorldCFrame(*p,CFrame(pos,WorldRotation(*p)));
        }

        for(size_t i=0;i<bodies.size();++i) for(size_t j=i+1;j<bodies.size();++j) {
            auto*a=bodies[i];auto*b=bodies[j];
            if(!a->CanCollide()||!b->CanCollide())continue;
            if(!Bounds(a).Intersects(Bounds(b)))continue;
            if(a->Anchored()&&!b->Anchored()) {
                auto&v=velocity_[b];v.y=std::max(0.f,v.y);
                auto bp=WorldPosition(*b);
                bp.y=Bounds(a).max.y+(Bounds(b).max.y-Bounds(b).min.y)*.5f;
                SetWorldCFrame(*b,CFrame(bp,WorldRotation(*b)));
            } else if(b->Anchored()&&!a->Anchored()) {
                auto&v=velocity_[a];v.y=std::max(0.f,v.y);
                auto ap=WorldPosition(*a);
                ap.y=Bounds(b).max.y+(Bounds(a).max.y-Bounds(a).min.y)*.5f;
                SetWorldCFrame(*a,CFrame(ap,WorldRotation(*a)));
            }
        }
    }

    RaycastHit Raycast(const Instance& root,const Ray& r) const {
        RaycastHit best;best.distance=1e30f;
        for(auto* x:root.GetDescendants()) if(auto* p=dynamic_cast<BasePart*>(x)) if(p->CanQuery()) {
            float t=0;
            if(RaycastPart(*p,r,t)&&t>=0&&t<best.distance) best={p,t,r.At(t)};
        }
        if(!best.part)best.distance=0;
        return best;
    }

    bool Overlap(const Instance&root,const AABB&q)const{
        for(auto*x:root.GetDescendants())if(auto*p=dynamic_cast<BasePart*>(x))
            if(p->CanQuery()&&Bounds(p).Intersects(q))return true;
        return false;
    }

    void SetGravity(Vector3 g){gravity_=g;}
    Vector3 Gravity()const{return gravity_;}
    void Clear(){velocity_.clear();sleep_.clear();}
    bool IsSleeping(const BasePart*p)const{return sleep_.count(p)!=0;}
};
}
