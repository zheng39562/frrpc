include ../make/config.mk

PROJECT_NAME=frrpc
PROJECT_OBJECTS= \
	./$(OUTPUT_TEMPORARY_PATH)/./proto_tool.o \
	./$(OUTPUT_TEMPORARY_PATH)/./frrpc.pb.o \
	./$(OUTPUT_TEMPORARY_PATH)/./net_link.o \
	./$(OUTPUT_TEMPORARY_PATH)/./net_server.o \
	./$(OUTPUT_TEMPORARY_PATH)/./net_client.o \
	./$(OUTPUT_TEMPORARY_PATH)/./server.o \
	./$(OUTPUT_TEMPORARY_PATH)/./channel.o \
	./$(OUTPUT_TEMPORARY_PATH)/./closure.o \
	./$(OUTPUT_TEMPORARY_PATH)/./frrpc_function.o

all : ${PROJECT_OBJECTS} proto
	mkdir -p ${BUILD_LIBRARY_PATH}
	mkdir -p ${BUILD_INCLUDE_PATH}/${PROJECT_NAME}
	g++ ${COMMON_CFLAGS} ${COMMON_LIBRARY} ${PROJECT_OBJECTS} ${PUBLIC_COMMON_LIBRARY} -fPIC -shared -o ${BUILD_LIBRARY_PATH}/lib${PROJECT_NAME}.so
	cp -f ${BUILD_LIBRARY_PATH}/lib${PROJECT_NAME}.so ${BUILD_EXECUTE_PATH}/
	cp -rf *.h ${BUILD_INCLUDE_PATH}/${PROJECT_NAME}

install:
	cp ${BUILD_LIBRARY_PATH}/lib${PROJECT_NAME}.so ${OUTPUT_LIB_PATH}
	cp -rf ${BUILD_INCLUDE_PATH}/${PROJECT_NAME} ${OUTPUT_INCLUDE_PATH}/

clean :
	rm -rf ${OUTPUT_TEMPORARY_PATH}/*.o
	rm -f ${BUILD_LIBRARY_PATH}/lib${PROJECT_NAME}.so
	rm -rf ${BUILD_INCLUDE_PATH}/${PROJECT_NAME}

proto : 
	protoc --proto_path=./ --cpp_out=./ frrpc.proto	

