#!/bin/sh

echo "Starting file transfer..."

input_file=$1
output_file="/var/tmp/IR_data.txt"

diff ${input_file} ${output_file} > /dev/null
if [ $? -ne 0 ]; then
        echo "Differences found"
        echo "Expected contents to match:"
        cat ${input_file}
        echo "But found contents:"
        cat ${output_file}
        echo "With differences"
        diff -u ${input_file} ${output_file}
        echo "Test complete with failure"
	exit 1
else
	echo "Contents match. Success"
fi

