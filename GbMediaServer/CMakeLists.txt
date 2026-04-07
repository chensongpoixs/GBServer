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
		# 开启测试RTC播放  采集当前桌面画面传输
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
# WebSocket统计服务依赖库自动下载配置
# 作者: chensong
# 日期: 2025-10-18
# ============================================================================

# 设置第三方库下载目录
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
file(MAKE_DIRECTORY ${THIRD_PARTY_DIR})

# ============================================================================
# 自动下载脚本调用
# ============================================================================
# 检查是否需要下载库
set(NEED_DOWNLOAD FALSE)

# 使用绝对路径检查
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

# 如果需要下载，自动调用下载脚本
if(NEED_DOWNLOAD)
    message(STATUS "========================================")
    message(STATUS "WebSocket依赖库缺失，正在自动下载...")
    message(STATUS "========================================")
    
    # 根据操作系统选择脚本
    if(WIN32)
        # Windows: 使用PowerShell脚本
        set(DOWNLOAD_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/download_websocket_libs.ps1")
        if(EXISTS ${DOWNLOAD_SCRIPT})
            message(STATUS "执行下载脚本: ${DOWNLOAD_SCRIPT}")
            execute_process(
                COMMAND powershell -ExecutionPolicy Bypass -File ${DOWNLOAD_SCRIPT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE DOWNLOAD_RESULT
                OUTPUT_VARIABLE DOWNLOAD_OUTPUT
                ERROR_VARIABLE DOWNLOAD_ERROR
                # 实时显示输出
                ECHO_OUTPUT_VARIABLE
                ECHO_ERROR_VARIABLE
            )
            
            if(DOWNLOAD_RESULT EQUAL 0)
                message(STATUS "下载脚本执行成功")
            else()
                message(WARNING "下载脚本执行失败")
                if(DOWNLOAD_ERROR)
                    message(WARNING "错误信息: ${DOWNLOAD_ERROR}")
                endif()
                message(WARNING "请手动运行: .\\download_websocket_libs.ps1")
            endif()
        else()
            message(WARNING "下载脚本不存在: ${DOWNLOAD_SCRIPT}")
        endif()
    else()
        # Linux/Unix: 使用Bash脚本
        set(DOWNLOAD_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/download_websocket_libs.sh")
        if(EXISTS ${DOWNLOAD_SCRIPT})
            message(STATUS "执行下载脚本: ${DOWNLOAD_SCRIPT}")
            
            # 确保脚本有执行权限
            execute_process(
                COMMAND chmod +x ${DOWNLOAD_SCRIPT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            )
            
            # 执行下载脚本
            execute_process(
                COMMAND bash ${DOWNLOAD_SCRIPT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE DOWNLOAD_RESULT
                OUTPUT_VARIABLE DOWNLOAD_OUTPUT
                ERROR_VARIABLE DOWNLOAD_ERROR
                # 实时显示输出
                ECHO_OUTPUT_VARIABLE
                ECHO_ERROR_VARIABLE
            )
            
            if(DOWNLOAD_RESULT EQUAL 0)
                message(STATUS "下载脚本执行成功")
            else()
                message(WARNING "下载脚本执行失败")
                if(DOWNLOAD_ERROR)
                    message(WARNING "错误信息: ${DOWNLOAD_ERROR}")
                endif()
                message(WARNING "请手动运行: ./download_websocket_libs.sh")
            endif()
        else()
            message(WARNING "下载脚本不存在: ${DOWNLOAD_SCRIPT}")
        endif()
    endif()
    
    message(STATUS "========================================")
endif()

# ============================================================================
# 1. websocketpp库（header-only）
# ============================================================================
message(STATUS "Checking websocketpp...")

# 使用绝对路径检查
set(WEBSOCKETPP_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/third_party/websocketpp/websocketpp/server.hpp")
message(STATUS "Looking for: ${WEBSOCKETPP_HEADER}")

if(EXISTS "${WEBSOCKETPP_HEADER}")
    set(WEBSOCKETPP_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/websocketpp")
    message(STATUS "Found local websocketpp: ${WEBSOCKETPP_INCLUDE_DIR}")
else()
    message(WARNING "websocketpp still not found after download attempt")
    message(WARNING "Expected at: ${WEBSOCKETPP_HEADER}")
    message(WARNING "WebSocket功能将被禁用")
    message(WARNING "请手动下载或运行下载脚本:")
    if(WIN32)
        message(WARNING "  .\\download_websocket_libs.ps1")
    else()
        message(WARNING "  ./download_websocket_libs.sh")
    endif()
    set(WEBSOCKETPP_INCLUDE_DIR "")
endif()

# ============================================================================
# 2. nlohmann/json库（header-only）
# ============================================================================
message(STATUS "Checking nlohmann/json...")

# 使用绝对路径检查
set(NLOHMANN_JSON_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/third_party/json/include/nlohmann/json.hpp")
message(STATUS "Looking for: ${NLOHMANN_JSON_HEADER}")

if(EXISTS "${NLOHMANN_JSON_HEADER}")
    set(NLOHMANN_JSON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/json/include")
    message(STATUS "Found local nlohmann/json: ${NLOHMANN_JSON_INCLUDE_DIR}")
else()
    message(WARNING "nlohmann/json still not found after download attempt")
    message(WARNING "Expected at: ${NLOHMANN_JSON_HEADER}")
    message(WARNING "WebSocket功能将被禁用")
    message(WARNING "请手动下载或运行下载脚本:")
    if(WIN32)
        message(WARNING "  .\\download_websocket_libs.ps1")
    else()
        message(WARNING "  ./download_websocket_libs.sh")
    endif()
    set(NLOHMANN_JSON_INCLUDE_DIR "")
endif()

# ============================================================================
# 3. ASIO库（header-only, standalone）
# ============================================================================
message(STATUS "Checking ASIO...")

# 使用绝对路径检查
set(ASIO_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/third_party/asio/asio/include/asio.hpp")
message(STATUS "Looking for: ${ASIO_HEADER}")

if(EXISTS "${ASIO_HEADER}")
    set(ASIO_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/asio/asio/include")
    message(STATUS "Found local ASIO: ${ASIO_INCLUDE_DIR}")
else()
    message(WARNING "ASIO still not found after download attempt")
    message(WARNING "Expected at: ${ASIO_HEADER}")
    message(WARNING "WebSocket功能将被禁用")
    message(WARNING "请手动下载或运行下载脚本:")
    if(WIN32)
        message(WARNING "  .\\download_websocket_libs.ps1")
    else()
        message(WARNING "  ./download_websocket_libs.sh")
    endif()
    set(ASIO_INCLUDE_DIR "")
endif()

# ============================================================================
# 检查是否可以启用WebSocket功能
# ============================================================================
if(WEBSOCKETPP_INCLUDE_DIR AND NLOHMANN_JSON_INCLUDE_DIR AND ASIO_INCLUDE_DIR)
    message(STATUS "========================================")
    message(STATUS "WebSocket依赖库已就绪")
    message(STATUS "WebSocket包含路径:")
    message(STATUS "  websocketpp: ${WEBSOCKETPP_INCLUDE_DIR}")
    message(STATUS "  nlohmann/json: ${NLOHMANN_JSON_INCLUDE_DIR}")
    message(STATUS "  ASIO: ${ASIO_INCLUDE_DIR}")
    message(STATUS "========================================")
    message(STATUS "如需启用WebSocket功能:")
    message(STATUS "1. 修改 server/ws_stats_service.h")
    message(STATUS "   将 #define ENABLE_WEBSOCKET 0 改为 1")
    message(STATUS "2. 重新运行 cmake")
    message(STATUS "========================================")
    
    # 设置编译定义，表示WebSocket库可用
    add_definitions(-DWEBSOCKET_LIBS_AVAILABLE=1)
else()
    message(STATUS "========================================")
    message(STATUS "WebSocket依赖库未就绪")
    message(STATUS "WebSocket功能已禁用 (ENABLE_WEBSOCKET=0)")
    message(STATUS "========================================")
    
    # 清空变量，避免添加空路径
    set(WEBSOCKETPP_INCLUDE_DIR "")
    set(NLOHMANN_JSON_INCLUDE_DIR "")
    set(ASIO_INCLUDE_DIR "")
    add_definitions(-DWEBSOCKET_LIBS_AVAILABLE=0)
endif()
# ============================================================================
# 添加包含目录
# ============================================================================
include_directories(
	. 
)

# 条件添加WebSocket库的包含路径
if(WEBSOCKETPP_INCLUDE_DIR)
    include_directories(${WEBSOCKETPP_INCLUDE_DIR})
    message(STATUS "添加websocketpp包含路径: ${WEBSOCKETPP_INCLUDE_DIR}")
endif()

if(NLOHMANN_JSON_INCLUDE_DIR)
    include_directories(${NLOHMANN_JSON_INCLUDE_DIR})
    message(STATUS "添加nlohmann/json包含路径: ${NLOHMANN_JSON_INCLUDE_DIR}")
endif()

if(ASIO_INCLUDE_DIR)
    include_directories(${ASIO_INCLUDE_DIR})
    message(STATUS "添加ASIO包含路径: ${ASIO_INCLUDE_DIR}")
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
			  #  webrtc  timeGetTime  
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
			  #  webrtc  timeGetTime  
			  winmm.lib
			  #ucrt.lib
			  #  libuv.lib  的依赖库
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