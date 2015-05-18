#!/bin/bash
for file in `find $1 -name input.magnus`;
do
	sed s/\<\ a\ \;\ /\<\ a,b\;B\ a\ b\=a\ b,A\ b\ a\=a\ b/ <$file >$file.1
	mv $file.1 $file
done
