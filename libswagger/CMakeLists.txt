

project(libswagger)
cmake_minimum_required(VERSION 2.8)
MESSAGE(STATUS  "####################################")
MESSAGE(STATUS  "#############${PROJECT_NAME}###############")
MESSAGE(STATUS  "####################################")


# oatpp C++17 
set(CMAKE_CXX_STANDARD 17)


add_definitions(


#-D_WINSOCK_DEPRECATED_NO_WARNINGS
#-D_CRT_SECURE_NO_DEPRECATE
#-D_CRT_NONSTDC_NO_WARNINGS
#-DWIN32
#-D_DEBUG
#-D_CONSOLE


        ## define path to swagger-ui static resources folder
        -DOATPP_SWAGGER_RES_PATH="${DEPS_DIR}/oatpp/include/oatpp-1.4.0/bin/oatpp-swagger/res"

        ## SQLite database file
        -DDATABASE_FILE="${CMAKE_CURRENT_SOURCE_DIR}/db.sqlite"
        ## SQLite database test file
        -DTESTDATABASE_FILE="${CMAKE_CURRENT_SOURCE_DIR}/test-db.sqlite"

        ## Path to database migration scripts
        -DDATABASE_MIGRATIONS="${CMAKE_CURRENT_SOURCE_DIR}/sql"
)
#add_definitions(
				#	-DWIN32
				#	-D_DEBUG
				##	-D_WINDOWS
				#	-D_UNICODE
				#	-DUNICODE
				#	-D_AFXDLL
#)

 
 include_directories( 
	.
	${DEPS_DIR}/oatpp/include/oatpp-1.4.0
	${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp
	${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp-sqlite
	${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp-swagger
	${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp-sqlite/sqlite
	${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp-sqlite
	${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp-sqlite
)

#

file(GLOB libswagger_source 
	 controller/*.cpp 
        controller/*.hpp
		controller/*.h
        db/*.hpp
		db/*.cpp 
		db/*.h
        dto/*.h
        dto/*.hpp
        dto/*.cpp 
		service/*.h
        service/*.cpp
        service/*.hpp
	*.h
	*.cpp 
	 *.cc 
	 *.hpp 
) 


 
add_library(${PROJECT_NAME}   STATIC  ${libswagger_source})
 

 


#set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /SAFESEH:NO")


set(AllFile ${libswagger_source} )
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



set_property(TARGET ${PROJECT_NAME}  				PROPERTY FOLDER ${DIR_NAME_GBServer}) 

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})