#! /bin/bash

FILES="main message_stream test"

for file in ${FILES} ; do
	echo ${file}
	rm -f ../graph/${file}.txt
	graph-easy ./${file}.pl >> ../graph/${file}.txt
done

exit;

