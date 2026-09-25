#pragma once
namespace rsm{struct Matrix4{float m[16]{};static Matrix4 Identity(){Matrix4 r;for(int i=0;i<4;i++)r.m[i*4+i]=1;return r;}};}