CC = g++
# -Wall for all warnings
# -Wextra for even more warnings
CFLAGS = -Wall -Wextra -g -std=c++17

SRCDIR = src
OBJDIR = build
BINDIR = bin
INCLUDEDIR = include
TESTDIR = tests

# Modify this line to include both the source and test directories
SRCFILES = $(wildcard $(SRCDIR)/*.cpp)
TESTFILES = $(wildcard $(TESTDIR)/*.cpp)
ALL_TESTFILES = $(SRCFILES) $(TESTFILES)

OBJFILES = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCFILES))
TESTOBJFILES = $(patsubst $(TESTDIR)/%.cpp,$(OBJDIR)/%.o,$(TESTFILES))

OBJFILES_NO_MAIN = $(filter-out $(OBJDIR)/main.o, $(OBJFILES))

all: $(BINDIR) $(BINDIR)/main

# Build main target
$(BINDIR)/main: $(OBJFILES) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $(OBJFILES)

# Compile test executable
test: $(BINDIR)/test

$(BINDIR)/test: $(TESTOBJFILES) $(OBJFILES_NO_MAIN) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $(TESTOBJFILES) $(OBJFILES_NO_MAIN)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -c $< -o $@

$(OBJDIR)/%.o: $(TESTDIR)/%.cpp | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -c $< -o $@

# Create directories
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -fr $(OBJDIR) $(BINDIR)

.PHONY: all clean test
