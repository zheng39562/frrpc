include ../../make/config.mk

OPTIONAL_CFLAGS=

CLIENT_OBJECTS= \
	./$(OUTPUT_TEMPORARY_PATH)/./echo.pb.o \
	./$(OUTPUT_TEMPORARY_PATH)/./client.o \

SERVER_OBJECTS= \
	./$(OUTPUT_TEMPORARY_PATH)/./echo.pb.o \
	./$(OUTPUT_TEMPORARY_PATH)/./server.o

all : clean proto server client

client : ${CLIENT_OBJECTS}
	${COMMON_CXX} ${COMMON_CFLAGS} ${COMMON_LIBRARY} ${EXAMPLE_LIBRARY} ${EXAMPLE_OBJECTS} ${CLIENT_OBJECTS} -o ./client

server : ${SERVER_OBJECTS}
	${COMMON_CXX} ${COMMON_CFLAGS} ${COMMON_LIBRARY} ${EXAMPLE_LIBRARY} ${EXAMPLE_OBJECTS} ${SERVER_OBJECTS} -o ./server

clean :
	rm -f ./client 
	rm -f ./server
	rm -rf ${OUTPUT_TEMPORARY_PATH}/server.o
	rm -rf ${OUTPUT_TEMPORARY_PATH}/client.o

proto : 
	protoc --proto_path=./ --cpp_out=./ echo.proto	

