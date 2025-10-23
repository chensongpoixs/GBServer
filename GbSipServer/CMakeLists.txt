

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
	#../libswagger
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
		
)
 
file(GLOB GbSipServer_source
        
		utils/*.cpp 
		utils/*.h 
		utils/*.c 
		 utils/*.hpp

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
#add_dependencies(${PROJECT_NAME} libswagger)
 add_dependencies(${PROJECT_NAME} libsip)

set(ALL_LIB 

 crypt32.lib
			iphlpapi.lib
			secur32.lib
#libswagger
libsip 
Dnsapi.lib   
delayimp.lib
ws2_32.lib
 Qwave.lib
 libcrypto.lib 
		libssl.lib 
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

			  libp2p_peerconnection 
			  libice
			  api 
			audio_device
			audio_processing
			desktop_capture
			video_capture
			video_coding
			call
			common_audio
			common_video
			libwebrtc 
			logging 
			media
			p2p 
			pc 
			rtc_base
			   
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