#!/bin/bash
#
# FOR LINUX & OSX / GOBLIN VERSION 2.8.2b18
#
OSTYPE="`uname -s`"
PWD="`pwd`"
MACHTYPE="`uname -m`"
#
if ( test "$OSTYPE" == "Darwin" ) then
    if ( test "$MACHTYPE" == "i386" ) then
        SYSTEM=darwin-intel-32
    else
        SYSTEM=darwin-ppc-32
    fi

    # These environment variables are only inherited by the matlab application
    # when run from a terminal.  Otherwise use: ~/.MacOSX/environment.plist

    # DYLD_LIBRARY_PATH
    if ( test -z $DYLD_LIBRARY_PATH ) then
        DYLD_LIBRARY_PATH=$PWD/goblin.2.8b18/lib/$SYSTEM:$DYLD_LIBRARY_PATH
    else
        DYLD_LIBRARY_PATH=$PWD/goblin.2.8b18/lib/$SYSTEM
    fi
    #
    export DYLD_LIBRARY_PATH
    #
    echo " "
    echo DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH
    echo " "
#
else
    # otherwise assume ($OSTYPE == Linux)
    if ( test "$MACHTYPE" == "i686" ) then
        SYSTEM=linux-32
    else
        echo "System not supported"
    fi

    # LD_LIBRARY_PATH
    if ( test -z $LD_LIBRARY_PATH ) then
        LD_LIBRARY_PATH=$PWD/goblin.2.8b18/lib/$SYSTEM:$LD_LIBRARY_PATH
    else
        LD_LIBRARY_PATH=$PWD/goblin.2.8b18/lib/$SYSTEM
    fi
    #
    export LD_LIBRARY_PATH
    #
    echo " "
    echo LD_LIBRARY_PATH=$LD_LIBRARY_PATH
    echo " "
#
fi






