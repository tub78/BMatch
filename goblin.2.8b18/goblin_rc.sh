#!/bin/sh
#
# FOR LINUX & OSX / GOBLIN VERSION 2.7.2b4
#

#
setenv SYSTEM 		"`uname -s`"
#
if ("$SYSTEM" == "Darwin") then

    # These environment variables are only inherited by the matlab application
    # when run from a terminal.  Otherwise use: ~/.MacOSX/environment.plist

    # DYLD_LIBRARY_PATH
    if ( $?DYLD_LIBRARY_PATH ) then
        setenv DYLD_LIBRARY_PATH /u/stu/Research/NetworkPrediction/goblin/goblin.2.8b18:$DYLD_LIBRARY_PATH
    else
        setenv DYLD_LIBRARY_PATH /u/stu/Research/NetworkPrediction/goblin/goblin.2.8b18
    endif
    #
    echo " "
    echo DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH
    echo " "
#
else
    # otherwise assume ("$SYSTEM" == "Linux")

    # LD_LIBRARY_PATH
    if ( $?LD_LIBRARY_PATH ) then
        setenv LD_LIBRARY_PATH /home/andrews/Research/NetworkPrediction/goblin/goblin.2.8b18:$LD_LIBRARY_PATH
    else
        setenv LD_LIBRARY_PATH /home/andrews/Research/NetworkPrediction/goblin/goblin.2.8b18
    endif
    #
    echo " "
    echo LD_LIBRARY_PATH=$LD_LIBRARY_PATH
    echo " "
#
endif



##
## LD_PRELOAD
#if ( $?LD_PRELOAD ) then
#    setenv LD_PRELOAD /lib/libgcc_s.so.1:$LD_PRELOAD
#else
#    setenv LD_PRELOAD /lib/libgcc_s.so.1
#endif
#
##
#
#if ( $?LD_PRELOAD ) then
#echo LD_PRELOAD=$LD_PRELOAD
#echo " "
#else
#endif
##

