all: route rpc

install: route_install rpc_install 

clean: route_clean rpc_clean

route:
	cd ./src/route/ && make

route_install:
	cd ./src/route/ && make install

route_clean:
	cd ./src/route/ && make clean

rpc:
	cd ./src/rpc/ && make

rpc_install:
	cd ./src/rpc/ && make install

rpc_clean:
	cd ./src/rpc/ && make clean

# 编译所有的用例(demo和test)
example_all:
	cd ./example/connect && make
	cd ./example/echo_async && make
	cd ./example/echo_sync && make
	cd ./example/tcp_self_echo && make
	
help:
	echo "help doc"

