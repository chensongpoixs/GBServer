

project(GbSipServer)
cmake_minimum_required(VERSION 2.8)
MESSAGE(STATUS  "####################################")
MESSAGE(STATUS  "#############${PROJECT_NAME}###############")
MESSAGE(STATUS  "####################################")
set(CMAKE_CXX_STANDARD 17)


#set(DEPS_DIR  "D:/dep/c_dep")

#add_definitions(
				#	-DWIN32
				#	-D_DEBUG
				##	-D_WINDOWS
				#	-D_UNICODE
				#	-DUNICODE
				#	-D_AFXDLL
#)

add_definitions(
        ## define path to swagger-ui static resources folder
        -DOATPP_SWAGGER_RES_PATH="${DEPS_DIR}/oatpp/include/oatpp-1.4.0/bin/oatpp-swagger/res"

        ## SQLite database file
        -DDATABASE_FILE="${CMAKE_CURRENT_SOURCE_DIR}/db.sqlite"
        ## SQLite database test file
        -DTESTDATABASE_FILE="${CMAKE_CURRENT_SOURCE_DIR}/test-db.sqlite"

        ## Path to database migration scripts
        -DDATABASE_MIGRATIONS="${CMAKE_CURRENT_SOURCE_DIR}/sql"
)


 include_directories(
	 ../../libwebrtc
	#../../deps/libyuv/include
	 ../../deps/abseil-cpp/
	.
	../../deps/libsip
	../../deps/libsip/c-ares 
	../../deps/libsip/eXosip2
	../../deps/libsip/osip2
	../../deps/libsip/osipparser2
	./utils 
	./swagger
	${DEPS_DIR}/oatpp/include/oatpp-1.4.0
	${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp
	${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp-sqlite
	${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp-swagger
	${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp-sqlite/sqlite
	${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp-sqlite
	${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp-sqlite
	 
)
link_directories( 

${DEPS_DIR}/oatpp/lib/oatpp-1.4.0

)


FILE(GLOB  swagger_source

		swagger/controller/*.cpp 
        swagger/controller/*.hpp
		swagger/controller/*.h
        swagger/db/*.hpp
		swagger/db/*.cpp 
		swagger/db/*.h
        swagger/dto/*.h
        swagger/dto/*.hpp
        swagger/dto/*.cpp 
		swagger/service/*.h
        swagger/service/*.cpp
        swagger/service/*.hpp
		swagger/*.h
        swagger/*.hpp 
        swagger/*.cpp 
)
 
file(GLOB GbSipServer_source
        
		utils/*.cpp 
		utils/*.h 
		utils/*.c 
		 
		*.h
	*.cpp 
	
	sql/*.sql 
	
        )
		


#
#add_subdirectory(curl-7.81.0)
#add_subdirectory(D:/Work/rtc/crtcserver/deps/curl-7.81.0)
#add_subdirectory(libsip)
 
add_executable(${PROJECT_NAME}     ${GbSipServer_source} 

							${swagger_source}
								)
 add_dependencies(${PROJECT_NAME} libsip)

set(ALL_LIB 

libsip 
Dnsapi.lib   
delayimp.lib
ws2_32.lib
 Qwave.lib

 #
 oatpp
 oatpp-sqlite
 oatpp-swagger
 sqlite

 # absl 
 abseil_dll
		absl_cordz_functions
		absl_cordz_handle
		absl_cordz_info
		absl_cordz_sample_token
		absl_flags
		absl_flags_commandlineflag_internal
		absl_flags_internal
		absl_flags_marshalling
		absl_flags_parse
		absl_flags_private_handle_accessor
		absl_flags_program_name
		absl_flags_reflection
		absl_flags_usage
		absl_flags_usage_internal
		absl_low_level_hash
		absl_random_internal_distribution_test_util
 # rtc_base
 logging

 async_resolver_interface
 async_socket
 bitstream_reader
 callback_list
 checks
 ip_address
 net_helpers
 network_constants
 null_socket_server
 platform_thread
platform_thread_types
 rate_limiter
 rtc_base
 rtc_base_approved
 rtc_base_win
 #rtc_base_win_wgc
 rtc_event
 rtc_numerics
 rtc_task_queue
 rtc_task_queue_win
 socket
 socket_address
 stringutils
 threading
 timeutils
 weak_ptr
 win32

 base64
 sigslot 

 iphlpapi.lib
		#crypto32.lib 
		ws2_32   
		crypt32
		Shell32.lib
		 advapi32
              iphlpapi
              psapi
              shell32
              user32
              userenv
              ws2_32
			  mswsock.lib
			  #  webrtc  timeGetTime  
			  winmm.lib
			  #ucrt.lib
			  #  libuv.lib  的依赖库
			  dbghelp.lib


			)
 
target_link_libraries(${PROJECT_NAME}   ${ALL_LIB} )

#set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /SAFESEH:NO")


set(AllFile ${GbSipServer_source} 

	${swagger_source}
)
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



set_property(TARGET ${PROJECT_NAME}  				PROPERTY FOLDER  ${DIR_NAME_GBServer} ) 

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})