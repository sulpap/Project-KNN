CC = g++
# -Wall for all warnings
# -Wextra for even more warnings
CFLAGS = -Wall -O3 -Wextra -g -std=c++17 -pthread -fopenmp

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
PARALLEL_FILTERED_MAIN_SRC = $(SRCDIR)/parallel_filtered_main.cpp
PARALLEL_FILTERED_MAIN_PARALLEL_SRC = $(SRCDIR)/parallel_filtered_main_parallel.cpp
PARALLEL_FILTERED_MAIN_PARALLEL_DISTANCES_SRC = $(SRCDIR)/parallel_filtered_main_parallel_distances.cpp

STITCHED_MAIN_SRC = $(SRCDIR)/stitched_main.cpp
STITCHED_MAIN_GRAPH_SRC = $(SRCDIR)/stitched_main_graph.cpp
PARALLEL_STITCHED_MAIN_SRC = $(SRCDIR)/parallel_stitched_main.cpp
PARALLEL_STITCHED_MAIN_PARALLEL_SRC = $(SRCDIR)/parallel_stitched_main_parallel.cpp
PARALLEL_STITCHED_MAIN_MEDOID_PARALLEL_1_SRC = $(SRCDIR)/parallel_stitched_main_medoid_parallel_1.cpp
PARALLEL_STITCHED_MAIN_MEDOID_PARALLEL_2_SRC = $(SRCDIR)/parallel_stitched_main_medoid_parallel_2.cpp
PARALLEL_STITCHED_MAIN_MEDOID_RANDOM_SRC = $(SRCDIR)/parallel_stitched_main_medoid_random.cpp
PARALLEL_STITCHED_MAIN_MEDOID_SUBSET_SRC = $(SRCDIR)/parallel_stitched_main_medoid_subset.cpp
PARALLEL_STITCHED_MAIN_GENERATE_GRAPH_PARALLEL_SRC = $(SRCDIR)/parallel_stitched_main_generate_graph_parallel.cpp

CALC_GT_SRC = $(SRCDIR)/calculate_groundtruth.cpp

COMMON_SRC = $(filter-out $(FIRST_MAIN_SRC) $(FILTERED_MAIN_SRC) $(FILTERED_MAIN_GRAPH_SRC) $(PARALLEL_FILTERED_MAIN_SRC) $(PARALLEL_FILTERED_MAIN_PARALLEL_SRC) $(PARALLEL_FILTERED_MAIN_PARALLEL_DISTANCES_SRC) $(STITCHED_MAIN_SRC) $(STITCHED_MAIN_GRAPH_SRC) $(PARALLEL_STITCHED_MAIN_SRC) $(PARALLEL_STITCHED_MAIN_PARALLEL_SRC) $(PARALLEL_STITCHED_MAIN_MEDOID_PARALLEL_1_SRC) $(PARALLEL_STITCHED_MAIN_MEDOID_PARALLEL_2_SRC) $(PARALLEL_STITCHED_MAIN_MEDOID_RANDOM_SRC) $(PARALLEL_STITCHED_MAIN_MEDOID_SUBSET_SRC) $(PARALLEL_STITCHED_MAIN_GENERATE_GRAPH_PARALLEL_SRC) $(CALC_GT_SRC), $(SRCFILES))

# first (old) main
FIRST_MAIN_OBJ = $(OBJDIR)/first_main.o

# filtered main
FILTERED_MAIN_OBJ = $(OBJDIR)/filtered_main.o
FILTERED_MAIN_GRAPH_OBJ = $(OBJDIR)/filtered_main_graph.o
PARALLEL_FILTERED_MAIN_OBJ = $(OBJDIR)/parallel_filtered_main.o
PARALLEL_FILTERED_MAIN_PARALLEL_OBJ = $(OBJDIR)/parallel_filtered_main_parallel.o
PARALLEL_FILTERED_MAIN_PARALLEL_DISTANCES_OBJ = $(OBJDIR)/parallel_filtered_main_parallel_distances.o

