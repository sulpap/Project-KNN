CC = g++
# -Wall for all warnings
# -Wextra for even more warnings
CFLAGS = -Wall -O3 -Wextra -g -std=c++17

SRCDIR = src
OBJDIR = build
BINDIR = bin
INCLUDEDIR = include
TESTDIR = tests

SRCFILES = $(wildcard $(SRCDIR)/*.cpp)
TESTFILES = $(wildcard $(TESTDIR)/*.cpp)

# Specific source files for each executable
FIRST_MAIN_SRC = $(SRCDIR)/first_main.cpp
FILTERED_MAIN_SRC = $(SRCDIR)/filtered_main.cpp
STITCHED_MAIN_SRC = $(SRCDIR)/stitched_main.cpp
CALC_GT_SRC = $(SRCDIR)/calculate_groundtruth.cpp
COMMON_SRC = $(filter-out $(FIRST_MAIN_SRC) $(FILTERED_MAIN_SRC) $(STITCHED_MAIN_SRC) $(CALC_GT_SRC), $(SRCFILES))

FIRST_MAIN_OBJ = $(OBJDIR)/first_main.o
FILTERED_MAIN_OBJ = $(OBJDIR)/filtered_main.o
STITCHED_MAIN_OBJ = $(OBJDIR)/stitched_main.o
CALC_GT_OBJ = $(OBJDIR)/calculate_groundtruth.o
COMMON_OBJ = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(COMMON_SRC))

TEST_OBJ = $(patsubst $(TESTDIR)/%.cpp, $(OBJDIR)/%.o, $(TESTFILES))

all: $(BINDIR)/first_main $(BINDIR)/filtered_main $(BINDIR)/stitched_main $(BINDIR)/calculate_groundtruth # test

# Build main executable
$(BINDIR)/first_main: $(FIRST_MAIN_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BINDIR)/filtered_main: $(FILTERED_MAIN_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BINDIR)/stitched_main: $(STITCHED_MAIN_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

# Build calculate_groundtruth executable
$(BINDIR)/calculate_groundtruth: $(CALC_GT_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

# Build test executable
test: $(BINDIR)/test

$(BINDIR)/test: $(TEST_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -c $< -o $@

$(OBJDIR)/%.o: $(TESTDIR)/%.cpp | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -c $< -o $@

# Run calculate_groundtruth after building it
groundtruth: $(BINDIR)/calculate_groundtruth
	@echo "Running calculate_groundtruth..."
	@./$(BINDIR)/calculate_groundtruth

valgrind: $(BINDIR)/test
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(BINDIR)/test

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -fr $(OBJDIR) $(BINDIR)

.PHONY: all clean test valgrind run_calculate_groundtruth
