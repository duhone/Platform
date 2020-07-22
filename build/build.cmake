set(root "${CMAKE_CURRENT_LIST_DIR}/..")

###############################################
#library
###############################################
set(PUBLIC_HDRS
    ${root}/inc/Platform/Event.h
    ${root}/inc/Platform/Guid.h
    ${root}/inc/Platform/MemoryMappedFile.h
    ${root}/inc/Platform/PathUtils.h
    ${root}/inc/Platform/PipeClient.h
    ${root}/inc/Platform/PipeServer.h
    ${root}/inc/Platform/Process.h
    ${root}/inc/Platform/SharedLibrary.h
    ${root}/inc/Platform/SharedMemory.h
    ${root}/inc/Platform/Window.h
)

set(SRCS
    ${root}/src/windows/Event.cpp
    ${root}/src/windows/Guid.cpp
    ${root}/src/windows/IOCP.h
    ${root}/src/windows/IOCP.cpp
    ${root}/src/windows/MemoryMappedFile.cpp
    ${root}/src/windows/PathUtils.cpp
    ${root}/src/windows/PipeClient.cpp
    ${root}/src/windows/PipeServer.cpp
    ${root}/src/windows/Process.cpp
    ${root}/src/windows/SharedLibrary.cpp
    ${root}/src/windows/SharedMemory.cpp
    ${root}/src/windows/Window.cpp
)

set(BUILD
    ${root}/build/build.cmake
)

add_library(platform  
  ${PUBLIC_HDRS} 
  ${SRCS} 
  ${BUILD}
)
		
settingsCR(platform)	
usePCH(platform core)
				
target_include_directories(platform PUBLIC "${root}/inc")
target_link_libraries(platform PUBLIC
  doctest
  function2
  spdlog
  fmt
  core 
)
			
###############################################
#unit tests
###############################################
set(PUBLIC_HDRS
)

set(SRCS
    ${root}/tests/main.cpp
    ${root}/tests/MemoryMappedFile.cpp
    ${root}/tests/SharedLibrary.cpp
)
	
set(BUILD
)

add_executable(platform_tests 
					${SRCS})
usePCH(platform_tests core)
						
target_link_libraries(platform_tests 
	doctest
	fmt
	platform
	core
)		

settingsCR(platform_tests)	
					
set_property(TARGET platform_tests APPEND PROPERTY FOLDER tests)

set(TEST_DATA 
	"${root}/tests/content/testdll.dll"
	"${root}/tests/content/test.txt"
)

add_custom_command(TARGET platform_tests POST_BUILD        
COMMAND ${CMAKE_COMMAND} -E copy_if_different  
	${TEST_DATA}
	$<TARGET_FILE_DIR:platform_tests>)
	

