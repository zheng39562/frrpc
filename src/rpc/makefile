include ../../make/config.mk

OPTIONAL_CFLAGS=-fPIC 

PROJECT_NAME=frrpc
PROJECT_OBJECTS= \
	./$(OUTPUT_TEMPORARY_PATH)/../pb/frrpc.pb.o \
	./$(OUTPUT_TEMPORARY_PATH)/../pb/net.pb.o \
	./$(OUTPUT_TEMPORARY_PATH)/../pb/route.pb.o \
	./$(OUTPUT_TEMPORARY_PATH)/../public/rpc_heart.o \
	./$(OUTPUT_TEMPORARY_PATH)/../public/rpc_serializable.o \
	./$(OUTPUT_TEMPORARY_PATH)/../rpc/channel.o \
	./$(OUTPUT_TEMPORARY_PATH)/../rpc/controller.o \
	./$(OUTPUT_TEMPORARY_PATH)/../rpc/frrpc_function.o \
	./$(OUTPUT_TEMPORARY_PATH)/../rpc/frrpc_log.o \
	./$(OUTPUT_TEMPORARY_PATH)/../rpc/net_channel.o \
	./$(OUTPUT_TEMPORARY_PATH)/../rpc/net_server.o \
	./$(OUTPUT_TEMPORARY_PATH)/../rpc/rpc_base_net.o \
	./$(OUTPUT_TEMPORARY_PATH)/../rpc/server.o

all : proto clean ${PROJECT_OBJECTS} 
	mkdir -p ${BUILD_LIBRARY_PATH}
	mkdir -p ${BUILD_INCLUDE_PATH}/${PROJECT_NAME}
	${COMMON_CXX} ${COMMON_CFLAGS} ${COMMON_LIBRARY} ${PROJECT_OBJECTS} -fPIC -shared -o ${BUILD_LIBRARY_PATH}/lib${PROJECT_NAME}.so
	cp -rf *.h ${BUILD_INCLUDE_PATH}/${PROJECT_NAME}

install:
	cp ${BUILD_LIBRARY_PATH}/lib${PROJECT_NAME}.so ${OUTPUT_LIB_PATH}
	cp -rf ${BUILD_INCLUDE_PATH}/${PROJECT_NAME} ${OUTPUT_INCLUDE_PATH}/

clean :
	rm -rf ${OUTPUT_TEMPORARY_PATH}/../*
	rm -f ${BUILD_LIBRARY_PATH}/lib${PROJECT_NAME}.so
	rm -rf ${BUILD_INCLUDE_PATH}/${PROJECT_NAME}

proto : 
	$(call BuildPBFile)
	
