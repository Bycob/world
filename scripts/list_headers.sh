for file in $(ls) ;
do
    if test $(echo $file|tail -c 3) = ".h"; then
        echo \#include \"${1}${file}\";
    fi
done
