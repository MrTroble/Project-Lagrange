cmake_minimum_required (VERSION 3.15)

project ("Lagrange-Viewer" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
add_compile_definitions(DEBUG=1)
endif()

if(false)
add_compile_options("/fsanitize=address")
add_link_options("/fsanitize=address")
if(true)
add_compile_definitions(_DISABLE_VECTOR_ANNOTATION=1)
add_compile_definitions(_DISABLE_STRING_ANNOTATION=1)
endif()
endif()

if(WIN32)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_link_options("/ignore:4099")
else()
add_compile_options("-fPIC")
endif()

add_subdirectory(TGEngine)

add_executable(TGApp "application/TGApp.cpp" "application/util/CellEntry.cpp" "application/util/VulkanHelper.cpp"  "application/TGAppDef.cpp" "application/module/TGAppIO.cpp")
target_link_libraries(TGApp PUBLIC tc::TGEngine)
target_include_directories(TGApp PUBLIC "TGEngine/TGEngine/public")

add_executable(TGAppTest "application/test/TGTest.cpp" "application/util/CellEntry.cpp" "application/util/VulkanHelper.cpp"  "application/TGAppDef.cpp" "application/module/TGAppIO.cpp")
target_link_libraries(TGAppTest PUBLIC tc::TGEngine gtest_main)
target_include_directories(TGAppTest PUBLIC "TGEngine/TGEngine/public")
install(DIRECTORY "application/assets" DESTINATION $<TARGET_FILE_DIR:TGApp>)