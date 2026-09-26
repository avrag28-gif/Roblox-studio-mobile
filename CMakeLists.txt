cmake_minimum_required(VERSION 3.22)
project(RobloxStudioMobile LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(RSM_ENABLE_LUAU "Build against official Luau source" OFF)
option(RSM_ENABLE_IOS "Enable shared iOS bridge build" OFF)
add_library(rsm_core STATIC src/core/instance.cpp src/core/data_model.cpp src/core/base_part.cpp)
add_executable(rsm_editor src/main.cpp)
target_link_libraries(rsm_editor PRIVATE rsm_core)
target_include_directories(rsm_core PUBLIC src)
set(RSM_TESTS
  rsm_smoke
  rsm_core_instance_test
  rsm_engine_smoke_test
  rsm_full_vertical_slice_test
  rsm_math_test
  rsm_system_integration_test
  rsm_phase_systems_test
  rsm_complete_engine_test
  rsm_mobile_systems_test
  rsm_runtime_asset_security_test
  rsm_script_manager_test
  rsm_release_systems_test
  rsm_asset_cache_gltf_test
  rsm_property_access_test
  rsm_studio_runtime_test)
add_executable(rsm_smoke tests/smoke_test.cpp)
add_executable(rsm_core_instance_test tests/core_instance_test.cpp)
add_executable(rsm_engine_smoke_test tests/engine_smoke_test.cpp)
add_executable(rsm_full_vertical_slice_test tests/full_vertical_slice_test.cpp)
add_executable(rsm_math_test tests/math_test.cpp)
add_executable(rsm_system_integration_test tests/system_integration_test.cpp)
add_executable(rsm_phase_systems_test tests/phase_systems_test.cpp)
add_executable(rsm_complete_engine_test tests/complete_engine_test.cpp)
add_executable(rsm_mobile_systems_test tests/mobile_systems_test.cpp)
add_executable(rsm_runtime_asset_security_test tests/runtime_asset_security_test.cpp)
add_executable(rsm_script_manager_test tests/script_manager_test.cpp)
add_executable(rsm_release_systems_test tests/release_systems_test.cpp)
add_executable(rsm_asset_cache_gltf_test tests/asset_cache_gltf_test.cpp)
add_executable(rsm_property_access_test tests/property_access_test.cpp)
add_executable(rsm_studio_runtime_test tests/studio_runtime_test.cpp)
foreach(t IN LISTS RSM_TESTS)
 target_link_libraries(${t} PRIVATE rsm_core)
endforeach()
enable_testing()
add_test(NAME core COMMAND rsm_smoke)
add_test(NAME instance COMMAND rsm_core_instance_test)
add_test(NAME engine COMMAND rsm_engine_smoke_test)
add_test(NAME vertical_slice COMMAND rsm_full_vertical_slice_test)
add_test(NAME math COMMAND rsm_math_test)
add_test(NAME system_integration COMMAND rsm_system_integration_test)
add_test(NAME phase_systems COMMAND rsm_phase_systems_test)
add_test(NAME complete_engine COMMAND rsm_complete_engine_test)
add_test(NAME mobile_systems COMMAND rsm_mobile_systems_test)
add_test(NAME runtime_asset_security COMMAND rsm_runtime_asset_security_test)
add_test(NAME script_manager COMMAND rsm_script_manager_test)
add_test(NAME release_systems COMMAND rsm_release_systems_test)
add_test(NAME asset_cache_gltf COMMAND rsm_asset_cache_gltf_test)
add_test(NAME property_access COMMAND rsm_property_access_test)
add_test(NAME studio_runtime COMMAND rsm_studio_runtime_test)
if(RSM_ENABLE_LUAU)
 include(FetchContent)
 FetchContent_Declare(luau GIT_REPOSITORY https://github.com/luau-lang/luau.git GIT_TAG master)
 set(LUAU_BUILD_CLI OFF CACHE BOOL "" FORCE)
 set(LUAU_BUILD_TESTS OFF CACHE BOOL "" FORCE)
 FetchContent_MakeAvailable(luau)
 target_link_libraries(rsm_core PUBLIC Luau.VM Luau.Compiler)
 target_compile_definitions(rsm_core PUBLIC RSM_LUAU_ENABLED=1)
endif()
