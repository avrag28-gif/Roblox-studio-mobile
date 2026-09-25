#include "../src/core/base_part.h"
#include <cassert>
#include <iostream>
int main(){rsm::Part p;p.SetName("Part");p.SetPosition({1,2,3});p.SetSize({4,5,6});auto c=p.Clone();assert(c&&c->ClassName()=="Part");auto* bp=dynamic_cast<rsm::BasePart*>(c.get());assert(bp&&bp->Position().x==1);std::cout<<"TYPED_OBJECT_PASS\n";}