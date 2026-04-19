cmake_minimum_required(VERSION 3.5)
project(GbMediaServer)
MESSAGE(STATUS  "####################################")
MESSAGE(STATUS  "#############${PROJECT_NAME}###############")
MESSAGE(STATUS  "####################################")
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD 17)

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

		#-D_AFXDLL
#-D_WINSOCK_DEPRECATED_NO_WARNINGS
#-D_CRT_SECURE_NO_DEPRECATE
#-D_CRT_NONSTDC_NO_WARNINGS
#-DWIN32
#-D_DEBUG
#-D_CONSOLE
		# Enable RTC play test: capture desktop and stream
		-DTEST_RTC_PLAY=1

        ## define path to swagger-ui static resources folder
       # -DOATPP_SWAGGER_RES_PATH="${DEPS_DIR}/oatpp/include/oatpp-1.4.0/bin/oatpp-swagger/res"
	   #
       # ## SQLite database file
       # -DDATABASE_FILE="${CMAKE_CURRENT_SOURCE_DIR}/db.sqlite"
       # ## SQLite database test file
       # -DTESTDATABASE_FILE="${CMAKE_CURRENT_SOURCE_DIR}/test-db.sqlite"
	   #
       # ## Path to database migration scripts
       # -DDATABASE_MIGRATIONS="${CMAKE_CURRENT_SOURCE_DIR}/sql"
)


# ============================================================================
# WebSocket stats: auto-download third-party dependencies
# Author: chensong
# Date: 2025-10-18
# ============================================================================

# Third-party download root
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
file(MAKE_DIRECTORY ${THIRD_PARTY_DIR})

# ============================================================================
# Auto-download script invocation
# ============================================================================
# Whether missing libs require download
set(NEED_DOWNLOAD FALSE)

