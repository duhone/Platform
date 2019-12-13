set(platform_root "${CMAKE_CURRENT_LIST_DIR}/..")

###############################################
#library
###############################################
set(PLATFORM_PUBLIC_HDRS
    ${platform_root}/inc/Platform/Event.h
    ${platform_root}/inc/Platform/Guid.h
    ${platform_root}/inc/Platform/MemoryMappedFile.h
    ${platform_root}/inc/Platform/PathUtils.h
    ${platform_root}/inc/Platform/PipeClient.h
    ${platform_root}/inc/Platform/PipeServer.h
    ${platform_root}/inc/Platform/Process.h
    ${platform_root}/inc/Platform/SharedLibrary.h
    ${platform_root}/inc/Platform/SharedMemory.h
    ${platform_root}/inc/Platform/Window.h
)

set(PLATFORM_SRCS
    ${platform_root}/src/windows/Event.cpp
    ${platform_root}/src/windows/Guid.cpp
    ${platform_root}/src/windows/IOCP.h
    ${platform_root}/src/windows/IOCP.cpp
    ${platform_root}/src/windows/MemoryMappedFile.cpp
    ${platform_root}/src/windows/PathUtils.cpp
    ${platform_root}/src/windows/PipeClient.cpp
    ${platform_root}/src/windows/PipeServer.cpp
    ${platform_root}/src/windows/Process.cpp
    ${platform_root}/src/windows/SharedLibrary.cpp
    ${platform_root}/src/windows/SharedMemory.cpp
    ${platform_root}/src/windows/Window.cpp
)

set(PLATFORM_BUILD
    ${platform_root}/build/build.cmake
)

add_library(platform OBJECT  
  ${PLATFORM_PUBLIC_HDRS} 
  ${PLATFORM_SRCS} 
  ${PLATFORM_BUILD}
)
						
source_group("Public Headers" FILES ${PLATFORM_PUBLIC_HDRS})
source_group("Source" FILES ${CMAKE_CURRENT_LIST_DIR})
source_group("Build" FILES ${PLATFORM_BUILD})
	
target_include_directories(platform PUBLIC "${platform_root}/inc")
target_link_libraries(platform
  spdlog
  fmt
  core 
)
				
if(IncludeTests)  
	###############################################
	#unit tests
	###############################################
set(PLATFORM_TEST_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/../tests/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tests/MemoryMappedFile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tests/SharedLibrary.cpp
)
	
	add_executable(platform_tests 
						${PLATFORM_TEST_SRCS})
							
  target_link_libraries(platform_tests 
    catch
    fmt
    platform
    core
  )		

	source_group("Source" FILES ${PLATFORM_TEST_SRCS})
						
	set_property(TARGET platform_tests APPEND PROPERTY FOLDER tests)
	
	add_custom_command(TARGET platform_tests POST_BUILD        
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
        "${CMAKE_CURRENT_LIST_DIR}/../tests/content/testdll.dll"
        $<TARGET_FILE_DIR:platform_tests>)
		
	add_custom_command(TARGET platform_tests POST_BUILD        
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
        "${CMAKE_CURRENT_LIST_DIR}/../tests/content/test.txt"
        $<TARGET_FILE_DIR:platform_tests>)

endif()
