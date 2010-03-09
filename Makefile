PROJ = "City of the Condemned"
VERSION_MAJOR = 0
VERSION_REVISION = $(shell git log --pretty=oneline | wc -l)
VERSION_FULL = "\"$(VERSION_MAJOR).$(VERSION_REVISION)\""
EXENAME = 7drl

ARCH := $(shell uname -m)

OBJBASE = obj
OBJDIR = $(OBJBASE)/$(ARCH)

CXX = g++
CXXFLAGS = -Wall -O2 -DVERSION_STRING=$(VERSION_FULL)
COMPILE = $(CXX) $(CXXFLAGS) -c

LIBS = -lncurses
OBJS := $(patsubst %.cc,$(OBJDIR)/%.o,$(wildcard *.cc))

.PHONY: all rebuildall release 7z objdir cleanall clean cleanobj cleanbin

all: main

rebuildall: cleanall main

release: rebuildall

7z: release
	@rm -f $(EXENAME).7z
	@7z a $(EXENAME).7z $(EXENAME)

main: objdir $(OBJS)
	@echo "Linking $(EXENAME)"
	@$(CXX) -o $(EXENAME) $(OBJS) $(LIBS)

$(OBJDIR)/%.o: %.cc
	@echo "Compiling $<"
	@$(COMPILE) -o $@ $<

objdir:
	@mkdir -p $(OBJDIR)

cleanall: cleanobj cleanbin

clean: cleanobj

cleanobj:
	rm -rf $(OBJBASE)
	
cleanbin:
	rm -f $(EXENAME)

