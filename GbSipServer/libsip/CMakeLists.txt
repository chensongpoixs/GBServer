project(libsip)
cmake_minimum_required(VERSION 2.8)
MESSAGE(STATUS  "####################################")
MESSAGE(STATUS  "#############${PROJECT_NAME}###############")
MESSAGE(STATUS  "####################################")

include_directories(
   .
   	.
	./c-ares 
	./eXosip2
	./osip2
	./osipparser2
)

file(GLOB libsip_source
        
		
		c-ares/*.cpp  
		c-ares/*.cc
        c-ares/*.h
		c-ares/*.c
		
		
		eXosip2/*.cpp  
		eXosip2/*.cc
        eXosip2/*.h
		eXosip2/*.c
        
		
		osip2/*.cpp  
		osip2/*.cc
        osip2/*.h
		osip2/*.c
		
		
		osipparser2/*.cpp  
		osipparser2/*.cc
        osipparser2/*.h
		osipparser2/*.c
		
		
		osipparser2/headers/*.cpp  
		osipparser2/headers/*.cc
        osipparser2/headers/*.h
		osipparser2/headers/*.c
        )
		


	
		

add_library(${PROJECT_NAME} STATIC ${libsip_source})
#target_link_libraries(${PROJECT_NAME} )


set(AllFile ${libsip_source} )
foreach(fileItem ${AllFile})       
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${fileItem}" DIRECTORY)
	
    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")
	#message(status   "---> fileItem = ${fileItem}     GROUP ---->  ${GROUP}")
    # Group into "Source Files" and "Header Files"
    set(GROUP "${GROUP}")
    source_group("${GROUP}" FILES "${fileItem}")
endforeach()

