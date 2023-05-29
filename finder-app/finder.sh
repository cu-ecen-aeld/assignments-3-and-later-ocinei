#!/bin/sh
directory_files=$1
str_for_search=$2

if [ -z "$directory_files" ]; then
    echo "directory_files not specified"
    exit 1
fi

if [ -z "$str_for_search" ]; then
    echo "str_for_search is not specified"
    exit 1
fi
if [ ! -d "$directory_files" ]; then
    echo "directory_files does not represent a directory"
    exit 1
fi


num_files=$(find $directory_files -type f | wc -l)
num_lines=0
for file in $(find $directory_files -type f); do
    num_lines=$((num_lines + $(grep -c $str_for_search $file)))
done


echo "The number of files are $num_files and the number of matching lines are $num_lines"
