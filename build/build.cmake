include_directories("${CMAKE_CURRENT_LIST_DIR}/../inc")


###############################################
#library
###############################################
set(PLATFORM_PUBLIC_HDRS
    ${CMAKE_CURRENT_LIST_DIR}/../inc/Platform/Event.h
    ${CMAKE_CURRENT_LIST_DIR}/../inc/Platform/Guid.h
    ${CMAKE_CURRENT_LIST_DIR}/../inc/Platform/MemoryMappedFile.h
    ${CMAKE_CURRENT_LIST_DIR}/../inc/Platform/PathUtils.h
    ${CMAKE_CURRENT_LIST_DIR}/../inc/Platform/PipeClient.h
    ${CMAKE_CURRENT_LIST_DIR}/../inc/Platform/PipeServer.h
    ${CMAKE_CURRENT_LIST_DIR}/../inc/Platform/Process.h
    ${CMAKE_CURRENT_LIST_DIR}/../inc/Platform/SharedLibrary.h
    ${CMAKE_CURRENT_LIST_DIR}/../inc/Platform/SharedMemory.h
    ${CMAKE_CURRENT_LIST_DIR}/../inc/Platform/Window.h
)

set(PLATFORM_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/../src/windows/Event.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../src/windows/Guid.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../src/windows/IOCP.h
    ${CMAKE_CURRENT_LIST_DIR}/../src/windows/IOCP.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../src/windows/MemoryMappedFile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../src/windows/PathUtils.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../src/windows/PipeClient.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../src/windows/PipeServer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../src/windows/Process.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../src/windows/SharedLibrary.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../src/windows/SharedMemory.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../src/windows/Window.cpp
)

set(PLATFORM_BUILD
    ${CMAKE_CURRENT_LIST_DIR}/../build/build.cmake
)

add_library(platform OBJECT  ${PLATFORM_PUBLIC_HDRS} ${PLATFORM_SRCS} ${PLATFORM_BUILD})
						
source_group("Public Headers" FILES ${PLATFORM_PUBLIC_HDRS})
source_group("Source" FILES ${CMAKE_CURRENT_LIST_DIR})
source_group("Build" FILES ${PLATFORM_BUILD})
	
				
if(IncludeTests)  
	###############################################
	#unit tests
	###############################################
set(PLATFORM_TEST_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/../tests/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tests/MemoryMappedFile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tests/SharedLibrary.cpp
)
	
	add_executable(platform_tests  $<TARGET_OBJECTS:platform>
						$<TARGET_OBJECTS:core>
						$<TARGET_OBJECTS:math>
						${PLATFORM_TEST_SRCS})
						
	source_group("Source" FILES ${PLATFORM_TEST_SRCS})
						
	set_property(TARGET platform_tests APPEND PROPERTY FOLDER tests)
	
	add_custom_command(TARGET platform_tests POST_BUILD        
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
        "${PROJECT_SOURCE_DIR}/../tests/content/testdll.dll"
        $<TARGET_FILE_DIR:platform_tests>)
		
	add_custom_command(TARGET platform_tests POST_BUILD        
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
        "${PROJECT_SOURCE_DIR}/../tests/content/test.txt"
        $<TARGET_FILE_DIR:platform_tests>)

endif()
