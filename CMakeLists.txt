include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.nasm.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.crinkler.txt)

cmake_minimum_required (VERSION 3.10)
project(intro CXX C)

set(CMAKE_CXX_STANDARD 17)

set(DEPENDENCY_FOLDER "Dependencies")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER ${DEPENDENCY_FOLDER})

# Output binary to predictable location (fixes cyclic dependency issues).
set(BINARY_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
	string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_OUT_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_OUT_DIR})
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_OUT_DIR})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/.clang-format ${CMAKE_CURRENT_BINARY_DIR}/.clang-format COPYONLY)

set(LIB_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
set(BUILD_TOOLS_DIR ${CMAKE_CURRENT_BINARY_DIR}/build_tools)

set(CMAKE_ASM_NASM_COMPILER ${NASM_PATH})
enable_language(ASM_NASM)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c" "src/*.hpp" "src/*.h" "src/*.tpp" "src/*.asm" "src/*.glsl" "src/*.frag" "src/*.vert")
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src) # For based paths.
target_include_directories(${PROJECT_NAME} PRIVATE ${LIB_INCLUDE_DIR})
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG_BUILD=1>)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
	# using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

	find_package(OpenGL REQUIRED)
	target_link_libraries(${PROJECT_NAME} PRIVATE dsound winmm ${OPENGL_LIBRARIES})
	
	string(REGEX REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")

	target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX /EHs-c- /Zc:__cplusplus /permissive-)
	target_compile_options(${PROJECT_NAME} PRIVATE /FAs /Gr /arch:IA32)
	target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/O2 /Ob1 /Oi /Os /Oy /GR- /QIfist>)
	target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/GF /MT /Zp1 /GS- /Gy /fp:fast /fp:except->)
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE "${LINK_FLAGS_RELEASE} /NODEFAULTLIB /SUBSYSTEM:WINDOWS
		/ENTRY:release_main /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO /CRINKLER /HASHTRIES:300 /COMPMODE:SLOW /ORDERTRIES:4000
		/PROGRESSGUI /RANGE:opengl32 /TRANSFORM:CALLS /NOINITIALIZERS /UNALIGNCODE /REPORT:crinkler_report.html")

	# final options /TINYIMPORT
	set_target_properties(${PROJECT_NAME} PROPERTIES VS_USER_PROPS ${CMAKE_CURRENT_SOURCE_DIR}/crinkler.props)

	add_definitions(-DNOMINMAX -DWINDOWS -DWIN32_LEAN_AND_MEAN -DWIN32_EXTRA_LEAN -DVC_EXTRALEAN)# -D_CRT_SECURE_NO_WARNINGS)

endif()

file(DOWNLOAD https://www.khronos.org/registry/OpenGL/api/GL/glext.h ${LIB_INCLUDE_DIR}/glext.h)
file(DOWNLOAD https://www.khronos.org/registry/EGL/api/KHR/khrplatform.h ${LIB_INCLUDE_DIR}/KHR/khrplatform.h)
file(DOWNLOAD https://www.khronos.org/registry/OpenGL/api/GL/wglext.h ${LIB_INCLUDE_DIR}/wglext.h)

