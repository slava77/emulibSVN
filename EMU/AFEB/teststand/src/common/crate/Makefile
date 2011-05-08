Sources = \
	Camac_73A.cpp \
	Crate.cpp \
	Le32.cpp \
	Storage.cpp \
	Tdc.cpp

IncludeDirs = \
	. \
	../../../../Jorway/include

LibraryDirs = ../../../../Jorway/lib


# Add -I in front of every include directory
IncludePaths = $(IncludeDirs:%=-I%)

# Add -L in front of every library directory
LibraryPaths = $(LibraryDirs:%=-L%)


# Classification of files
CCSources			= $(filter %.cc,	$(Sources))
CppSources			= $(filter %.cpp,	$(Sources))
CxxSources			= $(filter %.cxx,	$(Sources))
CSources			= $(filter %.c,		$(Sources))

Objects =            ${CCSources:.cc=.o} ${CppSources:.cpp=.o} ${CxxSources:.cxx=.o} ${CSources:.c=.o}

StaticLibraries = ../../../../Jorway/lib/libsjy.a

Dependencies =       ${CCSources:.cc=.d} ${CppSources:.cpp=.d} ${CxxSources:.cxx=.d} ${CSources:.c=.d}
TargetDependencies = $(patsubst %.d, $(PackageTargetDir)/%.d, $(Dependencies) )

CC		     = gcc
CXX		     = g++
LD		     = g++
AR		     = ar
NM		     = nm
RANLIB		     = ranlib

CFlags			= -g -O1 -fPIC -DSJY73A
CCFlags			= -g -O1 -Wall -fPIC -DSJY73A

DependencyFlags		= -MM


%.d:%.cc
	$(CXX)  $(CCFlags) ${DependencyFlags} $(IncludePaths)   $< > $@

%.d:%.cpp
	$(CXX)  $(CCFlags) ${DependencyFlags} $(IncludePaths)   $< > $@



%.o:%.cc
	$(CXX)  $(CCFlags) $(IncludePaths) -c -o $@ $<

%.o:%.cpp
	$(CXX)  $(CCFlags) $(IncludePaths) -c -o $@ $<

%.o:%.cxx
	$(CXX)  $(CCFlags) $(IncludePaths) -c -o $@ $<

%.o:%.c
	$(CC)  $(CFlags) $(IncludePaths) -c -o $@ $<

$(Objects): $(Sources)

testStand: $(Objects)

tests:	$(Objects) test_CountVsDac.cpp
	$(CXX)  $(CCFlags) $(IncludePaths) $(Objects) -o test_CountVsDac $(StaticLibraries) test_CountVsDac.cpp

clean:
	rm $(Objects) test_CountVsDac
