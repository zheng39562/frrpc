include ../../make/config.mk

PROJECT_NAME=echo_cpp
PROJECT_OBJECTS= \
	./$(OUTPUT_TEMPORARY_PATH)/./echo.pb.o \
	./$(OUTPUT_TEMPORARY_PATH)/./echo_cpp.o

all : ${PROJECT_OBJECTS}
	mkdir -p ${BUILD_EXECUTE_PATH}
	g++ ${COMMON_CFLAGS} ${COMMON_LIBRARY} ${EXAMPLE_OBJECTS} ${PUBLIC_COMMON_LIBRARY} -o ${BUILD_EXECUTE_PATH}/${PROJECT_NAME}
	rm -f ${BUILD_EXECUTE_PATH}/${PROJECT_NAME}_start.sh
	$(call BuildExampleScript, ${PROJECT_NAME}, ${FREEDOM_COMMON_IP}, ${FREEDOM_WORLD_PORT})

clean :
	rm -rf ${OUTPUT_TEMPORARY_PATH}/*.o
	rm -f ${BUILD_EXECUTE_PATH}/${PROJECT_NAME}

proto : 
	protoc --proto_path=./ --cpp_out=./ echo.proto	

