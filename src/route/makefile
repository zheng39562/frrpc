include ../../make/config.mk

PROJECT_NAME=route
PROJECT_OBJECTS= \
	./$(OUTPUT_TEMPORARY_PATH)/../pb/net.pb.o \
	./$(OUTPUT_TEMPORARY_PATH)/../pb/route.pb.o \
	./$(OUTPUT_TEMPORARY_PATH)/../public/rpc_pb2js.o \
	./$(OUTPUT_TEMPORARY_PATH)/../public/rpc_heart.o \
	./$(OUTPUT_TEMPORARY_PATH)/../public/rpc_serializable.o \
	./$(OUTPUT_TEMPORARY_PATH)/../route/route.o \
	./$(OUTPUT_TEMPORARY_PATH)/../route/main.o

all : ${PROJECT_OBJECTS}
	mkdir -p ${BUILD_EXECUTE_PATH}
	g++ ${COMMON_CFLAGS} ${COMMON_LIBRARY} ${PROJECT_OBJECTS} ${PUBLIC_COMMON_LIBRARY} -o ${BUILD_EXECUTE_PATH}/${PROJECT_NAME}

install:
	cp ${BUILD_EXECUTE_PATH}/${PROJECT_NAME} ${OUTPUT_EXECUTE_PATH}

clean :
	rm -rf ${OUTPUT_TEMPORARY_PATH}/../*
	rm -f ${BUILD_EXECUTE_PATH}/${PROJECT_NAME}

proto:
	$(call BuildPBFile)

