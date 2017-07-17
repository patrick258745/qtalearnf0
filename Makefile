CC := g++
SRCDIR := src
BINDIR := bin
BUILDDIR := build
QTAF0 := tools/qtaf0
EXECUTABLES := $(BINDIR)/qtamodel $(BINDIR)/mlasampling $(BINDIR)/mlatraining $(BINDIR)/qtatools

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -Wall -std=c++11 -O3
LIB := -L -lm -lpthread -lX11
INC := -I include/ -I lib/

all: ${EXECUTABLES}

$(BINDIR)/qtamodel: $(BUILDDIR)/qtamodel.o $(BUILDDIR)/model.o $(BUILDDIR)/source.o
	@echo " Linking" $@ "... "
	@echo " $(CC) $^ -o $@ $(LIB)"; $(CC) $^ -o $@ $(LIB)
	@echo "copy $@ to $(QTAF0)"; cp $@ $(QTAF0)/qtamodel

$(BINDIR)/mlasampling: $(BUILDDIR)/mlasampling.o $(BUILDDIR)/sampling.o
	@echo " Linking" $@ "... "
	@echo " $(CC) $^ -o $@ $(LIB)"; $(CC) $^ -o $@ $(LIB)

$(BINDIR)/mlatraining: $(BUILDDIR)/mlatraining.o $(BUILDDIR)/training.o
	@echo " Linking" $@ "... "
	@echo " $(CC) $^ -o $@ $(LIB)"; $(CC) $^ -o $@ $(LIB)

$(BINDIR)/qtatools: $(BUILDDIR)/qtatools.o $(BUILDDIR)/tools.o
	@echo " Linking" $@ "... "
	@echo " $(CC) $^ -o $@ $(LIB)"; $(CC) $^ -o $@ $(LIB)
	@echo "copy $@ to $(QTAF0)"; cp $@ $(QTAF0)/qtatools

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(BINDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(BUILDDIR)/source.o: lib/dlib/all/source.cpp
	@echo " $(CC) $(CFLAGS) -c lib/dlib/all/source.cpp -o build/source.o"; $(CC) -std=c++11 -c lib/dlib/all/source.cpp -o build/source.o

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(BINDIR) $(QTAF0)"; $(RM) -r $(BUILDDIR) $(BINDIR) $(QTAF0)/qta*

test: all
	@echo " Testing...";
	bash test/test.sh

corpus: all
	@echo " Processing whole corpus...";
	bash ../training/training.sh

.PHONY: clean test corpus
