CC = g++
# -Wall for all warnings
# -Wextra for even more warnings
CFLAGS = -Wall -O3 -Wextra -g -std=c++17 -pthread

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
FILTERED_MAIN_GRAPH_SRC = $(SRCDIR)/filtered_main_graph.cpp

STITCHED_MAIN_SRC = $(SRCDIR)/stitched_main.cpp
STITCHED_MAIN_GRAPH_SRC = $(SRCDIR)/stitched_main_graph.cpp

CALC_GT_SRC = $(SRCDIR)/calculate_groundtruth.cpp

COMMON_SRC = $(filter-out $(FIRST_MAIN_SRC) $(FILTERED_MAIN_SRC) $(FILTERED_MAIN_GRAPH_SRC) $(STITCHED_MAIN_SRC) $(STITCHED_MAIN_GRAPH_SRC) $(CALC_GT_SRC), $(SRCFILES))

# first (old) main
FIRST_MAIN_OBJ = $(OBJDIR)/first_main.o

# filtered main
FILTERED_MAIN_OBJ = $(OBJDIR)/filtered_main.o
FILTERED_MAIN_GRAPH_OBJ = $(OBJDIR)/filtered_main_graph.o

# stitched main
STITCHED_MAIN_OBJ = $(OBJDIR)/stitched_main.o
STITCHED_MAIN_GRAPH_OBJ = $(OBJDIR)/stitched_main_graph.o

# calculate groundtruth main
CALC_GT_OBJ = $(OBJDIR)/calculate_groundtruth.o

# common (not main)
COMMON_OBJ = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(COMMON_SRC))

TEST_OBJ = $(patsubst $(TESTDIR)/%.cpp, $(OBJDIR)/%.o, $(TESTFILES))

all: $(BINDIR)/first_main $(BINDIR)/filtered_main $(BINDIR)/filtered_main_graph $(BINDIR)/stitched_main $(BINDIR)/stitched_main_graph $(BINDIR)/calculate_groundtruth # test

# ----- Build main executables -----

# first (old) main
$(BINDIR)/first_main: $(FIRST_MAIN_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

# filtered
$(BINDIR)/filtered_main: $(FILTERED_MAIN_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BINDIR)/filtered_main_graph: $(FILTERED_MAIN_GRAPH_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

# stitched
$(BINDIR)/stitched_main: $(STITCHED_MAIN_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BINDIR)/stitched_main_graph: $(STITCHED_MAIN_GRAPH_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

# calculate_groundtruth
$(BINDIR)/calculate_groundtruth: $(CALC_GT_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

# test
test: $(BINDIR)/test

$(BINDIR)/test: $(TEST_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -c $< -o $@

$(OBJDIR)/%.o: $(TESTDIR)/%.cpp | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -c $< -o $@

# Build and calculate groundtruth for k = 100
groundtruth: $(BINDIR)/calculate_groundtruth
	./$(BINDIR)/calculate_groundtruth 100

graphs: $(BINDIR)/filtered_main_graph $(BINDIR)/stitched_main_graph
	./$(BINDIR)/filtered_main_graph 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin
	./$(BINDIR)/stitched_main_graph 110 96 1.2 98 ./datasets/smallscale/dummy-data.bin

valgrind: $(BINDIR)/test
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(BINDIR)/test

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -fr $(OBJDIR) $(BINDIR)

.PHONY: all clean test valgrind run_calculate_groundtruth
