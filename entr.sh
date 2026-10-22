find . -type f -name "*.c" -or -name "*.s" | \
entr -cs "./build.sh"