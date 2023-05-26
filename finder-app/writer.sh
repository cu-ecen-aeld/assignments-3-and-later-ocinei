file_to_write=$1
str_to_write=$2

if [ -z "$file_to_write" ]; then
    echo "Error: file_to_write is not a file!"
    exit 1
fi

if [ -z "$str_to_write" ]; then
    echo "Error: str_to_write is not specified."
    exit 1
fi

if [ -e "$file_to_write" ]; then
    rm "$file_to_write"
    echo "$str_to_write" > "$file_to_write"
else
    mkdir -p $(dirname "$file_to_write")
    echo "$str_to_write" > "$file_to_write"
fi
