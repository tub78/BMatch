#!/bin/sh


find ${PWD}/ ${PWD}/src/ ${PWD}/include/  \( -name "*.[ch]*" -or -name "*.m" \) -maxdepth 1 | ctags -L- --totals

#--tag-relative
#find ./ ./src/ ./include/  \( -name "*.[ch]*" -or -name "*.m" \) -maxdepth 1 