# Absolute paths for existence checks
set(WEBSOCKETPP_CHECK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/websocketpp/websocketpp/server.hpp")
set(JSON_CHECK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/json/include/nlohmann/json.hpp")
set(ASIO_CHECK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/asio/asio/include/asio.hpp")

if(NOT EXISTS "${WEBSOCKETPP_CHECK_PATH}")
    set(NEED_DOWNLOAD TRUE)
    message(STATUS "websocketpp not found")
endif()

if(NOT EXISTS "${JSON_CHECK_PATH}")
    set(NEED_DOWNLOAD TRUE)
    message(STATUS "nlohmann/json not found")
endif()

if(NOT EXISTS "${ASIO_CHECK_PATH}")
    set(NEED_DOWNLOAD TRUE)
    message(STATUS "ASIO not found")
endif()

# If anything is missing, run the download script
if(NEED_DOWNLOAD)
    message(STATUS "========================================")
    message(STATUS "WebSocket dependencies missing; downloading...")
    message(STATUS "========================================")
    
    # Pick script by OS
    if(WIN32)
        # Windows: PowerShell
        set(DOWNLOAD_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/download_websocket_libs.ps1")
        if(EXISTS ${DOWNLOAD_SCRIPT})
            message(STATUS "Running download script: ${DOWNLOAD_SCRIPT}")
            execute_process(
                COMMAND powershell -ExecutionPolicy Bypass -File ${DOWNLOAD_SCRIPT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE DOWNLOAD_RESULT
                OUTPUT_VARIABLE DOWNLOAD_OUTPUT
                ERROR_VARIABLE DOWNLOAD_ERROR
                # Echo output while running
                ECHO_OUTPUT_VARIABLE
                ECHO_ERROR_VARIABLE
            )
            
            if(DOWNLOAD_RESULT EQUAL 0)
                message(STATUS "Download script finished successfully")
            else()
                message(WARNING "Download script failed")
                if(DOWNLOAD_ERROR)
                    message(WARNING "Error output: ${DOWNLOAD_ERROR}")
                endif()
                message(WARNING "Run manually: .\\download_websocket_libs.ps1")
            endif()
        else()
            message(WARNING "Download script not found: ${DOWNLOAD_SCRIPT}")
        endif()
    else()
        # Linux/Unix: Bash
        set(DOWNLOAD_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/download_websocket_libs.sh")
        if(EXISTS ${DOWNLOAD_SCRIPT})
            message(STATUS "Running download script: ${DOWNLOAD_SCRIPT}")
            
            # chmod +x
            execute_process(
                COMMAND chmod +x ${DOWNLOAD_SCRIPT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            )
            
            # Run download script
            execute_process(
                COMMAND bash ${DOWNLOAD_SCRIPT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE DOWNLOAD_RESULT
                OUTPUT_VARIABLE DOWNLOAD_OUTPUT
                ERROR_VARIABLE DOWNLOAD_ERROR
                # Echo output while running
                ECHO_OUTPUT_VARIABLE
                ECHO_ERROR_VARIABLE
            )
            
            if(DOWNLOAD_RESULT EQUAL 0)
                message(STATUS "Download script finished successfully")
            else()
                message(WARNING "Download script failed")
                if(DOWNLOAD_ERROR)
                    message(WARNING "Error output: ${DOWNLOAD_ERROR}")
                endif()
                message(WARNING "Run manually: ./download_websocket_libs.sh")
            endif()
        else()
            message(WARNING "Download script not found: ${DOWNLOAD_SCRIPT}")
        endif()
    endif()
    
    message(STATUS "========================================")
endif()

# ============================================================================
# 1. websocketpp (header-only)
# ============================================================================
message(STATUS "Checking websocketpp...")

# Absolute paths for existence checks
set(WEBSOCKETPP_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/third_party/websocketpp/websocketpp/server.hpp")
message(STATUS "Looking for: ${WEBSOCKETPP_HEADER}")

if(EXISTS "${WEBSOCKETPP_HEADER}")
    set(WEBSOCKETPP_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/websocketpp")
    message(STATUS "Found local websocketpp: ${WEBSOCKETPP_INCLUDE_DIR}")
else()
    message(WARNING "websocketpp still not found after download attempt")
    message(WARNING "Expected at: ${WEBSOCKETPP_HEADER}")
    message(WARNING "WebSocket support will be disabled")
    message(WARNING "Download manually or run:")
    if(WIN32)
        message(WARNING "  .\\download_websocket_libs.ps1")
    else()
        message(WARNING "  ./download_websocket_libs.sh")
    endif()
    set(WEBSOCKETPP_INCLUDE_DIR "")
endif()

# ============================================================================
# 2. nlohmann/json (header-only)
# ============================================================================
message(STATUS "Checking nlohmann/json...")

# Absolute paths for existence checks
set(NLOHMANN_JSON_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/third_party/json/include/nlohmann/json.hpp")
message(STATUS "Looking for: ${NLOHMANN_JSON_HEADER}")

if(EXISTS "${NLOHMANN_JSON_HEADER}")
    set(NLOHMANN_JSON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/json/include")
    message(STATUS "Found local nlohmann/json: ${NLOHMANN_JSON_INCLUDE_DIR}")
else()
    message(WARNING "nlohmann/json still not found after download attempt")
    message(WARNING "Expected at: ${NLOHMANN_JSON_HEADER}")
    message(WARNING "WebSocket support will be disabled")
    message(WARNING "Download manually or run:")
    if(WIN32)
        message(WARNING "  .\\download_websocket_libs.ps1")
    else()
        message(WARNING "  ./download_websocket_libs.sh")
    endif()
    set(NLOHMANN_JSON_INCLUDE_DIR "")
endif()

# ============================================================================
# 3. ASIO (header-only, standalone)
# ============================================================================
message(STATUS "Checking ASIO...")

# Absolute paths for existence checks
set(ASIO_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/third_party/asio/asio/include/asio.hpp")
message(STATUS "Looking for: ${ASIO_HEADER}")

if(EXISTS "${ASIO_HEADER}")
    set(ASIO_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/asio/asio/include")
    message(STATUS "Found local ASIO: ${ASIO_INCLUDE_DIR}")
else()
    message(WARNING "ASIO still not found after download attempt")
    message(WARNING "Expected at: ${ASIO_HEADER}")
    message(WARNING "WebSocket support will be disabled")
    message(WARNING "Download manually or run:")
    if(WIN32)
        message(WARNING "  .\\download_websocket_libs.ps1")
    else()
        message(WARNING "  ./download_websocket_libs.sh")
    endif()
    set(ASIO_INCLUDE_DIR "")
endif()

# ============================================================================
# Enable WebSocket if all deps are present
# ============================================================================
if(WEBSOCKETPP_INCLUDE_DIR AND NLOHMANN_JSON_INCLUDE_DIR AND ASIO_INCLUDE_DIR)
    message(STATUS "========================================")
    message(STATUS "WebSocket dependencies ready")
    message(STATUS "WebSocket include dirs:")
    message(STATUS "  websocketpp: ${WEBSOCKETPP_INCLUDE_DIR}")
    message(STATUS "  nlohmann/json: ${NLOHMANN_JSON_INCLUDE_DIR}")
    message(STATUS "  ASIO: ${ASIO_INCLUDE_DIR}")
    message(STATUS "========================================")
    message(STATUS "To enable WebSocket at compile time:")
    message(STATUS "1. Edit server/ws_stats_service.h")
    message(STATUS "   Change #define ENABLE_WEBSOCKET from 0 to 1")
    message(STATUS "2. Re-run cmake")
    message(STATUS "========================================")
    
    # WEBSOCKET_LIBS_AVAILABLE: deps present (still need ENABLE_WEBSOCKET in header)
    add_definitions(-DWEBSOCKET_LIBS_AVAILABLE=1)
else()
    message(STATUS "========================================")
    message(STATUS "WebSocket dependencies not ready")
    message(STATUS "WebSocket disabled at build (ENABLE_WEBSOCKET=0 in header)")
    message(STATUS "========================================")
    
    # Clear so we do not add empty include paths
    set(WEBSOCKETPP_INCLUDE_DIR "")
    set(NLOHMANN_JSON_INCLUDE_DIR "")
    set(ASIO_INCLUDE_DIR "")
    add_definitions(-DWEBSOCKET_LIBS_AVAILABLE=0)
endif()
# ============================================================================
# Include directories
# ============================================================================
include_directories(
	. 
)

# Optional WebSocket include dirs
if(WEBSOCKETPP_INCLUDE_DIR)
    include_directories(${WEBSOCKETPP_INCLUDE_DIR})
    message(STATUS "Added websocketpp include: ${WEBSOCKETPP_INCLUDE_DIR}")
endif()

if(NLOHMANN_JSON_INCLUDE_DIR)
    include_directories(${NLOHMANN_JSON_INCLUDE_DIR})
    message(STATUS "Added nlohmann/json include: ${NLOHMANN_JSON_INCLUDE_DIR}")
endif()

if(ASIO_INCLUDE_DIR)
    include_directories(${ASIO_INCLUDE_DIR})
    message(STATUS "Added ASIO include: ${ASIO_INCLUDE_DIR}")
endif()

include_directories(
	#./swagger
	#../libswagger
	#${DEPS_DIR}/oatpp/include/oatpp-1.4.0
	#${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp
	#${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp-sqlite
	#${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp-swagger
	#${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp-sqlite/sqlite
	#${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp-sqlite
	#${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp-sqlite
	 
) 
#)
link_directories( 

#${DEPS_DIR}/oatpp/lib/oatpp-1.4.0

)

#
#FILE(GLOB  swagger_source
#
#		swagger/controller/*.cpp 
#        swagger/controller/*.hpp
#		swagger/controller/*.h
#        swagger/db/*.hpp
#		swagger/db/*.cpp 
#		swagger/db/*.h
#        swagger/dto/*.h
#        swagger/dto/*.hpp
#        swagger/dto/*.cpp 
#		swagger/service/*.h
#        swagger/service/*.cpp
#        swagger/service/*.hpp
#		swagger/*.h 
#		swagger/*.cpp 
#		swagger/*.hpp
#
#		
#)

 include_directories(
	 ../../libwebrtc
	#../../deps/libyuv/include
	 ../../deps/abseil-cpp/
	 
	 
)
#link_directories( 
# 
#)

FILE(GLOB    Media_source


			server/*.h
			server/*.cpp 
	 consumer/*.h 
	 consumer/*.cpp 
	 	 producer/*.h 
	 producer/*.cpp 
	 utils/*.h 
	 utils/*.cpp 
	 share/*.h
	 share/*.cpp 
)
 
file(GLOB GbMediaServer_source
        


		 
		*.h
	*.cpp 
	*.hpp 
	 
	
        )

#add_library(GbMedia STATIC  ${Media_source} 
#
#	#${swagger_source}
#)

#add_subdirectory(swagger)
 
#add_subdirectory(libsip)
 
add_executable(${PROJECT_NAME}     ${GbMediaServer_source}  ${Media_source}   )
if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.8 AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/utils/file_log_writer.cpp")
	set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/utils/file_log_writer.cpp"
		PROPERTIES COMPILE_FEATURES cxx_std_17)
endif()
add_dependencies(${PROJECT_NAME} libmedia_codec)
add_dependencies(${PROJECT_NAME} libmedia_transfer_protocol)
#add_dependencies(${PROJECT_NAME}   libswagger)
# add_dependencies(${PROJECT_NAME} libsip)

set(ALL_LIB 
			 
			api 
			libice
		libp2p_peerconnection 
		libmedia_codec 
		libmedia_transfer_protocol
		#libcross_platform_collection_render
			audio_device
			#audio_processing
			desktop_capture
			video_capture
			video_coding
			audio_coding

			call
			common_audio
			common_video
			libwebrtc 
			logging 
			media
			p2p 
			pc 
			rtc_base
		 	
			 ###########################
 
				 ## AAC
				 faad
				 opus
				 libsrtp2
				 x264
		   jpeg-static
		

		#curl 
		jsoncpp_static
		 
		 

	 


		  
	 
		
		
		yaml_cpp
		 
		usrsctp

	 
		 
		 
		#abseil_dll
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
		
			)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "current platform: windows")
    set(WIN_LIB 
		libvpx
	     audio_processing
		 libvpx_asm
		Dnsapi.lib   
			delayimp.lib
			ws2_32.lib
			 Qwave.lib
			 crypt32.lib
			iphlpapi.lib
			secur32.lib
			crypt32.lib
			iphlpapi.lib
			secur32.lib
			 iphlpapi.lib
		 
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
			  # WebRTC uses timeGetTime on Windows
			  winmm.lib  
			  dbghelp.lib
		 crypt32.lib
		iphlpapi.lib
       secur32.lib
	   d3d11.lib
      dxgi.lib
	  	 dmoguids.lib
          wmcodecdspuuid.lib
        amstrmid.lib
         msdmo.lib
        oleaut32.lib

		Ws2_32.lib
		Wldap32.lib
	 Imm32 
			comctl32
			ws2_32.lib
			winmm.lib

			libcrypto
			libssl
			yuv
		#libcrypto.lib 
		#libssl.lib 
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
			  # WebRTC uses timeGetTime on Windows
			  winmm.lib
			  #ucrt.lib
			  # dbghelp: libuv dependency on Windows
			  dbghelp.lib
			   dmoguids.lib
          wmcodecdspuuid.lib
        amstrmid.lib
         msdmo.lib
        oleaut32.lib
	)
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
	message(STATUS "current platform: unix")
    set(LINUX_LIB 
		vpx

				crypto
				ssl
				m 
				dl
				rt
	)

endif()
 
target_link_libraries(${PROJECT_NAME}   ${ALL_LIB}   

	${WIN_LIB}
	${LINUX_LIB}
)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
	target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs)
endif()

#set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /SAFESEH:NO")
			# Console (/SUBSYSTEM:CONSOLE)
#set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE /SAFESEH:NO")

set(AllFile 
${GbMediaServer_source} 
            ${Media_source} 
	# ${swagger_source}
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
#set_property(TARGET GbMedia 				PROPERTY FOLDER  ${DIR_NAME_GBServer} ) 
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})