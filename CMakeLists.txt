project(GbServer)
cmake_minimum_required(VERSION 2.8)
MESSAGE(STATUS  "####################################")
MESSAGE(STATUS  "#############${PROJECT_NAME}###############")
MESSAGE(STATUS  "####################################")


set(DIR_NAME_GBServer  ${PROJECT_NAME})

include_directories(
	 ../libwebrtc
	#../../deps/libyuv/include
	 ../deps/abseil-cpp/
	#.
	#../../deps/libsip
	#../../deps/libsip/c-ares 
	#../../deps/libsip/eXosip2
	#../../deps/libsip/osip2
	#../../deps/libsip/osipparser2
	#./utils 
	#../libswagger
	#${DEPS_DIR}/oatpp/include/oatpp-1.4.0
	#${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp
	#${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp-sqlite
	#${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp-swagger
	#${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp-sqlite/sqlite
	#${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp-sqlite
	#${DEPS_DIR}/oatpp/include/oatpp-1.4.0/oatpp-sqlite
	 
)

add_subdirectory(libswagger)
add_subdirectory(GbSipServer)
add_subdirectory(GbMediaServer) 