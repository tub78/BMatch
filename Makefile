#
# B-matching Toolbox
# January, 2008
# Stuart Andrews 
#

include Makefile.conf

PWD = $(shell pwd)
VERSION = $(shell test -e README &&  head README | grep "^Version")

SRCDIR = src
INCLUDEDIR = include
BINDIR = bin

# location of executable on your system
BIN = $(BINDIR)/$(SYSTEM)
EXE = $(BIN)/bmatch
MEXEXE = $(BIN)/bmatch_ijw_mex.$(MEXEXTENSION)

# if using precompiled library 2.8b18
LIBDIRGOBLIN=goblin.2.8b18/lib/$(SYSTEM) # GOBLIN include flags
INCLDIRGOBLIN=goblin.2.8b18/include # GOBLIN linker flags

LFLAGS=		-L$(LIBDIRGOBLIN) # linker flags
IFLAGS=		-I$(INCLUDEDIR) -I$(INCLDIRGOBLIN) # include flags


LIBSGOBLIN= -lgoblin # goblin graph library
LIBS=		-lc -lm -lpthread # additional libraries used by cmdline tools
LIBSMEX= 	-lc # additional libraries used by mex files



SRCS = $(wildcard $(SRCDIR)/*.cpp)

MEX_DEPENDS = $(filter-out %/test.cpp %/bmatch.cpp %/bmatch_ijw_mex.cpp,$(SRCS))

CMDLINE_OBJS = $(patsubst %.cpp,%.o, $(MEX_DEPENDS))


#
#
all: intro
	@echo "Running \"make link\" ..."
	@make -s link
	@echo "Running \"make mexlink\" ..."
	@make -s mexlink


.PHONY: intro
intro:
	@echo ""
	@echo "Bmatching Toolbox"
	@echo "$(VERSION)"
	@echo "by Vlad Shchogolev, Bert Huang, and Stuart Andrews"
	@echo ""

clean:
	rm -f *~
	rm -f $(CMDLINE_OBJS) $(SRCDIR)/bmatch.o
	rm -f bmatch $(EXE)
	rm -f matlab_bmatch $(MEXEXE)

.PHONY: sysinfo
sysinfo:
	@echo "System overview:"
	@echo ""
	@echo "          PWD " $(PWD)
	@echo "           OS " $(OS)
	@echo "     MACHTYPE " $(MACHTYPE)
	@echo "       SYSTEM " $(SYSTEM)
	@echo "   MATLABPATH " $(MATLABPATH)
	@echo "    MATLABCMD " $(MATLABCMD)
	@echo "       MEXCMD " $(MEXCMD)
	@echo "    MEXEXTCMD " $(MEXEXTCMD)
	@echo " MEXEXTENSION " $(MEXEXTENSION)
	@echo ""


# link executable for your system
link: checkexe
	@rm -f bmatch;
	@echo "#!/bin/sh" > bmatch ;
	@echo "" >> bmatch ;
	@if [ $(OS) = "darwin" ] ; then 								\
		echo DYLD_LIBRARY_PATH=$(PWD)/$(LIBDIRGOBLIN) >> bmatch ; 	\
		echo export DYLD_LIBRARY_PATH >> bmatch;					\
	else 															\
		echo LD_LIBRARY_PATH=$(PWD)/$(LIBDIRGOBLIN) >> bmatch; 		\
		echo export LD_LIBRARY_PATH >> bmatch; 						\
	fi;
	@echo "" >> bmatch ;
	@echo $(PWD)/bin/$(SYSTEM)/bmatch \$$\* >> bmatch ;
	@echo "" >> bmatch ;
	@chmod 755 bmatch ;
	@echo "link created - ready to use command line tool ""./bmatch"" ";


# link executable for your system
mexlink: checkmatlab checkmexexe
	@rm -f matlab_bmatch;
	@echo "#!/bin/sh" > matlab_bmatch ;
	@echo "" >> matlab_bmatch ;
	@if [ $(OS) = "darwin" ] ; then 									\
		echo DYLD_LIBRARY_PATH=$(PWD)/$(LIBDIRGOBLIN) >> matlab_bmatch ;\
		echo export DYLD_LIBRARY_PATH >> matlab_bmatch; 				\
	else 																\
		echo LD_LIBRARY_PATH=$(PWD)/$(LIBDIRGOBLIN) >> matlab_bmatch;	\
		echo export LD_LIBRARY_PATH >> matlab_bmatch; 					\
	fi;
	@echo "" >> matlab_bmatch ;
	@echo $(MATLABCMD) -r \"addpath $(PWD)\/$(BIN)\; addpath $(PWD)\;\" \$$\* >> matlab_bmatch ;
	@echo "" >> matlab_bmatch ;
	@chmod 755 matlab_bmatch ;
	@echo "link created - ready to use command line tool ""./matlab_bmatch"" ";


# Command line
bmatch:  $(CMDLINE_OBJS) $(SRCDIR)/bmatch.o
	$(LD) $(LFLAGS) $(CMDLINE_OBJS) $(SRCDIR)/bmatch.o -o $(BIN)/bmatch $(LIBSGOBLIN) $(LIBS)
	@make -s link


# MEX compilation
toolbox: checkmex $(SRCDIR)/bmatch_ijw_mex.cpp $(MEX_DEPENDS)
	$(MEXCMD) -outdir $(BIN) $(IFLAGS) $(LFLAGS) $(SRCDIR)/bmatch_ijw_mex.cpp $(MEX_DEPENDS) $(LIBSGOBLIN) $(LIBSMEX)
	@make -s mexlink




%.o: %.cpp
	$(CXX) $(IFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: archive archive2
archive:
	@make -f $(HOME)/Makefile rtilde
	make -s -C ../ -I bmatch/ -f bmatch/Makefile archive2

archive2:
	tar cvfz bmatch.tgz bmatch
	if (! scp bmatch.tgz margin:pub/html/pub ) ; then 					\
		scp bmatch.tgz margin:pub/html/pub;							\
	fi
	if (! scp bmatch/README margin:pub/html/pub/README_bmatch ) ; then \
		scp bmatch/README margin:pub/html/pub/README_bmatch;			\
	fi


.PHONY: checkexe
checkexe:
	@if ! [ -x $(EXE) ] ; then 											\
		echo "";														\
		echo " bmatch executable not found !! ";						\
		echo "    try: make bmatch"; 									\
		echo "";														\
		make -s sysinfo;												\
		exit 1;															\
	fi;

.PHONY: checkmatlab
checkmatlab: 
	@if ! [ -x "$(MATLABCMD)" ] ; then 									\
		echo "";														\
		echo " matlab command not found !!";							\
		echo "    check MATLABPATH in Makefile.conf";					\
		echo "";														\
		make -s sysinfo;												\
		exit 1;															\
	fi


.PHONY: checkmex
checkmex: 
	@if ! [ -x "$(MEXCMD)" ] ; then 									\
		echo "";														\
		echo " mex command not found !! ";								\
		echo "    check MATLABPATH in Makefile.conf"; 					\
		echo "";														\
		make -s sysinfo;												\
		exit 1; 														\
	else																\
		if [ -z "$(MEXEXTENSION)" ] ; then 								\
			echo "";													\
			echo " MEXEXTENSION not initialized !! ";					\
			echo "    please specify MEXEXTENSION in Makefile.conf";	\
			echo "";													\
			make -s sysinfo;											\
			exit 1;														\
		fi																\
	fi


.PHONY: checkmexexe
checkmexexe:
	@if ! [ -x $(MEXEXE) ] ; then 										\
		echo "";														\
		echo " mex executable not found !! ";							\
		echo "    try: make toolbox"; 									\
		echo "    failing that, check MATLABPATH in Makefile.conf"; 	\
		echo "";														\
		exit 1;    														\
	fi;



#
#MEXHELP = $(shell $(MEXCMD) -help|head -10|grep -i "mex-file")
#
#	@if [ -z "$(MEXHELP)" ] ; then 											\
#		echo "Mex not found. Check path to matlab tools in Makefile.conf"; 	\
#		exit;																\
#	fi
#	@if [ -z "$(MEXEXTENSION)" ] ; then 									\
#		echo "Mexext not found. Set appropriate MEXEXTENSION in Makefile.";	\
#		exit;																\
#	fi
#

# Command line
leaktest:  $(CMDLINE_OBJS)
	rm -f $(SRCDIR)/bmatch.o
	$(CXX) $(IFLAGS) $(CXXFLAGS) -D_LEAKTEST_ -c src/test.cpp -o src/bmatch.o
	$(LD) $(LFLAGS) $(CMDLINE_OBJS) $(SRCDIR)/bmatch.o -o $(BIN)/bmatch $(LIBSGOBLIN) $(LIBS)
	@make -s link

#	$(CXX) $(IFLAGS) $(CXXFLAGS) -D_LEAKTEST_ -c src/bmatch.cpp -o src/bmatch.o


depend:
	makedepend -- $(IFLAGS) -- $(SRCS)


# DO NOT DELETE

