#pragma once
#include "../math/vector3.h"
namespace rsm{struct CameraState{Vector3 position{0,5,10};Vector3 target{};float fov=60,nearPlane=.05f,farPlane=10000;};}