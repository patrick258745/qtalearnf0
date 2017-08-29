CC := g++
SRCDIR := src
BINDIR := bin
BUILDDIR := build
QTAF0 := tools/qtaf0
EXECUTABLES := $(BINDIR)/qtamodel $(BINDIR)/mlasampling $(BINDIR)/mlatraining $(BINDIR)/qtatools

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -std=c++11 -O3 -DDLIB_NO_GUI_SUPPORT
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

praat:
	@echo " Building praat server...";
	bash tools/praat/build-praat.sh

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(BINDIR) $(QTAF0)"; $(RM) -r $(BUILDDIR) $(BINDIR) $(QTAF0)/qta*

reset:
	@echo " Cleaning test data...";
	@echo " $(RM) -r test/qta/ "; $(RM) -r test/qta/audios/ test/qta/plots/ test/qta/qta-* test/qta/qta.*;
	@echo " $(RM) -r test/lrr/ "; $(RM) -r test/lrr/audios/ test/lrr/plots/ test/lrr/lrr-* test/lrr/lrr.measures test/lrr/lrr.target test/lrr/lrr.stat;
		@echo " $(RM) -r test/krr/ "; $(RM) -r test/krr/audios/ test/krr/plots/ test/krr/krr-* test/krr/krr.measures test/krr/krr.target test/krr/krr.stat;
		@echo " $(RM) -r test/svr/ "; $(RM) -r test/svr/audios/ test/svr/plots/ test/svr/svr-* test/svr/svr.measures test/svr/svr.target test/svr/svr.stat;
		@echo " $(RM) -r test/mlp/ "; $(RM) -r test/mlp/audios/ test/mlp/plots/ test/mlp/mlp-* test/mlp/mlp.measures test/mlp/mlp.target test/mlp/mlp.stat;
		$(RM) test/data.sample;

reset2:
	@echo " Cleaning learn data...";
	@echo " $(RM) -r learn/qta/ "; $(RM) -r learn/qta/audios/ learn/qta/plots/ learn/qta/qta-* learn/qta/qta.*;
	@echo " $(RM) -r learn/lrr/ "; $(RM) -r learn/lrr/audios/ learn/lrr/plots/ learn/lrr/lrr-* learn/lrr/lrr.measures learn/lrr/lrr.target learn/lrr/lrr.stat;
		@echo " $(RM) -r learn/krr/ "; $(RM) -r learn/krr/audios/ learn/krr/plots/ learn/krr/krr-* learn/krr/krr.measures learn/krr/krr.target learn/krr/krr.stat;
		@echo " $(RM) -r learn/svr/ "; $(RM) -r learn/svr/audios/ learn/svr/plots/ learn/svr/svr-* learn/svr/svr.measures learn/svr/svr.target learn/svr/svr.stat;
		@echo " $(RM) -r learn/mlp/ "; $(RM) -r learn/mlp/audios/ learn/mlp/plots/ learn/mlp/mlp-* learn/mlp/mlp.measures learn/mlp/mlp.target learn/mlp/mlp.stat;
		$(RM) learn/data.sample;

test: all
	@echo " Testing...";
	bash test/test.sh

learn: all
	@echo " Processing whole corpus...";
	bash learn/learn.sh

job: all
	@echo " Starting new computing job...";
	sbatch job.batch

.PHONY: clean test learn reset reset2 praat job
