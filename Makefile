CC := g++
SRCDIR := src
BINDIR := bin
BUILDDIR := build
QTAF0 := tools/qtaf0
EXECUTABLES := $(BINDIR)/qtamodel $(BINDIR)/mlasampling $(BINDIR)/mlatraining $(BINDIR)/qtatools

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -std=c++0x -O3 -DDLIB_NO_GUI_SUPPORT -DDEBUG_MSG
LIB := -L -lm -lpthread
INC := -I include/ -I lib/

all: ${EXECUTABLES}

$(BINDIR)/qtamodel: $(BUILDDIR)/qtamodel.o $(BUILDDIR)/model.o $(BUILDDIR)/source.o
	@echo " Linking" $@ "... "
	@echo " $(CC) $^ -o $@ $(LIB)"; $(CC) $^ -o $@ $(LIB)
	@echo "copy $@ to $(QTAF0)"; cp $@ $(QTAF0)/qtamodel

$(BINDIR)/mlasampling: $(BUILDDIR)/mlasampling.o $(BUILDDIR)/sampling.o
	@echo " Linking" $@ "... "
	@echo " $(CC) $^ -o $@ $(LIB)"; $(CC) $^ -o $@ $(LIB)

$(BINDIR)/mlatraining: $(BUILDDIR)/mlatraining.o $(BUILDDIR)/training.o $(BUILDDIR)/lrr.o $(BUILDDIR)/krr.o $(BUILDDIR)/svr.o $(BUILDDIR)/mlp.o $(BUILDDIR)/source.o
	@echo " Linking" $@ "... "
	@echo " $(CC) $^ -o $@ $(LIB)"; $(CC) $^ -o $@ $(LIB)

$(BINDIR)/qtatools: $(BUILDDIR)/qtatools.o $(BUILDDIR)/tools.o
	@echo " Linking" $@ "... "
	@echo " $(CC) $^ -o $@ $(LIB)"; $(CC) $^ -o $@ $(LIB)
	@echo "copy $@ to $(QTAF0)"; cp $@ $(QTAF0)/qtatools

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(BINDIR)
	@echo " $(CC) $(CFLAGS) -Wall $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(BUILDDIR)/source.o: lib/dlib/all/source.cpp
	@echo " $(CC) $(CFLAGS) lib/dlib/all/source.cpp -c -o build/source.o"; $(CC) $(CFLAGS) lib/dlib/all/source.cpp -c -o build/source.o

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(BINDIR) $(QTAF0)"; $(RM) -r $(BUILDDIR) $(BINDIR) $(QTAF0)/qta*

reset:
	@echo " Cleaning test data";
	@echo " $(RM) -r test/lrr/ "; $(RM) -r test/lrr/test* test/lrr/training*;
	@echo " $(RM) -r test/krr/ "; $(RM) -r test/krr/test* test/krr/training*;
	@echo " $(RM) -r test/svr/ "; $(RM) -r test/svr/test* test/svr/training*;
	@echo " $(RM) -r test/mlp/ "; $(RM) -r test/mlp/test* test/mlp/training*;

test: all
	@echo " Testing...";
	bash test/test.sh

corpus: all
	@echo " Processing whole corpus...";
	bash ../training/training.sh

.PHONY: clean test corpus reset
