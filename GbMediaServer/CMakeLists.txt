

project(GbMediaServer)
cmake_minimum_required(VERSION 2.8)
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
        -DOATPP_SWAGGER_RES_PATH="${DEPS_DIR}/oatpp/include/oatpp-1.4.0/bin/oatpp-swagger/res"

        ## SQLite database file
        -DDATABASE_FILE="${CMAKE_CURRENT_SOURCE_DIR}/db.sqlite"
        ## SQLite database test file
        -DTESTDATABASE_FILE="${CMAKE_CURRENT_SOURCE_DIR}/test-db.sqlite"

        ## Path to database migration scripts
        -DDATABASE_MIGRATIONS="${CMAKE_CURRENT_SOURCE_DIR}/sql"
)


 include_directories(
	. 
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
#
#add_subdirectory(curl-7.81.0)
#add_subdirectory(D:/Work/rtc/crtcserver/deps/curl-7.81.0)
#add_subdirectory(libsip)
 
add_executable(${PROJECT_NAME}     ${GbMediaServer_source}  ${Media_source}   )
add_dependencies(${PROJECT_NAME} libmedia_transfer_protocol)
#add_dependencies(${PROJECT_NAME}   libswagger)
# add_dependencies(${PROJECT_NAME} libsip)

set(ALL_LIB 
			#libswagger
			#libswagger
			#libsip 
			Dnsapi.lib   
			delayimp.lib
			ws2_32.lib
			 Qwave.lib
			 crypt32.lib
			iphlpapi.lib
			secur32.lib
			 #
			# oatpp
			# oatpp-sqlite
			# oatpp-swagger
			# sqlite
			crypt32.lib
			iphlpapi.lib
			secur32.lib
			 # absl 
			# abseil_dll
			# Meida_LIB
		#absl_cordz_functions
		#absl_cordz_handle
		#absl_cordz_info
		#absl_cordz_sample_token
		#absl_flags
		#absl_flags_commandlineflag_internal
		#absl_flags_internal
		#absl_flags_marshalling
		#absl_flags_parse
		#absl_flags_private_handle_accessor
		#absl_flags_program_name
		#absl_flags_reflection
		#absl_flags_usage
		#absl_flags_usage_internal
		#absl_low_level_hash
		#absl_random_internal_distribution_test_util
		api 
			audio_device
			audio_processing
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

			  libmedia_transfer_protocol

			  libcrypto
		libssl
				 


				###########################

				 
				 #AAC
				 faad
				 opus
				 libsrtp2
				 x264
		 
		 crypt32.lib
		iphlpapi.lib
       secur32.lib
		 

		libice
		libp2p_peerconnection 

		libmedia_transfer_protocol
		libsrtp2
		libmedia_codec
		#libx264.lib
		x264

		
		#libx264.lib
		#libx264d.lib
	#libx264.dll.lib
	#libx264.lib

	 


	d3d11.lib
      dxgi.lib
	jpeg-static
		

		#curl 
		jsoncpp_static
	       dmoguids.lib
          wmcodecdspuuid.lib
        amstrmid.lib
         msdmo.lib
        oleaut32.lib
		opus
		
libcross_platform_collection_render
	 
		ole32.lib
        oleaut32.lib
        strmiids.lib
        user32.lib
		 
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
		jpeg-static
		

		#curl 
		jsoncpp_static
		#libcurl
		# curl 
		Ws2_32.lib
		Wldap32.lib
		# api
		 

		d3d11.lib
      dxgi.lib
		
 

			Imm32 
			comctl32
			ws2_32.lib
			winmm.lib


			yuv
		libcrypto.lib 
		libssl.lib 
		#
		##libredis.lib
		#libzmalloc.lib
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

		 

		libice
		libp2p_peerconnection 

		libmedia_transfer_protocol
		libsrtp2
		libmedia_codec
		#libx264.lib
		x264

		
		#libx264.lib
		#libx264d.lib
	#libx264.dll.lib
	#libx264.lib

 			
	



	       dmoguids.lib
          wmcodecdspuuid.lib
        amstrmid.lib
         msdmo.lib
        oleaut32.lib
		opus
		 
		libvpx.lib
		libvpx_asm.lib


		 

			)
 
target_link_libraries(${PROJECT_NAME}   ${ALL_LIB} )

#set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /SAFESEH:NO")
			# Console (/SUBSYSTEM:CONSOLE)
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE /SAFESEH:NO")

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