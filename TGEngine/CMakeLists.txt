cmake_minimum_required (VERSION 3.12)

project ("TGEngine" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

add_compile_definitions(ENGINE_NAME="TGEngine" ENGINE_VERSION=8388608 APPLICATION_NAME="TGEngineTest" DEBUG)

if(WIN32)  
add_compile_options("/D_ITERATOR_DEBUG_LEVEL=0" "/MD" "/EHa")
add_link_options("/ignore:4099")
else()
add_compile_options("-Wno-deprecated-volatile")
endif()

include(FetchContent)
FETCHCONTENT_DECLARE(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
message("Downloading vulkansdk")
FETCHCONTENT_DECLARE(
	vulkansdk
	URL https://seafile.media-dienste.de/f/85da9d3e98b347a490f6/?dl=1

	BUILD_COMMAND ""
)

FETCHCONTENT_DECLARE(
	imgui
	URL https://github.com/ocornut/imgui/archive/refs/tags/v1.83.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FETCHCONTENT_MAKEAVAILABLE(googletest)
FETCHCONTENT_MAKEAVAILABLE(vulkansdk)
FETCHCONTENT_MAKEAVAILABLE(imgui)
include_directories("${vulkansdk_SOURCE_DIR}/Include" "${vulkansdk_SOURCE_DIR}/Third-Party/Include" "${imgui_SOURCE_DIR}")

set(ENGINE_DATA   
	"public/TGEngine.hpp"
	"public/graphics/vulkan/VulkanGraphicsModule.hpp"
	"public/Module.hpp"
	"public/Error.hpp"
	"public/graphics/GameGraphicsModule.hpp"
	"public/Util.hpp"
	"private/TGEngine.cpp"
	"private/Error.cpp"
    "private/graphics/Vulkan/VulkanGraphicsModule.cpp"     
    "private/graphics/GameGraphicsModule.cpp"
    "private/Util.cpp"
	"private/graphics/WindowModule.cpp" 
	"private/graphics/Vulkan/VulkanShaderModule.cpp" 
	"public/graphics/vulkan/VulkanShaderModule.hpp"
	"public/graphics/Material.hpp" 
	"public/graphics/APILayer.hpp" 
	"private/graphics/Material.cpp"  
	"public/IO/IOModule.hpp" 
	"private/IO/IOModule.cpp"
    "public/graphics/vulkan/VulkanShaderPipe.hpp" 
	"public/graphics/GameShaderModule.hpp" 
	"private/graphics/GUIModule.cpp" 
	"public/graphics/GUIModule.hpp" 
	"public/graphics/vulkan/VulkanModuleDef.hpp"
	"${imgui_SOURCE_DIR}/imconfig.h"
	"${imgui_SOURCE_DIR}/imgui.cpp"
	"${imgui_SOURCE_DIR}/imgui.h"
	"${imgui_SOURCE_DIR}/imgui_demo.cpp"
	"${imgui_SOURCE_DIR}/imgui_draw.cpp"
	"${imgui_SOURCE_DIR}/imgui_internal.h"
	"${imgui_SOURCE_DIR}/imgui_tables.cpp"
	"${imgui_SOURCE_DIR}/imgui_widgets.cpp"
	"${imgui_SOURCE_DIR}/imstb_rectpack.h"
	"${imgui_SOURCE_DIR}/imstb_textedit.h"
	"${imgui_SOURCE_DIR}/imstb_truetype.h"
	"${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.h"
)
if(WIN32)  
link_directories("${vulkansdk_SOURCE_DIR}/Lib" "${vulkansdk_SOURCE_DIR}/Third-Party/Bin")
list(APPEND 	
    "${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp"
	"${imgui_SOURCE_DIR}/backends/imgui_impl_win32.h"
)
endif()

add_library (TGEngine STATIC ${ENGINE_DATA})
target_link_libraries(TGEngine PUBLIC glslang shaderc_combined)
if(WIN32)
target_link_libraries(TGEngine PUBLIC vulkan-1)
else()
target_link_libraries(TGEngine PUBLIC vulkan m)
endif()

add_executable(TGEngineTest "test/TGTest.cpp")
target_include_directories(TGEngineTest PUBLIC "public")
add_custom_command(TARGET TGEngineTest PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/test/assets/ $<TARGET_FILE_DIR:TGEngineTest>/assets)
target_link_libraries(TGEngineTest PUBLIC TGEngine gtest_main)
add_test(NAME TGEngineTest COMMAND TGEngineTest)

add_executable(TGRenderTest "renderTest/TGRenderTest.cpp")
target_include_directories(TGRenderTest PUBLIC "public")
target_link_libraries(TGRenderTest PUBLIC TGEngine)


add_executable(TGApp "application/TGApp.cpp" "application/util/CellEntry.cpp" "application/util/VulkanHelper.cpp"  "application/TGAppDef.cpp")
target_include_directories(TGApp PUBLIC "public")
target_link_libraries(TGApp PUBLIC TGEngine)

add_executable(TGAppTest "application/test/TGTest.cpp" "application/util/CellEntry.cpp" "application/util/VulkanHelper.cpp"  "application/TGAppDef.cpp")
target_include_directories(TGAppTest PUBLIC "public")
target_link_libraries(TGAppTest PUBLIC TGEngine gtest_main)
