#pragma once
#include "renderer.h"
#include <cmath>
#ifdef __ANDROID__
#include <GLES3/gl3.h>
#endif
namespace rsm {
class GLESRenderer final : public Renderer {
public:
 bool Initialize() override {
#ifdef __ANDROID__
  const char* v="#version 300 es\nlayout(location=0) in vec3 aPos;\nuniform mat4 uMVP;\nvoid main(){gl_Position=uMVP*vec4(aPos,1.0);}";
  const char* f="#version 300 es\nprecision mediump float;\nout vec4 o;\nuniform vec4 uColor;\nvoid main(){o=uColor;}";
  vs_=compile(GL_VERTEX_SHADER,v); fs_=compile(GL_FRAGMENT_SHADER,f); if(!vs_||!fs_) return false;
  program_=glCreateProgram(); glAttachShader(program_,vs_); glAttachShader(program_,fs_); glLinkProgram(program_);
  GLint ok=0; glGetProgramiv(program_,GL_LINK_STATUS,&ok); if(!ok) return false;
  glGenVertexArrays(1,&vao_); glGenBuffers(1,&vbo_); return true;
#else
  return false;
#endif
 }
 void Resize(int w,int h) override {
#ifdef __ANDROID__
  w_=w;h_=h;glViewport(0,0,w,h);
#else
  (void)w;(void)h;
#endif
 }
 void Render(const Instance& root) override {
#ifdef __ANDROID__
  glEnable(GL_DEPTH_TEST); glClearColor(.055f,.063f,.08f,1); glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glUseProgram(program_); visible_=0;
  for(auto* i:root.GetDescendants()) if(auto* p=dynamic_cast<const BasePart*>(i)) drawPart(*p);
#else
  (void)root;
#endif
 }
 const char* Backend() const override { return "OpenGL ES 3"; }\n void SetCamera(const Camera& c) override { camera_=c; }
 std::size_t VisibleCount() const { return visible_; }
private:
#ifdef __ANDROID__
 GLuint vs_=0,fs_=0,program_=0,vao_=0,vbo_=0;
 int w_=0,h_=0; std::size_t visible_=0; Camera camera_{};
 static GLuint compile(GLenum type,const char* src){GLuint s=glCreateShader(type);glShaderSource(s,1,&src,nullptr);glCompileShader(s);GLint ok=0;glGetShaderiv(s,GL_COMPILE_STATUS,&ok);if(!ok){glDeleteShader(s);return 0;}return s;}
 static void mul(const float*a,const float*b,float*out){for(int c=0;c<4;c++)for(int r=0;r<4;r++){out[c*4+r]=0;for(int k=0;k<4;k++)out[c*4+r]+=a[k*4+r]*b[c*4+k];}}\n void buildMvp(Vector3 pos,float*out) const { float dx=camera_.target.x-camera_.position.x,dy=camera_.target.y-camera_.position.y,dz=camera_.target.z-camera_.position.z; float fl=std::sqrt(dx*dx+dy*dy+dz*dz); if(fl<1e-5f)fl=1;dx/=fl;dy/=fl;dz/=fl; float rx=dz,ry=0,rz=-dx; float rl=std::sqrt(rx*rx+rz*rz);if(rl<1e-5f)rl=1;rx/=rl;rz/=rl;float ux=ry*dz-rz*dy,uy=rz*dx-rx*dz,uz=rx*dy-ry*dx;float view[16]={rx,ux,-dx,0,ry,uy,-dy,0,rz,uz,-dz,0,-(rx*camera_.position.x+ry*camera_.position.y+rz*camera_.position.z),-(ux*camera_.position.x+uy*camera_.position.y+uz*camera_.position.z),dx*camera_.position.x+dy*camera_.position.y+dz*camera_.position.z,1}; float aspect=h_?float(w_)/h_:1;float f=1/std::tan(camera_.fov*3.14159265f/360.f),n=camera_.nearPlane,farv=camera_.farPlane;float proj[16]={f/aspect,0,0,0,0,f,0,0,0,0,(farv+n)/(n-farv),-1,0,0,(2*farv*n)/(n-farv),0};float vp[16];mul(proj,view,vp);float tr[16]={1,0,0,0,0,1,0,0,0,0,1,0,-pos.x,-pos.y,-pos.z,1};mul(vp,tr,out);}\n void drawPart(const BasePart& p){
  float x=p.Position().x,y=p.Position().y,z=p.Position().z,sx=p.Size().x*.5f,sy=p.Size().y*.5f,sz=p.Size().z*.5f;
  const float v[]={x-sx,y-sy,z-sz,x+sx,y-sy,z-sz,x+sx,y+sy,z-sz,x-sx,y+sy,z-sz,x-sx,y-sy,z+sz,x+sx,y-sy,z+sz,x+sx,y+sy,z+sz,x-sx,y+sy,z+sz};
  const unsigned short idx[]={0,1,2,2,3,0,4,6,5,6,4,7,0,4,5,5,1,0,3,2,6,6,7,3,1,5,6,6,2,1,4,0,3,3,7,4};
  glBindVertexArray(vao_);glBindBuffer(GL_ARRAY_BUFFER,vbo_);glBufferData(GL_ARRAY_BUFFER,sizeof(v),v,GL_STREAM_DRAW);
  glEnableVertexAttribArray(0);glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
  GLint loc=glGetUniformLocation(program_,"uColor");glUniform4f(loc,p.Color().r,p.Color().g,p.Color().b,1.f-p.Transparency());
  // MVP is intentionally supplied as identity for the first native surface pass; editor camera owns projection.
  float m[16]; buildMvp(p.Position(),m); glUniformMatrix4fv(glGetUniformLocation(program_,"uMVP"),1,GL_FALSE,m);
  glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_SHORT,idx); ++visible_;
 }
#endif
};
}