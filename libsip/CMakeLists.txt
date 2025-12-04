project(libsip)
cmake_minimum_required(VERSION 2.8)
MESSAGE(STATUS "#########################")


ADD_DEFINITIONS(-DHAVE_NETDB_H 
		-DHAVE_NETINET_IN_H
		-DHAVE_SYS_IOCTL_H
		-D__POCC__
		-DHAVE_LIMITS_H
		-DHAVE_FCNTL_H
		-DHAVE_FCNTL_O_NONBLOCK
		-DHAVE_STRUCT_TIMEVAL=0
		-DHAVE_PTHREAD
		-DHAVE_SEMAPHORE_H
		-DHAVE_ERRNO_H
		#-DHAVE_RECV
		#-DHAVE_SEND
		#-DRECV_TYPE_RETV
		#-DHAVE_SEND
		-DHAVE_SYS_SOCKET_H
		-DHAVE_CONFIG_H   
		#-DHAVE_SETSOCKOPT_SO_NONBLOCK
		)
 include_directories(
	  
	.
	 
	 c-ares 
	 eXosip2
	 osip2
	 osipparser2
 )

file(GLOB ALL_CXX_SRCS
        ./*.cpp  
		./*.cc
        ./*.h
		./*.c
		
		./c-ares/*.cpp  
		./c-ares/*.cc
        ./c-ares/*.h
		./c-ares/*.c
		
		
		./eXosip2/*.cpp  
		./eXosip2/*.cc
        ./eXosip2/*.h
		./eXosip2/*.c
         
		 
		./osip2/*.cpp  
		./osip2/*.cc
        ./osip2/*.h
		./osip2/*.c
		
		
		./osipparser2/*.cpp  
		./osipparser2/*.cc
        ./osipparser2/*.h
		./osipparser2/*.c
		
		
		./osipparser2/headers/*.cpp  
		./osipparser2/headers/*.cc
        ./osipparser2/headers/*.h
		./osipparser2/headers/*.c
        )

add_library(${PROJECT_NAME}  STATIC  ${ALL_CXX_SRCS})
target_link_libraries(${PROJECT_NAME} )