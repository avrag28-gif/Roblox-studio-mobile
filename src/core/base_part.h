#pragma once
#include "instance.h"
#include <array>

namespace rsm {

enum class PartShape { Block, Sphere, Cylinder, Wedge, CornerWedge };

class BasePart : public Instance {
public:
    BasePart();

    PartShape Shape() const noexcept { return shape_; }
    void SetShape(PartShape shape) noexcept { shape_ = shape; }
    std::array<float,3> Size() const noexcept { return size_; }
    void SetSize(float x, float y, float z) noexcept { size_ = {x,y,z}; }
    bool Anchored() const noexcept { return anchored_; }
    void SetAnchored(bool value) noexcept { anchored_ = value; }
    bool CanCollide() const noexcept { return canCollide_; }
    void SetCanCollide(bool value) noexcept { canCollide_ = value; }
    float Mass() const noexcept { return mass_; }
    void SetMass(float value) noexcept { mass_ = value; }
    float Friction() const noexcept { return friction_; }
    void SetFriction(float value) noexcept { friction_ = value; }
    float Elasticity() const noexcept { return elasticity_; }
    void SetElasticity(float value) noexcept { elasticity_ = value; }

private:
    PartShape shape_ = PartShape::Block;
    std::array<float,3> size_{4.0f, 1.0f, 4.0f};
    bool anchored_ = false;
    bool canCollide_ = true;
    float mass_ = 1.0f;
    float friction_ = 0.5f;
    float elasticity_ = 0.3f;
};

}
