#include "../src/assets/asset_cache.h"
#include "../src/project/crash_recovery.h"
#include "../src/project/migration.h"
#include "../src/runtime/client_server_runtime.h"
#include "../src/core/data_model.h"
#include <cassert>
int main(){
 rsm::AssetCache c(100); assert(c.Put("a",60)); assert(c.Put("b",40)); assert(c.Used()==100); assert(c.Put("c",50)); assert(c.Used()<=100);
 std::string old="RSM2\nscene";std::string now;assert(rsm::ProjectMigration::Migrate(old,now));assert(now.rfind("RSM3\n",0)==0);
 rsm::DataModel game;assert(rsm::ClientServerRuntime().Running()==false); rsm::ClientServerRuntime rt;assert(rt.Start(game));assert(rt.Running());assert(rt.Server()&&rt.Client());rt.Stop();assert(!rt.Running());return 0;
}