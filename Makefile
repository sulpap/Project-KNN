CC = g++
# -Wall for all warnings
# -Wextra for even more warnings
CFLAGS = -Wall -Wextra -g -std=c++17

SRCDIR = src
OBJDIR = build
BINDIR = bin
INCLUDEDIR = include

SRCFILES = $(wildcard $(SRCDIR)/*.cpp)
OBJFILES = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCFILES))

all: $(BINDIR) $(BINDIR)/main

$(BINDIR)/main: $(OBJFILES) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $(OBJFILES) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -c $< -o $@

# Creating directories
# ---------------------

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# ---- end of creating directories ---------

clean:
	rm -fr $(OBJDIR) $(BINDIR)

.PHONY: all clean