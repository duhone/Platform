cmake_minimum_required (VERSION 2.6)
project (Platform)

add_subdirectory("${PROJECT_SOURCE_DIR}/../../core/build" "${PROJECT_SOURCE_DIR}/../../core_build")
include_directories("${PROJECT_SOURCE_DIR}/../../3rdParty/catch")
include_directories("${PROJECT_SOURCE_DIR}/../../core/inc")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

###############################################
#options
###############################################

option(IncludeTests "Include Unit Tests" ON)

include_directories("${PROJECT_SOURCE_DIR}/../inc/platform")

###############################################
#library
###############################################
file(GLOB_RECURSE plat_inc "${PROJECT_SOURCE_DIR}/../inc/*.h")
file(GLOB_RECURSE plat_src1 "${PROJECT_SOURCE_DIR}/../src/*.h")
file(GLOB_RECURSE plat_src2 "${PROJECT_SOURCE_DIR}/../src/windows/*.cpp")
list(APPEND plat_src ${plat_inc})
list(APPEND plat_src ${plat_src1})
list(APPEND plat_src ${plat_src2})

add_library(platform OBJECT ${plat_src})
					
set_property(TARGET platform APPEND PROPERTY COMPILE_FLAGS "-WX -W4")
#set_property(TARGET core APPEND PROPERTY COMPILE_FLAGS "-Werror -Wall -Wextra")	
				
if(IncludeTests)  
	###############################################
	#unit tests
	###############################################
	file(GLOB_RECURSE plat_test_inc "${PROJECT_SOURCE_DIR}/../tests/*.h")
	file(GLOB_RECURSE plat_test_src1 "${PROJECT_SOURCE_DIR}/../tests/*.cpp")
	list(APPEND plat_test_src ${plat_test_inc})
	list(APPEND plat_test_src ${plat_test_src1})
	
	add_executable(platform_tests $<TARGET_OBJECTS:platform> ${plat_test_src})
						
	set_property(TARGET platform_tests APPEND PROPERTY FOLDER tests)
	set_property(TARGET platform_tests APPEND PROPERTY COMPILE_FLAGS "-WX -W4")
	#set_property(TARGET tests APPEND PROPERTY COMPILE_FLAGS "-Werror -Wall -Wextra")

	add_custom_command(TARGET platform_tests POST_BUILD        
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
        "${PROJECT_SOURCE_DIR}/../tests/content/testdll.dll"
        $<TARGET_FILE_DIR:platform_tests>)

endif()
