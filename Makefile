CC := g++
SRCDIR := src
BINDIR := bin
BUILDDIR := build
EXECUTABLES := $(BINDIR)/qtasearch $(BINDIR)/qtasample $(BINDIR)/qtatrain $(BINDIR)/qtastat
 
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -Wall -std=c++11 -O3
LIB := -L lib -lm
INC := -I include -I lib

all: ${EXECUTABLES}

$(BINDIR)/qtasearch: $(BUILDDIR)/qtasearch.o $(BUILDDIR)/search.o
	@echo " Linking" $@ "... "
	@echo " $(CC) $^ -o $@ $(LIB)"; $(CC) $^ -o $@ $(LIB)

$(BINDIR)/qtasample: $(BUILDDIR)/qtasample.o $(BUILDDIR)/sampling.o
	@echo " Linking" $@ "... "
	@echo " $(CC) $^ -o $@ $(LIB)"; $(CC) $^ -o $@ $(LIB)

$(BINDIR)/qtatrain: $(BUILDDIR)/qtatrain.o $(BUILDDIR)/training.o
	@echo " Linking" $@ "... "
	@echo " $(CC) $^ -o $@ $(LIB)"; $(CC) $^ -o $@ $(LIB)

$(BINDIR)/qtastat: $(BUILDDIR)/qtastat.o $(BUILDDIR)/statistics.o
	@echo " Linking" $@ "... "
	@echo " $(CC) $^ -o $@ $(LIB)"; $(CC) $^ -o $@ $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(BINDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(BINDIR)"; $(RM) -r $(BUILDDIR) $(BINDIR)

test: all
	@echo " Testing...";
	#bash ./learnF0qta.sh test/config.xml

.PHONY: clean test
