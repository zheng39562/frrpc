COMMON_LIBRARY=-luuid -lpthread -lrt -ldl -lboost_filesystem -lboost_regex -lfrpublic -lprotobuf -lfrnet_epoll
COMMON_SQL_LIBRARY=-lfr_sql -lmysqlcppconn 
COMMON_LIBRARY_PATH=-L/usr/ -L../../build/lib/
COMMON_INCLUDE=-I../../src/
COMMON_MACRO=-D__LINUX -D__FRNET_EPOLL
#COMMON_OPTION=-Wl,--no-as-needed
COMMON_CFLAGS=-g -std=c++11
COMMON_CXX=g++

OPTIONAL_CFLAGS=

EXAMPLE_LIBRARY=-lfrrpc

BUILD_LIBRARY_PATH=../../build/lib
BUILD_INCLUDE_PATH=../../build/include
BUILD_EXECUTE_PATH=../../build/bin
OUTPUT_LIB_PATH=/usr/lib64
OUTPUT_INCLUDE_PATH=/usr/local/include
OUTPUT_EXECUTE_PATH=/usr/local/bin
OUTPUT_TEMPORARY_PATH=../../out/out
OUTPUT_EXAMPLE_PATH=../../out/out

.PHONY: clean all install example

# ------------------------------------------------------------------------------------------------------------------------
# 匹配规则
# --- pb pattern ---  (还为生效，需要进一步研究makefile的匹配规则，期望能够在out中自动生成，而不是独立一个目录)
./${OUTPUT_TEMPORARY_PATH}/../pb/%.o : %.cpp
	-mkdir -p ./${OUTPUT_TEMPORARY_PATH}/$(dir $<)
	$(COMMON_CXX) $(COMMON_CFLAGS) ${OPTIONAL_CFLAGS} $(COMMON_MACRO) -c $(COMMON_INCLUDE) $< -o $@

./${OUTPUT_TEMPORARY_PATH}/../pb/%.o : %.cc
	-mkdir -p ./${OUTPUT_TEMPORARY_PATH}/$(dir $<)
	$(COMMON_CXX) $(COMMON_CFLAGS) ${OPTIONAL_CFLAGS} $(COMMON_MACRO) -c $(COMMON_INCLUDE) $< -o $@

./${OUTPUT_TEMPORARY_PATH}/../pb/%.o : %.c
	-mkdir -p ./${OUTPUT_TEMPORARY_PATH}/$(dir $<)
	$(COMMON_CXX) $(COMMON_CFLAGS) ${OPTIONAL_CFLAGS} $(COMMON_MACRO) -c $(COMMON_INCLUDE) $< -o $@
# --- common pattern --- 
./${OUTPUT_TEMPORARY_PATH}/%.o : %.cpp
	-mkdir -p ./${OUTPUT_TEMPORARY_PATH}/$(dir $<)
	$(COMMON_CXX) $(COMMON_CFLAGS) ${OPTIONAL_CFLAGS} $(COMMON_MACRO) -c $(COMMON_INCLUDE) $< -o $@

./${OUTPUT_TEMPORARY_PATH}/%.o : %.cc
	-mkdir -p ./${OUTPUT_TEMPORARY_PATH}/$(dir $<)
	$(COMMON_CXX) $(COMMON_CFLAGS) ${OPTIONAL_CFLAGS} $(COMMON_MACRO) -c $(COMMON_INCLUDE) $< -o $@

./${OUTPUT_TEMPORARY_PATH}/%.o : %.c
	-mkdir -p ./${OUTPUT_TEMPORARY_PATH}/$(dir $<)
	$(COMMON_CXX) $(COMMON_CFLAGS) ${OPTIONAL_CFLAGS} $(COMMON_MACRO) -c $(COMMON_INCLUDE) $< -o $@
# ------------------------------------------------------------------------------------------------------------------------


FREEDOM_COMMON_IP=127.0.0.1
FREEDOM_WORLD_PORT=12000

# $(1) project_name 
define BuildStopScript
	rm -f ${BUILD_EXECUTE_PATH}/$(strip $(1))_stop.sh
	@echo -e "#! /bin/bash\n" >> ${BUILD_EXECUTE_PATH}/$(strip $(1))_stop.sh
	@echo -e "PID=\`ps -ef | grep \"$(strip $(1))\" | awk -F ' ' '{ if(\$$8 == \"./${strip $(1)}\") print \$$2; }'\` " >> ${BUILD_EXECUTE_PATH}/$(strip $(1))_stop.sh
	@echo -e "if [ \$${PID} > 0 ] ; then" >> ${BUILD_EXECUTE_PATH}/$(strip $(1))_stop.sh
	@echo -e "\techo \"kill \$${PID}\"" >> ${BUILD_EXECUTE_PATH}/$(strip $(1))_stop.sh
	@echo -e "\tkill \$${PID}" >> ${BUILD_EXECUTE_PATH}/$(strip $(1))_stop.sh
	@echo -e "fi\n" >> ${BUILD_EXECUTE_PATH}/$(strip $(1))_stop.sh
	@echo -e "exit\n" >> ${BUILD_EXECUTE_PATH}/$(strip $(1))_stop.sh
	chmod +x ${BUILD_EXECUTE_PATH}/$(strip $(1))_stop.sh
endef

# Build start and stop script
# $(1) project_name
# $(2) ip 
# $(3) port
define BuildExampleScript
	rm -f ${BUILD_EXECUTE_PATH}/$(strip $(1))_start.sh
	@echo -e "#! /bin/bash\n" >> ${BUILD_EXECUTE_PATH}/$(strip $(1))_start.sh
	@echo -e "./$(strip $(1)) $(2) $(3)\n" >> ${BUILD_EXECUTE_PATH}/$(strip $(1))_start.sh
	@echo -e "exit\n" >> ${BUILD_EXECUTE_PATH}/$(strip $(1))_start.sh
	chmod +x ${BUILD_EXECUTE_PATH}/$(strip $(1))_start.sh

	$(call BuildStopScript, $(1))
endef

# $(1) project_name
# $(2) ip 
# $(3) port
define BuildEngineScript
	rm -f ${BUILD_EXECUTE_PATH}/$(strip $(1))_start.sh
	@echo -e "#! /bin/bash\n" >> ${BUILD_EXECUTE_PATH}/$(strip $(1))_start.sh
	@echo -e "./engine $(1) $(2) $(3) & \n" >> ${BUILD_EXECUTE_PATH}/$(strip $(1))_start.sh
	@echo -e "exit\n" >> ${BUILD_EXECUTE_PATH}/$(strip $(1))_start.sh
	chmod +x ${BUILD_EXECUTE_PATH}/$(strip $(1))_start.sh

	$(call BuildStopScript, $(1))
endef

# build protobuf file.
define BuildPBFile
	protoc --proto_path=../pb/ --cpp_out=../pb/ ../pb/frrpc.proto	
	protoc --proto_path=../pb/ --cpp_out=../pb/ ../pb/net.proto	
	protoc --proto_path=../pb/ --cpp_out=../pb/ ../pb/route.proto	
endef



