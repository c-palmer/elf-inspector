find . -type f -name "*.c" \
           -or -name "*.s" \
           -or -name "*.h" \
           -or -name "*.sh" | \
entr -cs "./build.sh $1"