# stitched main
STITCHED_MAIN_OBJ = $(OBJDIR)/stitched_main.o
STITCHED_MAIN_GRAPH_OBJ = $(OBJDIR)/stitched_main_graph.o
PARALLEL_STITCHED_MAIN_OBJ = $(OBJDIR)/parallel_stitched_main.o
PARALLEL_STITCHED_MAIN_PARALLEL_OBJ = $(OBJDIR)/parallel_stitched_main_parallel.o
PARALLEL_STITCHED_MAIN_MEDOID_PARALLEL_1_OBJ = $(OBJDIR)/parallel_stitched_main_medoid_parallel_1.o
PARALLEL_STITCHED_MAIN_MEDOID_PARALLEL_2_OBJ = $(OBJDIR)/parallel_stitched_main_medoid_parallel_2.o
PARALLEL_STITCHED_MAIN_MEDOID_RANDOM_OBJ = $(OBJDIR)/parallel_stitched_main_medoid_random.o
PARALLEL_STITCHED_MAIN_MEDOID_SUBSET_OBJ = $(OBJDIR)/parallel_stitched_main_medoid_subset.o
PARALLEL_STITCHED_MAIN_GENERATE_GRAPH_PARALLEL_OBJ = $(OBJDIR)/parallel_stitched_main_generate_graph_parallel.o

# calculate groundtruth main
CALC_GT_OBJ = $(OBJDIR)/calculate_groundtruth.o

# common (not main)
COMMON_OBJ = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(COMMON_SRC))

TEST_OBJ = $(patsubst $(TESTDIR)/%.cpp, $(OBJDIR)/%.o, $(TESTFILES))

all: $(BINDIR)/first_main $(BINDIR)/filtered_main $(BINDIR)/filtered_main_graph $(BINDIR)/parallel_filtered_main $(BINDIR)/parallel_filtered_main_parallel $(BINDIR)/parallel_filtered_main_parallel_distances $(BINDIR)/stitched_main $(BINDIR)/stitched_main_graph $(BINDIR)/parallel_stitched_main $(BINDIR)/parallel_stitched_main_parallel $(BINDIR)/parallel_stitched_main_medoid_parallel_1 $(BINDIR)/parallel_stitched_main_medoid_parallel_2 $(BINDIR)/parallel_stitched_main_medoid_random $(BINDIR)/parallel_stitched_main_medoid_subset $(BINDIR)/parallel_stitched_main_generate_graph_parallel $(BINDIR)/calculate_groundtruth # test

# ----- Build main executables -----

# first (old) main
$(BINDIR)/first_main: $(FIRST_MAIN_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

# filtered
$(BINDIR)/filtered_main: $(FILTERED_MAIN_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BINDIR)/filtered_main_graph: $(FILTERED_MAIN_GRAPH_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BINDIR)/parallel_filtered_main: $(PARALLEL_FILTERED_MAIN_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BINDIR)/parallel_filtered_main_parallel: $(PARALLEL_FILTERED_MAIN_PARALLEL_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BINDIR)/parallel_filtered_main_parallel_distances: $(PARALLEL_FILTERED_MAIN_PARALLEL_DISTANCES_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

# stitched
$(BINDIR)/stitched_main: $(STITCHED_MAIN_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BINDIR)/stitched_main_graph: $(STITCHED_MAIN_GRAPH_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BINDIR)/parallel_stitched_main: $(PARALLEL_STITCHED_MAIN_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BINDIR)/parallel_stitched_main_parallel: $(PARALLEL_STITCHED_MAIN_PARALLEL_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BINDIR)/parallel_stitched_main_medoid_parallel_1: $(PARALLEL_STITCHED_MAIN_MEDOID_PARALLEL_1_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BINDIR)/parallel_stitched_main_medoid_parallel_2: $(PARALLEL_STITCHED_MAIN_MEDOID_PARALLEL_2_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BINDIR)/parallel_stitched_main_medoid_random: $(PARALLEL_STITCHED_MAIN_MEDOID_RANDOM_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BINDIR)/parallel_stitched_main_medoid_subset: $(PARALLEL_STITCHED_MAIN_MEDOID_SUBSET_OBJ) $(COMMON_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BINDIR)/parallel_stitched_main_generate_graph_parallel: $(PARALLEL_STITCHED_MAIN_GENERATE_GRAPH_PARALLEL_OBJ) $(COMMON_OBJ) | $(BINDIR)
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
