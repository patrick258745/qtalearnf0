CC := g++
SRCDIR := src
BINDIR := bin
BUILDDIR := build
EXECUTABLES := $(BINDIR)/qtatarget $(BINDIR)/qtasample $(BINDIR)/qtatrain $(BINDIR)/qtatool
 
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -Wall -std=c++11 -O3
LIB := -L lib -lm
INC := -I include -I lib

all: ${EXECUTABLES}

$(BINDIR)/qtatarget: $(BUILDDIR)/qtatarget.o $(BUILDDIR)/targets.o
	@echo " Linking" $@ "... "
	@echo " $(CC) $^ -o $@ $(LIB)"; $(CC) $^ -o $@ $(LIB)

$(BINDIR)/qtasample: $(BUILDDIR)/qtasample.o $(BUILDDIR)/sampling.o
	@echo " Linking" $@ "... "
	@echo " $(CC) $^ -o $@ $(LIB)"; $(CC) $^ -o $@ $(LIB)

$(BINDIR)/qtatrain: $(BUILDDIR)/qtatrain.o $(BUILDDIR)/training.o
	@echo " Linking" $@ "... "
	@echo " $(CC) $^ -o $@ $(LIB)"; $(CC) $^ -o $@ $(LIB)

$(BINDIR)/qtatool: $(BUILDDIR)/qtatool.o $(BUILDDIR)/tools.o
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
