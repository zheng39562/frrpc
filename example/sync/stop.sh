#! /bin/bash

client_pid=`ps -ef | grep "./client" | awk -F ' ' '{ if($8 == "./client") print $2 }'`
server_pid=`ps -ef | grep "./server" | awk -F ' ' '{ if($8 == "./server") print $2 }'`

echo "client pid "$client_pid
kill -9 $client_pid;
echo "server pid "$server_pid
kill -9 $server_pid;

exit;

