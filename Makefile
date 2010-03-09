PROJ = City of the Condemned
EXENAME = 7drl

ARCH := $(shell uname -m)
OBJBASE = obj
OBJDIR = $(OBJBASE)/$(ARCH)

CXX = g++
CXXFLAGS = -Wall -O2
COMPILE = $(CXX) $(CXXFLAGS) -c

LIBS = -lncurses
OBJS := $(patsubst %.cc,$(OBJDIR)/%.o,$(wildcard *.cc))

.PHONY: all rebuildall objdir cleanall clean cleanobj cleanbin

all: main

rebuildall: cleanall main

main: objdir $(OBJS)
	$(CXX) -o $(EXENAME) $(OBJS) $(LIBS)

$(OBJDIR)/%.o: %.cc
	$(COMPILE) -o $@ $<

objdir:
	@mkdir -p $(OBJDIR)

cleanall: cleanobj cleanbin

clean: cleanobj

cleanobj:
	rm -rf $(OBJBASE)
	
cleanbin:
	rm -f $(EXENAME)
