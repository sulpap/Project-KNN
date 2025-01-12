#!/bin/bash

#TEST 1 (filteredVamana serial vs parallel vs parallel distances)

# Program to execute

# Get current time in seconds since the Unix epoch
current_time_start=$(date +%s)

test_1_start=$(date +%s)

PROGRAMS=( \
"./bin/parallel_filtered_main 100 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_filtered_main_parallel 100 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_filtered_main_parallel_distances 100 110 69 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
)
RESULTS_FILE="results.txt"
TEMP_FILE="temp_output.txt"
RUNS=5

#Clear previous results (Only on Test 1)
echo "Test 1 (filteredVamana serial vs parallel vs parallel distances)" > "$RESULTS_FILE"
echo "Running Test 1/12 (filteredVamana serial vs parallel vs parallel distances)"

# Initialize variables
base_program_avg_total_recall_all=0
base_program_avg_average_recall_all=0
base_program_avg_total_recall_filtered=0
base_program_avg_average_recall_filtered=0
base_program_avg_total_recall_unfiltered=0
base_program_avg_average_recall_unfiltered=0
base_program_avg_index_time=0
base_program_avg_total_query_time_all=0
base_program_avg_average_FGS_time_all=0
base_program_avg_average_FGS_time_filtered=0
base_program_avg_average_FGS_time_unfiltered=0
base_program_avg_runtime=0

program_avg_total_recalls_all=()
program_avg_average_recalls_all=()
program_avg_total_recalls_filtered=()
program_avg_average_recalls_filtered=()
program_avg_total_recalls_unfiltered=()
program_avg_average_recalls_unfiltered=()
program_avg_index_times=()
program_avg_total_query_times_all=()
program_avg_average_FGS_times_all=()
program_avg_average_FGS_times_filtered=()
program_avg_average_FGS_times_unfiltered=()
program_avg_runtimes=()


echo "Run Results:" >> "$RESULTS_FILE"

echo >> "$RESULTS_FILE"

echo "Programs are:" >> "$RESULTS_FILE"

for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"
    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))): $PROGRAM" >> "$RESULTS_FILE"
    else 
        echo "Program $((p+1)): $PROGRAM" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"

# Run each program
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"

    program_total_recall_all_sum=0
    program_average_recall_all_sum=0
    program_total_recall_filtered_sum=0
    program_average_recall_filtered_sum=0
    program_total_recall_unfiltered_sum=0
    program_average_recall_unfiltered_sum=0
    program_index_time_sum=0
    program_total_query_time_all_sum=0
    program_average_FGS_time_all_sum=0
    program_average_FGS_time_filtered_sum=0
    program_average_FGS_time_unfiltered_sum=0
    program_runtime_sum=0

    echo "Running Program $((p+1))/${#PROGRAMS[@]}: $PROGRAM" >> "$RESULTS_FILE"
    echo "Running Program $((p+1))/${#PROGRAMS[@]}..."

    for ((i=1; i<=RUNS; i++)); do
        echo "Running the program (Run $i/$RUNS)..."

        # Run the program and capture output
        $PROGRAM > "$TEMP_FILE"

        # Extract recalls and timing summary using grep and awk
        total_recall_all=$(awk '/Total recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_all=$(awk '/Average recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_filtered=$(awk '/Total recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_filtered=$(awk '/Average recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_unfiltered=$(awk '/Total recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_unfiltered=$(awk '/Average recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        index_time=$(awk '/Index build time/ {print $(NF-4)}' "$TEMP_FILE")
        total_query_time_all=$(awk '/Total time Query calculation took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_all=$(awk '/Average time FilteredGreadySearch took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_filtered=$(awk '/Average time FilteredGreadySearch took for FILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_unfiltered=$(awk '/Average time FilteredGreadySearch took for UNFILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        runtime=$(awk '/Program ran for/ {print $4}' "$TEMP_FILE")

        # Append run results to file
        echo "Run $i:" >> "$RESULTS_FILE"
        echo "Total Recall (ALL): $total_recall_all%" >> "$RESULTS_FILE"
        echo "Average Recall (ALL): $average_recall_all%" >> "$RESULTS_FILE"
        echo "Total Recall (Filtered): $total_recall_filtered%" >> "$RESULTS_FILE"
        echo "Average Recall (Filtered): $average_recall_filtered%" >> "$RESULTS_FILE"
        echo "Total Recall (Unfiltered): $total_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Average Recall (Unfiltered): $average_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Index Time: $index_time seconds" >> "$RESULTS_FILE"
        echo "Total Query Time (ALL): $total_query_time_all seconds" >> "$RESULTS_FILE" 
        echo "Average FGS Time (ALL): $average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Filtered): $average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Unfiltered): $average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Program Runtime: $runtime seconds" >> "$RESULTS_FILE"
        echo >> "$RESULTS_FILE"

        # Accumulate values
        program_total_recall_all_sum=$(echo "scale=12; $program_total_recall_all_sum + $total_recall_all" | bc)
        program_average_recall_all_sum=$(echo "scale=12; $program_average_recall_all_sum + $average_recall_all" | bc)
        program_total_recall_filtered_sum=$(echo "scale=12; $program_total_recall_filtered_sum + $total_recall_filtered" | bc)
        program_average_recall_filtered_sum=$(echo "scale=12; $program_average_recall_filtered_sum + $average_recall_filtered" | bc)
        program_total_recall_unfiltered_sum=$(echo "scale=12; $program_total_recall_unfiltered_sum + $total_recall_unfiltered" | bc)
        program_average_recall_unfiltered_sum=$(echo "scale=12; $program_average_recall_unfiltered_sum + $average_recall_unfiltered" | bc)
        program_index_time_sum=$(echo "scale=12; $program_index_time_sum + $index_time" | bc)
        program_total_query_time_all_sum=$(echo "scale=12; $program_total_query_time_all_sum + $total_query_time_all" | bc)
        program_average_FGS_time_all_sum=$(echo "scale=12; $program_average_FGS_time_all_sum + $average_FGS_time_all" | bc)
        program_average_FGS_time_filtered_sum=$(echo "scale=12; $program_average_FGS_time_filtered_sum + $average_FGS_time_filtered" | bc)
        program_average_FGS_time_unfiltered_sum=$(echo "scale=12; $program_average_FGS_time_unfiltered_sum + $average_FGS_time_unfiltered" | bc)
        program_runtime_sum=$(echo "scale=12; $program_runtime_sum + $runtime" | bc)
    done


    # Calculate average
    average_program_total_recall_all=$(echo "scale=10; $program_total_recall_all_sum / $RUNS" | bc)
    average_program_average_recall_all=$(echo "scale=10; $program_average_recall_all_sum / $RUNS" | bc)
    average_program_total_recall_filtered=$(echo "scale=10; $program_total_recall_filtered_sum / $RUNS" | bc)
    average_program_average_recall_filtered=$(echo "scale=10; $program_average_recall_filtered_sum / $RUNS" | bc)
    average_program_total_recall_unfiltered=$(echo "scale=10; $program_total_recall_unfiltered_sum / $RUNS" | bc)
    average_program_average_recall_unfiltered=$(echo "scale=10; $program_average_recall_unfiltered_sum / $RUNS" | bc)
    average_program_index_time=$(echo "scale=10; $program_index_time_sum / $RUNS" | bc)
    average_program_total_query_time_all=$(echo "scale=10; $program_total_query_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_all=$(echo "scale=10; $program_average_FGS_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_filtered=$(echo "scale=10; $program_average_FGS_time_filtered_sum / $RUNS" | bc)
    average_program_average_FGS_time_unfiltered=$(echo "scale=10; $program_average_FGS_time_unfiltered_sum / $RUNS" | bc)
    average_program_runtime=$(echo "scale=10; $program_runtime_sum / $RUNS" | bc)

    program_avg_total_recalls_all+=("$average_program_total_recall_all")
    program_avg_average_recalls_all+=("$average_program_average_recall_all")
    program_avg_total_recalls_filtered+=("$average_program_total_recall_filtered")
    program_avg_average_recalls_filtered+=("$average_program_average_recall_filtered")
    program_avg_total_recalls_unfiltered+=("$average_program_total_recall_unfiltered")
    program_avg_average_recalls_unfiltered+=("$average_program_average_recall_unfiltered")
    program_avg_index_times+=("$average_program_index_time")
    program_avg_total_query_times_all+=("$average_program_total_query_time_all")
    program_avg_average_FGS_times_all+=("$average_program_average_FGS_time_all")
    program_avg_average_FGS_times_filtered+=("$average_program_average_FGS_time_filtered")
    program_avg_average_FGS_times_unfiltered+=("$average_program_average_FGS_time_unfiltered")
    program_avg_runtimes+=("$average_program_runtime")

    # Save base program runtime
    if [ $p -eq 0 ]; then
        base_program_avg_total_recall_all=$average_program_total_recall_all
        base_program_avg_average_recall_all=$average_program_average_recall_all
        base_program_avg_total_recall_filtered=$average_program_total_recall_filtered
        base_program_avg_average_recall_filtered=$average_program_average_recall_filtered
        base_program_avg_total_recall_unfiltered=$average_program_total_recall_unfiltered
        base_program_avg_average_recall_unfiltered=$average_program_average_recall_unfiltered
        base_program_avg_index_time=$average_program_index_time
        base_program_avg_total_query_time_all=$average_program_total_query_time_all
        base_program_avg_average_FGS_time_all=$average_program_average_FGS_time_all
        base_program_avg_average_FGS_time_filtered=$average_program_average_FGS_time_filtered
        base_program_avg_average_FGS_time_unfiltered=$average_program_average_FGS_time_unfiltered
        base_program_avg_runtime=$average_program_runtime
    fi


    # Append average runtime to results file
    echo "Average Results for Program $((p+1)):" >> "$RESULTS_FILE"
    echo "Average Total Recall (ALL): $average_program_total_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (ALL): $average_program_average_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (FILTERED): $average_program_total_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (FILTERED): $average_program_average_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (UNFILTERED): $average_program_total_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (UNFILTERED): $average_program_average_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Index Time: $average_program_index_time seconds" >> "$RESULTS_FILE"
    echo "Average Total Query Time (ALL): $average_program_total_query_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (ALL): $average_program_average_FGS_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (FILTERED): $average_program_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (UNFILTERED): $average_program_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Runtime: $average_program_runtime seconds" >> "$RESULTS_FILE"
    echo >> "$RESULTS_FILE"
done

# Calculate percentage improvement
echo "Performance Comparison:" >> "$RESULTS_FILE"
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    average_total_recall_all="${program_avg_total_recalls_all[p]}"
    average_average_recall_all="${program_avg_average_recalls_all[p]}"
    average_total_recall_filtered="${program_avg_total_recalls_filtered[p]}"
    average_average_recall_filtered="${program_avg_average_recalls_filtered[p]}"
    average_total_recall_unfiltered="${program_avg_total_recalls_unfiltered[p]}"
    average_average_recall_unfiltered="${program_avg_average_recalls_unfiltered[p]}"
    average_index_time="${program_avg_index_times[p]}"
    average_total_query_time_all="${program_avg_total_query_times_all[p]}"
    average_average_FGS_time_all="${program_avg_average_FGS_times_all[p]}"
    average_average_FGS_time_filtered="${program_avg_average_FGS_times_filtered[p]}"
    average_average_FGS_time_unfiltered="${program_avg_average_FGS_times_unfiltered[p]}"
    average_runtime="${program_avg_runtimes[p]}"

    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))):" >> "$RESULTS_FILE"
        echo "Average Total Recall (ALL): $average_total_recall_all %" >> "$RESULTS_FILE"
        echo "Average Average Recall (ALL): $average_average_recall_all %" >> "$RESULTS_FILE"
        echo "Average Total Recall (FILTERED): $average_total_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (FILTERED): $average_average_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Total Recall (UNFILTERED): $average_total_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (UNFILTERED): $average_average_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Index Time: $average_index_time seconds" >> "$RESULTS_FILE"
        echo "Average Total Query Time (ALL): $average_total_query_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (ALL): $average_average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Average Runtime: $average_runtime seconds" >> "$RESULTS_FILE"
    else
        echo >> "$RESULTS_FILE"
        echo "Program $((p+1)):" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_all / $base_program_avg_total_recall_all) * 100" | bc)
        echo "$improvement% better Average Total Recall (ALL) than the Base Program with Average Total Recall (ALL): $average_total_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_all / $base_program_avg_average_recall_all) * 100" | bc)
        echo "$improvement% better Average Average Recall (ALL) than the Base Program with Average Average Recall (ALL): $average_average_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_filtered / $base_program_avg_total_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Total Recall (FILTERED) than the Base Program with Average Total Recall (FILTERED): $average_total_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_filtered / $base_program_avg_average_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Average Recall (FILTERED) than the Base Program with Average Average Recall (FILTERED): $average_average_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_unfiltered / $base_program_avg_total_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Total Recall (UNFILTERED) than the Base Program with Average Total Recall (UNFILTERED): $average_total_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_unfiltered / $base_program_avg_average_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average Recall (UNFILTERED) than the Base Program with Average Average Recall (UNFILTERED): $average_average_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_index_time / $base_program_avg_index_time) * 100" | bc)
        echo "$improvement% faster Average Index Time than the Base Program with Average Index Time: $average_index_time" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_query_time_all / $base_program_avg_total_query_time_all) * 100" | bc)
        echo "$improvement% better Average Total Query Time (ALL) than the Base Program with Average Total Query Time (ALL): $average_total_query_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_all / $base_program_avg_average_FGS_time_all) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (ALL) than the Base Program with Average Average FGS Time (ALL): $average_average_FGS_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_filtered / $base_program_avg_average_FGS_time_filtered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (FILTERED) than the Base Program with Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_unfiltered / $base_program_avg_average_FGS_time_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (UNFILTERED) than the Base Program with Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_runtime / $base_program_avg_runtime) * 100" | bc)
        echo "$improvement% faster Average Runtime than the Base Program with Average Runtime: $average_runtime" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"
echo >> "$RESULTS_FILE"
test_1_finish=$(date +%s)
test_1_time_diff=$(echo "$test_1_finish-test_1_start" | bc)
echo "Test 1 took $test_1_time_diff seconds."
echo

#TEST 2 (stitchedVamana serial vs parallel vs medoid_parallel_1 vs medoid_parallel_2 vs medoid_random vs medoid_subset vs generate_graph_parallel)

test_2_start=$(date +%s)

# Program to execute
PROGRAMS=( \
"./bin/parallel_stitched_main 100 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main_parallel 100 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main_medoid_parallel_1 100 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main_medoid_parallel_2 100 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main_medoid_random 100 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main_medoid_subset 100 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main_generate_graph_parallel 100 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
)
RESULTS_FILE="results.txt"
TEMP_FILE="temp_output.txt"
RUNS=5

echo "Test 2 (stitchedVamana serial vs parallel vs medoid_parallel_1 vs medoid_parallel_2 vs medoid_random vs medoid_subset vs generate_graph_parallel)" >> "$RESULTS_FILE"
echo "Running Test 2/12 (stitchedVamana serial vs parallel vs medoid_parallel_1 vs medoid_parallel_2 vs medoid_random vs medoid_subset vs generate_graph_parallel)"

# Initialize variables
base_program_avg_total_recall_all=0
base_program_avg_average_recall_all=0
base_program_avg_total_recall_filtered=0
base_program_avg_average_recall_filtered=0
base_program_avg_total_recall_unfiltered=0
base_program_avg_average_recall_unfiltered=0
base_program_avg_index_time=0
base_program_avg_total_query_time_all=0
base_program_avg_average_FGS_time_all=0
base_program_avg_average_FGS_time_filtered=0
base_program_avg_average_FGS_time_unfiltered=0
base_program_avg_runtime=0

program_avg_total_recalls_all=()
program_avg_average_recalls_all=()
program_avg_total_recalls_filtered=()
program_avg_average_recalls_filtered=()
program_avg_total_recalls_unfiltered=()
program_avg_average_recalls_unfiltered=()
program_avg_index_times=()
program_avg_total_query_times_all=()
program_avg_average_FGS_times_all=()
program_avg_average_FGS_times_filtered=()
program_avg_average_FGS_times_unfiltered=()
program_avg_runtimes=()


echo "Run Results:" >> "$RESULTS_FILE"

echo >> "$RESULTS_FILE"

echo "Programs are:" >> "$RESULTS_FILE"

for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"
    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))): $PROGRAM" >> "$RESULTS_FILE"
    else 
        echo "Program $((p+1)): $PROGRAM" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"

# Run each program
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"

    program_total_recall_all_sum=0
    program_average_recall_all_sum=0
    program_total_recall_filtered_sum=0
    program_average_recall_filtered_sum=0
    program_total_recall_unfiltered_sum=0
    program_average_recall_unfiltered_sum=0
    program_index_time_sum=0
    program_total_query_time_all_sum=0
    program_average_FGS_time_all_sum=0
    program_average_FGS_time_filtered_sum=0
    program_average_FGS_time_unfiltered_sum=0
    program_runtime_sum=0

    echo "Running Program $((p+1))/${#PROGRAMS[@]}: $PROGRAM" >> "$RESULTS_FILE"
    echo "Running Program $((p+1))/${#PROGRAMS[@]}..."

    for ((i=1; i<=RUNS; i++)); do
        echo "Running the program (Run $i/$RUNS)..."

        # Run the program and capture output
        $PROGRAM > "$TEMP_FILE"

        # Extract recalls and timing summary using grep and awk
        total_recall_all=$(awk '/Total recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_all=$(awk '/Average recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_filtered=$(awk '/Total recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_filtered=$(awk '/Average recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_unfiltered=$(awk '/Total recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_unfiltered=$(awk '/Average recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        index_time=$(awk '/Index build time/ {print $(NF-4)}' "$TEMP_FILE")
        total_query_time_all=$(awk '/Total time Query calculation took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_all=$(awk '/Average time FilteredGreadySearch took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_filtered=$(awk '/Average time FilteredGreadySearch took for FILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_unfiltered=$(awk '/Average time FilteredGreadySearch took for UNFILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        runtime=$(awk '/Program ran for/ {print $4}' "$TEMP_FILE")

        # Append run results to file
        echo "Run $i:" >> "$RESULTS_FILE"
        echo "Total Recall (ALL): $total_recall_all%" >> "$RESULTS_FILE"
        echo "Average Recall (ALL): $average_recall_all%" >> "$RESULTS_FILE"
        echo "Total Recall (Filtered): $total_recall_filtered%" >> "$RESULTS_FILE"
        echo "Average Recall (Filtered): $average_recall_filtered%" >> "$RESULTS_FILE"
        echo "Total Recall (Unfiltered): $total_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Average Recall (Unfiltered): $average_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Index Time: $index_time seconds" >> "$RESULTS_FILE"
        echo "Total Query Time (ALL): $total_query_time_all seconds" >> "$RESULTS_FILE" 
        echo "Average FGS Time (ALL): $average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Filtered): $average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Unfiltered): $average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Program Runtime: $runtime seconds" >> "$RESULTS_FILE"
        echo >> "$RESULTS_FILE"

        # Accumulate values
        program_total_recall_all_sum=$(echo "scale=12; $program_total_recall_all_sum + $total_recall_all" | bc)
        program_average_recall_all_sum=$(echo "scale=12; $program_average_recall_all_sum + $average_recall_all" | bc)
        program_total_recall_filtered_sum=$(echo "scale=12; $program_total_recall_filtered_sum + $total_recall_filtered" | bc)
        program_average_recall_filtered_sum=$(echo "scale=12; $program_average_recall_filtered_sum + $average_recall_filtered" | bc)
        program_total_recall_unfiltered_sum=$(echo "scale=12; $program_total_recall_unfiltered_sum + $total_recall_unfiltered" | bc)
        program_average_recall_unfiltered_sum=$(echo "scale=12; $program_average_recall_unfiltered_sum + $average_recall_unfiltered" | bc)
        program_index_time_sum=$(echo "scale=12; $program_index_time_sum + $index_time" | bc)
        program_total_query_time_all_sum=$(echo "scale=12; $program_total_query_time_all_sum + $total_query_time_all" | bc)
        program_average_FGS_time_all_sum=$(echo "scale=12; $program_average_FGS_time_all_sum + $average_FGS_time_all" | bc)
        program_average_FGS_time_filtered_sum=$(echo "scale=12; $program_average_FGS_time_filtered_sum + $average_FGS_time_filtered" | bc)
        program_average_FGS_time_unfiltered_sum=$(echo "scale=12; $program_average_FGS_time_unfiltered_sum + $average_FGS_time_unfiltered" | bc)
        program_runtime_sum=$(echo "scale=12; $program_runtime_sum + $runtime" | bc)
    done


    # Calculate average
    average_program_total_recall_all=$(echo "scale=10; $program_total_recall_all_sum / $RUNS" | bc)
    average_program_average_recall_all=$(echo "scale=10; $program_average_recall_all_sum / $RUNS" | bc)
    average_program_total_recall_filtered=$(echo "scale=10; $program_total_recall_filtered_sum / $RUNS" | bc)
    average_program_average_recall_filtered=$(echo "scale=10; $program_average_recall_filtered_sum / $RUNS" | bc)
    average_program_total_recall_unfiltered=$(echo "scale=10; $program_total_recall_unfiltered_sum / $RUNS" | bc)
    average_program_average_recall_unfiltered=$(echo "scale=10; $program_average_recall_unfiltered_sum / $RUNS" | bc)
    average_program_index_time=$(echo "scale=10; $program_index_time_sum / $RUNS" | bc)
    average_program_total_query_time_all=$(echo "scale=10; $program_total_query_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_all=$(echo "scale=10; $program_average_FGS_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_filtered=$(echo "scale=10; $program_average_FGS_time_filtered_sum / $RUNS" | bc)
    average_program_average_FGS_time_unfiltered=$(echo "scale=10; $program_average_FGS_time_unfiltered_sum / $RUNS" | bc)
    average_program_runtime=$(echo "scale=10; $program_runtime_sum / $RUNS" | bc)

    program_avg_total_recalls_all+=("$average_program_total_recall_all")
    program_avg_average_recalls_all+=("$average_program_average_recall_all")
    program_avg_total_recalls_filtered+=("$average_program_total_recall_filtered")
    program_avg_average_recalls_filtered+=("$average_program_average_recall_filtered")
    program_avg_total_recalls_unfiltered+=("$average_program_total_recall_unfiltered")
    program_avg_average_recalls_unfiltered+=("$average_program_average_recall_unfiltered")
    program_avg_index_times+=("$average_program_index_time")
    program_avg_total_query_times_all+=("$average_program_total_query_time_all")
    program_avg_average_FGS_times_all+=("$average_program_average_FGS_time_all")
    program_avg_average_FGS_times_filtered+=("$average_program_average_FGS_time_filtered")
    program_avg_average_FGS_times_unfiltered+=("$average_program_average_FGS_time_unfiltered")
    program_avg_runtimes+=("$average_program_runtime")

    # Save base program runtime
    if [ $p -eq 0 ]; then
        base_program_avg_total_recall_all=$average_program_total_recall_all
        base_program_avg_average_recall_all=$average_program_average_recall_all
        base_program_avg_total_recall_filtered=$average_program_total_recall_filtered
        base_program_avg_average_recall_filtered=$average_program_average_recall_filtered
        base_program_avg_total_recall_unfiltered=$average_program_total_recall_unfiltered
        base_program_avg_average_recall_unfiltered=$average_program_average_recall_unfiltered
        base_program_avg_index_time=$average_program_index_time
        base_program_avg_total_query_time_all=$average_program_total_query_time_all
        base_program_avg_average_FGS_time_all=$average_program_average_FGS_time_all
        base_program_avg_average_FGS_time_filtered=$average_program_average_FGS_time_filtered
        base_program_avg_average_FGS_time_unfiltered=$average_program_average_FGS_time_unfiltered
        base_program_avg_runtime=$average_program_runtime
    fi


    # Append average runtime to results file
    echo "Average Results for Program $((p+1)):" >> "$RESULTS_FILE"
    echo "Average Total Recall (ALL): $average_program_total_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (ALL): $average_program_average_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (FILTERED): $average_program_total_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (FILTERED): $average_program_average_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (UNFILTERED): $average_program_total_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (UNFILTERED): $average_program_average_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Index Time: $average_program_index_time seconds" >> "$RESULTS_FILE"
    echo "Average Total Query Time (ALL): $average_program_total_query_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (ALL): $average_program_average_FGS_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (FILTERED): $average_program_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (UNFILTERED): $average_program_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Runtime: $average_program_runtime seconds" >> "$RESULTS_FILE"
    echo >> "$RESULTS_FILE"
done

# Calculate percentage improvement
echo "Performance Comparison:" >> "$RESULTS_FILE"
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    average_total_recall_all="${program_avg_total_recalls_all[p]}"
    average_average_recall_all="${program_avg_average_recalls_all[p]}"
    average_total_recall_filtered="${program_avg_total_recalls_filtered[p]}"
    average_average_recall_filtered="${program_avg_average_recalls_filtered[p]}"
    average_total_recall_unfiltered="${program_avg_total_recalls_unfiltered[p]}"
    average_average_recall_unfiltered="${program_avg_average_recalls_unfiltered[p]}"
    average_index_time="${program_avg_index_times[p]}"
    average_total_query_time_all="${program_avg_total_query_times_all[p]}"
    average_average_FGS_time_all="${program_avg_average_FGS_times_all[p]}"
    average_average_FGS_time_filtered="${program_avg_average_FGS_times_filtered[p]}"
    average_average_FGS_time_unfiltered="${program_avg_average_FGS_times_unfiltered[p]}"
    average_runtime="${program_avg_runtimes[p]}"

    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))):" >> "$RESULTS_FILE"
        echo "Average Total Recall (ALL): $average_total_recall_all %" >> "$RESULTS_FILE"
        echo "Average Average Recall (ALL): $average_average_recall_all %" >> "$RESULTS_FILE"
        echo "Average Total Recall (FILTERED): $average_total_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (FILTERED): $average_average_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Total Recall (UNFILTERED): $average_total_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (UNFILTERED): $average_average_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Index Time: $average_index_time seconds" >> "$RESULTS_FILE"
        echo "Average Total Query Time (ALL): $average_total_query_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (ALL): $average_average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Average Runtime: $average_runtime seconds" >> "$RESULTS_FILE"
    else
        echo >> "$RESULTS_FILE"
        echo "Program $((p+1)):" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_all / $base_program_avg_total_recall_all) * 100" | bc)
        echo "$improvement% better Average Total Recall (ALL) than the Base Program with Average Total Recall (ALL): $average_total_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_all / $base_program_avg_average_recall_all) * 100" | bc)
        echo "$improvement% better Average Average Recall (ALL) than the Base Program with Average Average Recall (ALL): $average_average_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_filtered / $base_program_avg_total_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Total Recall (FILTERED) than the Base Program with Average Total Recall (FILTERED): $average_total_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_filtered / $base_program_avg_average_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Average Recall (FILTERED) than the Base Program with Average Average Recall (FILTERED): $average_average_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_unfiltered / $base_program_avg_total_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Total Recall (UNFILTERED) than the Base Program with Average Total Recall (UNFILTERED): $average_total_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_unfiltered / $base_program_avg_average_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average Recall (UNFILTERED) than the Base Program with Average Average Recall (UNFILTERED): $average_average_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_index_time / $base_program_avg_index_time) * 100" | bc)
        echo "$improvement% faster Average Index Time than the Base Program with Average Index Time: $average_index_time" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_query_time_all / $base_program_avg_total_query_time_all) * 100" | bc)
        echo "$improvement% better Average Total Query Time (ALL) than the Base Program with Average Total Query Time (ALL): $average_total_query_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_all / $base_program_avg_average_FGS_time_all) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (ALL) than the Base Program with Average Average FGS Time (ALL): $average_average_FGS_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_filtered / $base_program_avg_average_FGS_time_filtered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (FILTERED) than the Base Program with Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_unfiltered / $base_program_avg_average_FGS_time_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (UNFILTERED) than the Base Program with Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_runtime / $base_program_avg_runtime) * 100" | bc)
        echo "$improvement% faster Average Runtime than the Base Program with Average Runtime: $average_runtime" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"
echo >> "$RESULTS_FILE"
test_2_finish=$(date +%s)
test_2_time_diff=$(echo "$test_2_finish-test_2_start" | bc)
echo "Test 2 took $test_2_time_diff seconds."
echo

#TEST 3 (filtered_main vs parallel_filtered_main (Mostly for Query time))

test_3_start=$(date +%s)

# Program to execute
PROGRAMS=( \
"./bin/filtered_main 100 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_filtered_main 100 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
)
RESULTS_FILE="results.txt"
TEMP_FILE="temp_output.txt"
RUNS=5

echo "Test 3 (filtered_main vs parallel_filtered_main (Mostly for Query time))" >> "$RESULTS_FILE"
echo "Running Test 3/12 (filtered_main vs parallel_filtered_main (Mostly for Query time))"

# Initialize variables
base_program_avg_total_recall_all=0
base_program_avg_average_recall_all=0
base_program_avg_total_recall_filtered=0
base_program_avg_average_recall_filtered=0
base_program_avg_total_recall_unfiltered=0
base_program_avg_average_recall_unfiltered=0
base_program_avg_index_time=0
base_program_avg_total_query_time_all=0
base_program_avg_average_FGS_time_all=0
base_program_avg_average_FGS_time_filtered=0
base_program_avg_average_FGS_time_unfiltered=0
base_program_avg_runtime=0

program_avg_total_recalls_all=()
program_avg_average_recalls_all=()
program_avg_total_recalls_filtered=()
program_avg_average_recalls_filtered=()
program_avg_total_recalls_unfiltered=()
program_avg_average_recalls_unfiltered=()
program_avg_index_times=()
program_avg_total_query_times_all=()
program_avg_average_FGS_times_all=()
program_avg_average_FGS_times_filtered=()
program_avg_average_FGS_times_unfiltered=()
program_avg_runtimes=()


echo "Run Results:" >> "$RESULTS_FILE"

echo >> "$RESULTS_FILE"

echo "Programs are:" >> "$RESULTS_FILE"

for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"
    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))): $PROGRAM" >> "$RESULTS_FILE"
    else 
        echo "Program $((p+1)): $PROGRAM" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"

# Run each program
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"

    program_total_recall_all_sum=0
    program_average_recall_all_sum=0
    program_total_recall_filtered_sum=0
    program_average_recall_filtered_sum=0
    program_total_recall_unfiltered_sum=0
    program_average_recall_unfiltered_sum=0
    program_index_time_sum=0
    program_total_query_time_all_sum=0
    program_average_FGS_time_all_sum=0
    program_average_FGS_time_filtered_sum=0
    program_average_FGS_time_unfiltered_sum=0
    program_runtime_sum=0

    echo "Running Program $((p+1))/${#PROGRAMS[@]}: $PROGRAM" >> "$RESULTS_FILE"
    echo "Running Program $((p+1))/${#PROGRAMS[@]}..."

    for ((i=1; i<=RUNS; i++)); do
        echo "Running the program (Run $i/$RUNS)..."

        # Run the program and capture output
        $PROGRAM > "$TEMP_FILE"

        # Extract recalls and timing summary using grep and awk
        total_recall_all=$(awk '/Total recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_all=$(awk '/Average recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_filtered=$(awk '/Total recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_filtered=$(awk '/Average recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_unfiltered=$(awk '/Total recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_unfiltered=$(awk '/Average recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        index_time=$(awk '/Index build time/ {print $(NF-4)}' "$TEMP_FILE")
        total_query_time_all=$(awk '/Total time Query calculation took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_all=$(awk '/Average time FilteredGreadySearch took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_filtered=$(awk '/Average time FilteredGreadySearch took for FILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_unfiltered=$(awk '/Average time FilteredGreadySearch took for UNFILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        runtime=$(awk '/Program ran for/ {print $4}' "$TEMP_FILE")

        # Append run results to file
        echo "Run $i:" >> "$RESULTS_FILE"
        echo "Total Recall (ALL): $total_recall_all%" >> "$RESULTS_FILE"
        echo "Average Recall (ALL): $average_recall_all%" >> "$RESULTS_FILE"
        echo "Total Recall (Filtered): $total_recall_filtered%" >> "$RESULTS_FILE"
        echo "Average Recall (Filtered): $average_recall_filtered%" >> "$RESULTS_FILE"
        echo "Total Recall (Unfiltered): $total_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Average Recall (Unfiltered): $average_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Index Time: $index_time seconds" >> "$RESULTS_FILE"
        echo "Total Query Time (ALL): $total_query_time_all seconds" >> "$RESULTS_FILE" 
        echo "Average FGS Time (ALL): $average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Filtered): $average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Unfiltered): $average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Program Runtime: $runtime seconds" >> "$RESULTS_FILE"
        echo >> "$RESULTS_FILE"

        # Accumulate values
        program_total_recall_all_sum=$(echo "scale=12; $program_total_recall_all_sum + $total_recall_all" | bc)
        program_average_recall_all_sum=$(echo "scale=12; $program_average_recall_all_sum + $average_recall_all" | bc)
        program_total_recall_filtered_sum=$(echo "scale=12; $program_total_recall_filtered_sum + $total_recall_filtered" | bc)
        program_average_recall_filtered_sum=$(echo "scale=12; $program_average_recall_filtered_sum + $average_recall_filtered" | bc)
        program_total_recall_unfiltered_sum=$(echo "scale=12; $program_total_recall_unfiltered_sum + $total_recall_unfiltered" | bc)
        program_average_recall_unfiltered_sum=$(echo "scale=12; $program_average_recall_unfiltered_sum + $average_recall_unfiltered" | bc)
        program_index_time_sum=$(echo "scale=12; $program_index_time_sum + $index_time" | bc)
        program_total_query_time_all_sum=$(echo "scale=12; $program_total_query_time_all_sum + $total_query_time_all" | bc)
        program_average_FGS_time_all_sum=$(echo "scale=12; $program_average_FGS_time_all_sum + $average_FGS_time_all" | bc)
        program_average_FGS_time_filtered_sum=$(echo "scale=12; $program_average_FGS_time_filtered_sum + $average_FGS_time_filtered" | bc)
        program_average_FGS_time_unfiltered_sum=$(echo "scale=12; $program_average_FGS_time_unfiltered_sum + $average_FGS_time_unfiltered" | bc)
        program_runtime_sum=$(echo "scale=12; $program_runtime_sum + $runtime" | bc)
    done


    # Calculate average
    average_program_total_recall_all=$(echo "scale=10; $program_total_recall_all_sum / $RUNS" | bc)
    average_program_average_recall_all=$(echo "scale=10; $program_average_recall_all_sum / $RUNS" | bc)
    average_program_total_recall_filtered=$(echo "scale=10; $program_total_recall_filtered_sum / $RUNS" | bc)
    average_program_average_recall_filtered=$(echo "scale=10; $program_average_recall_filtered_sum / $RUNS" | bc)
    average_program_total_recall_unfiltered=$(echo "scale=10; $program_total_recall_unfiltered_sum / $RUNS" | bc)
    average_program_average_recall_unfiltered=$(echo "scale=10; $program_average_recall_unfiltered_sum / $RUNS" | bc)
    average_program_index_time=$(echo "scale=10; $program_index_time_sum / $RUNS" | bc)
    average_program_total_query_time_all=$(echo "scale=10; $program_total_query_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_all=$(echo "scale=10; $program_average_FGS_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_filtered=$(echo "scale=10; $program_average_FGS_time_filtered_sum / $RUNS" | bc)
    average_program_average_FGS_time_unfiltered=$(echo "scale=10; $program_average_FGS_time_unfiltered_sum / $RUNS" | bc)
    average_program_runtime=$(echo "scale=10; $program_runtime_sum / $RUNS" | bc)

    program_avg_total_recalls_all+=("$average_program_total_recall_all")
    program_avg_average_recalls_all+=("$average_program_average_recall_all")
    program_avg_total_recalls_filtered+=("$average_program_total_recall_filtered")
    program_avg_average_recalls_filtered+=("$average_program_average_recall_filtered")
    program_avg_total_recalls_unfiltered+=("$average_program_total_recall_unfiltered")
    program_avg_average_recalls_unfiltered+=("$average_program_average_recall_unfiltered")
    program_avg_index_times+=("$average_program_index_time")
    program_avg_total_query_times_all+=("$average_program_total_query_time_all")
    program_avg_average_FGS_times_all+=("$average_program_average_FGS_time_all")
    program_avg_average_FGS_times_filtered+=("$average_program_average_FGS_time_filtered")
    program_avg_average_FGS_times_unfiltered+=("$average_program_average_FGS_time_unfiltered")
    program_avg_runtimes+=("$average_program_runtime")

    # Save base program runtime
    if [ $p -eq 0 ]; then
        base_program_avg_total_recall_all=$average_program_total_recall_all
        base_program_avg_average_recall_all=$average_program_average_recall_all
        base_program_avg_total_recall_filtered=$average_program_total_recall_filtered
        base_program_avg_average_recall_filtered=$average_program_average_recall_filtered
        base_program_avg_total_recall_unfiltered=$average_program_total_recall_unfiltered
        base_program_avg_average_recall_unfiltered=$average_program_average_recall_unfiltered
        base_program_avg_index_time=$average_program_index_time
        base_program_avg_total_query_time_all=$average_program_total_query_time_all
        base_program_avg_average_FGS_time_all=$average_program_average_FGS_time_all
        base_program_avg_average_FGS_time_filtered=$average_program_average_FGS_time_filtered
        base_program_avg_average_FGS_time_unfiltered=$average_program_average_FGS_time_unfiltered
        base_program_avg_runtime=$average_program_runtime
    fi


    # Append average runtime to results file
    echo "Average Results for Program $((p+1)):" >> "$RESULTS_FILE"
    echo "Average Total Recall (ALL): $average_program_total_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (ALL): $average_program_average_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (FILTERED): $average_program_total_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (FILTERED): $average_program_average_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (UNFILTERED): $average_program_total_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (UNFILTERED): $average_program_average_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Index Time: $average_program_index_time seconds" >> "$RESULTS_FILE"
    echo "Average Total Query Time (ALL): $average_program_total_query_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (ALL): $average_program_average_FGS_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (FILTERED): $average_program_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (UNFILTERED): $average_program_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Runtime: $average_program_runtime seconds" >> "$RESULTS_FILE"
    echo >> "$RESULTS_FILE"
done

# Calculate percentage improvement
echo "Performance Comparison:" >> "$RESULTS_FILE"
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    average_total_recall_all="${program_avg_total_recalls_all[p]}"
    average_average_recall_all="${program_avg_average_recalls_all[p]}"
    average_total_recall_filtered="${program_avg_total_recalls_filtered[p]}"
    average_average_recall_filtered="${program_avg_average_recalls_filtered[p]}"
    average_total_recall_unfiltered="${program_avg_total_recalls_unfiltered[p]}"
    average_average_recall_unfiltered="${program_avg_average_recalls_unfiltered[p]}"
    average_index_time="${program_avg_index_times[p]}"
    average_total_query_time_all="${program_avg_total_query_times_all[p]}"
    average_average_FGS_time_all="${program_avg_average_FGS_times_all[p]}"
    average_average_FGS_time_filtered="${program_avg_average_FGS_times_filtered[p]}"
    average_average_FGS_time_unfiltered="${program_avg_average_FGS_times_unfiltered[p]}"
    average_runtime="${program_avg_runtimes[p]}"

    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))):" >> "$RESULTS_FILE"
        echo "Average Total Recall (ALL): $average_total_recall_all %" >> "$RESULTS_FILE"
        echo "Average Average Recall (ALL): $average_average_recall_all %" >> "$RESULTS_FILE"
        echo "Average Total Recall (FILTERED): $average_total_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (FILTERED): $average_average_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Total Recall (UNFILTERED): $average_total_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (UNFILTERED): $average_average_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Index Time: $average_index_time seconds" >> "$RESULTS_FILE"
        echo "Average Total Query Time (ALL): $average_total_query_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (ALL): $average_average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Average Runtime: $average_runtime seconds" >> "$RESULTS_FILE"
    else
        echo >> "$RESULTS_FILE"
        echo "Program $((p+1)):" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_all / $base_program_avg_total_recall_all) * 100" | bc)
        echo "$improvement% better Average Total Recall (ALL) than the Base Program with Average Total Recall (ALL): $average_total_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_all / $base_program_avg_average_recall_all) * 100" | bc)
        echo "$improvement% better Average Average Recall (ALL) than the Base Program with Average Average Recall (ALL): $average_average_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_filtered / $base_program_avg_total_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Total Recall (FILTERED) than the Base Program with Average Total Recall (FILTERED): $average_total_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_filtered / $base_program_avg_average_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Average Recall (FILTERED) than the Base Program with Average Average Recall (FILTERED): $average_average_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_unfiltered / $base_program_avg_total_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Total Recall (UNFILTERED) than the Base Program with Average Total Recall (UNFILTERED): $average_total_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_unfiltered / $base_program_avg_average_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average Recall (UNFILTERED) than the Base Program with Average Average Recall (UNFILTERED): $average_average_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_index_time / $base_program_avg_index_time) * 100" | bc)
        echo "$improvement% faster Average Index Time than the Base Program with Average Index Time: $average_index_time" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_query_time_all / $base_program_avg_total_query_time_all) * 100" | bc)
        echo "$improvement% better Average Total Query Time (ALL) than the Base Program with Average Total Query Time (ALL): $average_total_query_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_all / $base_program_avg_average_FGS_time_all) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (ALL) than the Base Program with Average Average FGS Time (ALL): $average_average_FGS_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_filtered / $base_program_avg_average_FGS_time_filtered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (FILTERED) than the Base Program with Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_unfiltered / $base_program_avg_average_FGS_time_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (UNFILTERED) than the Base Program with Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_runtime / $base_program_avg_runtime) * 100" | bc)
        echo "$improvement% faster Average Runtime than the Base Program with Average Runtime: $average_runtime" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"
echo >> "$RESULTS_FILE"
test_3_finish=$(date +%s)
test_3_time_diff=$(echo "$test_3_finish-test_3_start" | bc)
echo "Test 3 took $test_3_time_diff seconds."
echo

#TEST 4 (stitched_main vs parallel_stitched_main (Mostly for Query time))

test_4_start=$(date +%s)

# Program to execute
PROGRAMS=( \
"./bin/stitched_main 100 110 96 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main 100 110 96 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
)
RESULTS_FILE="results.txt"
TEMP_FILE="temp_output.txt"
RUNS=5

echo "Test 4 (stitched_main vs parallel_stitched_main (Mostly for Query time))" >> "$RESULTS_FILE"
echo "Running Test 4/12 (stitched_main vs parallel_stitched_main (Mostly for Query time))"

# Initialize variables
base_program_avg_total_recall_all=0
base_program_avg_average_recall_all=0
base_program_avg_total_recall_filtered=0
base_program_avg_average_recall_filtered=0
base_program_avg_total_recall_unfiltered=0
base_program_avg_average_recall_unfiltered=0
base_program_avg_index_time=0
base_program_avg_total_query_time_all=0
base_program_avg_average_FGS_time_all=0
base_program_avg_average_FGS_time_filtered=0
base_program_avg_average_FGS_time_unfiltered=0
base_program_avg_runtime=0

program_avg_total_recalls_all=()
program_avg_average_recalls_all=()
program_avg_total_recalls_filtered=()
program_avg_average_recalls_filtered=()
program_avg_total_recalls_unfiltered=()
program_avg_average_recalls_unfiltered=()
program_avg_index_times=()
program_avg_total_query_times_all=()
program_avg_average_FGS_times_all=()
program_avg_average_FGS_times_filtered=()
program_avg_average_FGS_times_unfiltered=()
program_avg_runtimes=()


echo "Run Results:" >> "$RESULTS_FILE"

echo >> "$RESULTS_FILE"

echo "Programs are:" >> "$RESULTS_FILE"

for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"
    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))): $PROGRAM" >> "$RESULTS_FILE"
    else 
        echo "Program $((p+1)): $PROGRAM" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"

# Run each program
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"

    program_total_recall_all_sum=0
    program_average_recall_all_sum=0
    program_total_recall_filtered_sum=0
    program_average_recall_filtered_sum=0
    program_total_recall_unfiltered_sum=0
    program_average_recall_unfiltered_sum=0
    program_index_time_sum=0
    program_total_query_time_all_sum=0
    program_average_FGS_time_all_sum=0
    program_average_FGS_time_filtered_sum=0
    program_average_FGS_time_unfiltered_sum=0
    program_runtime_sum=0

    echo "Running Program $((p+1))/${#PROGRAMS[@]}: $PROGRAM" >> "$RESULTS_FILE"
    echo "Running Program $((p+1))/${#PROGRAMS[@]}..."

    for ((i=1; i<=RUNS; i++)); do
        echo "Running the program (Run $i/$RUNS)..."

        # Run the program and capture output
        $PROGRAM > "$TEMP_FILE"

        # Extract recalls and timing summary using grep and awk
        total_recall_all=$(awk '/Total recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_all=$(awk '/Average recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_filtered=$(awk '/Total recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_filtered=$(awk '/Average recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_unfiltered=$(awk '/Total recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_unfiltered=$(awk '/Average recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        index_time=$(awk '/Index build time/ {print $(NF-4)}' "$TEMP_FILE")
        total_query_time_all=$(awk '/Total time Query calculation took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_all=$(awk '/Average time FilteredGreadySearch took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_filtered=$(awk '/Average time FilteredGreadySearch took for FILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_unfiltered=$(awk '/Average time FilteredGreadySearch took for UNFILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        runtime=$(awk '/Program ran for/ {print $4}' "$TEMP_FILE")

        # Append run results to file
        echo "Run $i:" >> "$RESULTS_FILE"
        echo "Total Recall (ALL): $total_recall_all%" >> "$RESULTS_FILE"
        echo "Average Recall (ALL): $average_recall_all%" >> "$RESULTS_FILE"
        echo "Total Recall (Filtered): $total_recall_filtered%" >> "$RESULTS_FILE"
        echo "Average Recall (Filtered): $average_recall_filtered%" >> "$RESULTS_FILE"
        echo "Total Recall (Unfiltered): $total_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Average Recall (Unfiltered): $average_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Index Time: $index_time seconds" >> "$RESULTS_FILE"
        echo "Total Query Time (ALL): $total_query_time_all seconds" >> "$RESULTS_FILE" 
        echo "Average FGS Time (ALL): $average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Filtered): $average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Unfiltered): $average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Program Runtime: $runtime seconds" >> "$RESULTS_FILE"
        echo >> "$RESULTS_FILE"

        # Accumulate values
        program_total_recall_all_sum=$(echo "scale=12; $program_total_recall_all_sum + $total_recall_all" | bc)
        program_average_recall_all_sum=$(echo "scale=12; $program_average_recall_all_sum + $average_recall_all" | bc)
        program_total_recall_filtered_sum=$(echo "scale=12; $program_total_recall_filtered_sum + $total_recall_filtered" | bc)
        program_average_recall_filtered_sum=$(echo "scale=12; $program_average_recall_filtered_sum + $average_recall_filtered" | bc)
        program_total_recall_unfiltered_sum=$(echo "scale=12; $program_total_recall_unfiltered_sum + $total_recall_unfiltered" | bc)
        program_average_recall_unfiltered_sum=$(echo "scale=12; $program_average_recall_unfiltered_sum + $average_recall_unfiltered" | bc)
        program_index_time_sum=$(echo "scale=12; $program_index_time_sum + $index_time" | bc)
        program_total_query_time_all_sum=$(echo "scale=12; $program_total_query_time_all_sum + $total_query_time_all" | bc)
        program_average_FGS_time_all_sum=$(echo "scale=12; $program_average_FGS_time_all_sum + $average_FGS_time_all" | bc)
        program_average_FGS_time_filtered_sum=$(echo "scale=12; $program_average_FGS_time_filtered_sum + $average_FGS_time_filtered" | bc)
        program_average_FGS_time_unfiltered_sum=$(echo "scale=12; $program_average_FGS_time_unfiltered_sum + $average_FGS_time_unfiltered" | bc)
        program_runtime_sum=$(echo "scale=12; $program_runtime_sum + $runtime" | bc)
    done


    # Calculate average
    average_program_total_recall_all=$(echo "scale=10; $program_total_recall_all_sum / $RUNS" | bc)
    average_program_average_recall_all=$(echo "scale=10; $program_average_recall_all_sum / $RUNS" | bc)
    average_program_total_recall_filtered=$(echo "scale=10; $program_total_recall_filtered_sum / $RUNS" | bc)
    average_program_average_recall_filtered=$(echo "scale=10; $program_average_recall_filtered_sum / $RUNS" | bc)
    average_program_total_recall_unfiltered=$(echo "scale=10; $program_total_recall_unfiltered_sum / $RUNS" | bc)
    average_program_average_recall_unfiltered=$(echo "scale=10; $program_average_recall_unfiltered_sum / $RUNS" | bc)
    average_program_index_time=$(echo "scale=10; $program_index_time_sum / $RUNS" | bc)
    average_program_total_query_time_all=$(echo "scale=10; $program_total_query_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_all=$(echo "scale=10; $program_average_FGS_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_filtered=$(echo "scale=10; $program_average_FGS_time_filtered_sum / $RUNS" | bc)
    average_program_average_FGS_time_unfiltered=$(echo "scale=10; $program_average_FGS_time_unfiltered_sum / $RUNS" | bc)
    average_program_runtime=$(echo "scale=10; $program_runtime_sum / $RUNS" | bc)

    program_avg_total_recalls_all+=("$average_program_total_recall_all")
    program_avg_average_recalls_all+=("$average_program_average_recall_all")
    program_avg_total_recalls_filtered+=("$average_program_total_recall_filtered")
    program_avg_average_recalls_filtered+=("$average_program_average_recall_filtered")
    program_avg_total_recalls_unfiltered+=("$average_program_total_recall_unfiltered")
    program_avg_average_recalls_unfiltered+=("$average_program_average_recall_unfiltered")
    program_avg_index_times+=("$average_program_index_time")
    program_avg_total_query_times_all+=("$average_program_total_query_time_all")
    program_avg_average_FGS_times_all+=("$average_program_average_FGS_time_all")
    program_avg_average_FGS_times_filtered+=("$average_program_average_FGS_time_filtered")
    program_avg_average_FGS_times_unfiltered+=("$average_program_average_FGS_time_unfiltered")
    program_avg_runtimes+=("$average_program_runtime")

    # Save base program runtime
    if [ $p -eq 0 ]; then
        base_program_avg_total_recall_all=$average_program_total_recall_all
        base_program_avg_average_recall_all=$average_program_average_recall_all
        base_program_avg_total_recall_filtered=$average_program_total_recall_filtered
        base_program_avg_average_recall_filtered=$average_program_average_recall_filtered
        base_program_avg_total_recall_unfiltered=$average_program_total_recall_unfiltered
        base_program_avg_average_recall_unfiltered=$average_program_average_recall_unfiltered
        base_program_avg_index_time=$average_program_index_time
        base_program_avg_total_query_time_all=$average_program_total_query_time_all
        base_program_avg_average_FGS_time_all=$average_program_average_FGS_time_all
        base_program_avg_average_FGS_time_filtered=$average_program_average_FGS_time_filtered
        base_program_avg_average_FGS_time_unfiltered=$average_program_average_FGS_time_unfiltered
        base_program_avg_runtime=$average_program_runtime
    fi


    # Append average runtime to results file
    echo "Average Results for Program $((p+1)):" >> "$RESULTS_FILE"
    echo "Average Total Recall (ALL): $average_program_total_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (ALL): $average_program_average_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (FILTERED): $average_program_total_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (FILTERED): $average_program_average_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (UNFILTERED): $average_program_total_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (UNFILTERED): $average_program_average_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Index Time: $average_program_index_time seconds" >> "$RESULTS_FILE"
    echo "Average Total Query Time (ALL): $average_program_total_query_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (ALL): $average_program_average_FGS_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (FILTERED): $average_program_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (UNFILTERED): $average_program_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Runtime: $average_program_runtime seconds" >> "$RESULTS_FILE"
    echo >> "$RESULTS_FILE"
done

# Calculate percentage improvement
echo "Performance Comparison:" >> "$RESULTS_FILE"
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    average_total_recall_all="${program_avg_total_recalls_all[p]}"
    average_average_recall_all="${program_avg_average_recalls_all[p]}"
    average_total_recall_filtered="${program_avg_total_recalls_filtered[p]}"
    average_average_recall_filtered="${program_avg_average_recalls_filtered[p]}"
    average_total_recall_unfiltered="${program_avg_total_recalls_unfiltered[p]}"
    average_average_recall_unfiltered="${program_avg_average_recalls_unfiltered[p]}"
    average_index_time="${program_avg_index_times[p]}"
    average_total_query_time_all="${program_avg_total_query_times_all[p]}"
    average_average_FGS_time_all="${program_avg_average_FGS_times_all[p]}"
    average_average_FGS_time_filtered="${program_avg_average_FGS_times_filtered[p]}"
    average_average_FGS_time_unfiltered="${program_avg_average_FGS_times_unfiltered[p]}"
    average_runtime="${program_avg_runtimes[p]}"

    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))):" >> "$RESULTS_FILE"
        echo "Average Total Recall (ALL): $average_total_recall_all %" >> "$RESULTS_FILE"
        echo "Average Average Recall (ALL): $average_average_recall_all %" >> "$RESULTS_FILE"
        echo "Average Total Recall (FILTERED): $average_total_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (FILTERED): $average_average_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Total Recall (UNFILTERED): $average_total_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (UNFILTERED): $average_average_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Index Time: $average_index_time seconds" >> "$RESULTS_FILE"
        echo "Average Total Query Time (ALL): $average_total_query_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (ALL): $average_average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Average Runtime: $average_runtime seconds" >> "$RESULTS_FILE"
    else
        echo >> "$RESULTS_FILE"
        echo "Program $((p+1)):" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_all / $base_program_avg_total_recall_all) * 100" | bc)
        echo "$improvement% better Average Total Recall (ALL) than the Base Program with Average Total Recall (ALL): $average_total_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_all / $base_program_avg_average_recall_all) * 100" | bc)
        echo "$improvement% better Average Average Recall (ALL) than the Base Program with Average Average Recall (ALL): $average_average_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_filtered / $base_program_avg_total_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Total Recall (FILTERED) than the Base Program with Average Total Recall (FILTERED): $average_total_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_filtered / $base_program_avg_average_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Average Recall (FILTERED) than the Base Program with Average Average Recall (FILTERED): $average_average_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_unfiltered / $base_program_avg_total_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Total Recall (UNFILTERED) than the Base Program with Average Total Recall (UNFILTERED): $average_total_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_unfiltered / $base_program_avg_average_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average Recall (UNFILTERED) than the Base Program with Average Average Recall (UNFILTERED): $average_average_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_index_time / $base_program_avg_index_time) * 100" | bc)
        echo "$improvement% faster Average Index Time than the Base Program with Average Index Time: $average_index_time" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_query_time_all / $base_program_avg_total_query_time_all) * 100" | bc)
        echo "$improvement% better Average Total Query Time (ALL) than the Base Program with Average Total Query Time (ALL): $average_total_query_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_all / $base_program_avg_average_FGS_time_all) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (ALL) than the Base Program with Average Average FGS Time (ALL): $average_average_FGS_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_filtered / $base_program_avg_average_FGS_time_filtered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (FILTERED) than the Base Program with Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_unfiltered / $base_program_avg_average_FGS_time_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (UNFILTERED) than the Base Program with Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_runtime / $base_program_avg_runtime) * 100" | bc)
        echo "$improvement% faster Average Runtime than the Base Program with Average Runtime: $average_runtime" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"
echo >> "$RESULTS_FILE"
test_4_finish=$(date +%s)
test_4_time_diff=$(echo "$test_4_finish-test_4_start" | bc)
echo "Test 4 took $test_4_time_diff seconds."
echo

#TEST 5 (parallel_filtered_main different L)

test_5_start=$(date +%s)

# Program to execute
PROGRAMS=( \
"./bin/parallel_filtered_main 100 50 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_filtered_main 100 70 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_filtered_main 100 90 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_filtered_main 100 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_filtered_main 100 130 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_filtered_main 100 150 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
)
RESULTS_FILE="results.txt"
TEMP_FILE="temp_output.txt"
RUNS=5

echo "Test 5 (parallel_filtered_main different L)" >> "$RESULTS_FILE"
echo "Running Test 5/12 (parallel_filtered_main different L)"

# Initialize variables
base_program_avg_total_recall_all=0
base_program_avg_average_recall_all=0
base_program_avg_total_recall_filtered=0
base_program_avg_average_recall_filtered=0
base_program_avg_total_recall_unfiltered=0
base_program_avg_average_recall_unfiltered=0
base_program_avg_index_time=0
base_program_avg_total_query_time_all=0
base_program_avg_average_FGS_time_all=0
base_program_avg_average_FGS_time_filtered=0
base_program_avg_average_FGS_time_unfiltered=0
base_program_avg_runtime=0

program_avg_total_recalls_all=()
program_avg_average_recalls_all=()
program_avg_total_recalls_filtered=()
program_avg_average_recalls_filtered=()
program_avg_total_recalls_unfiltered=()
program_avg_average_recalls_unfiltered=()
program_avg_index_times=()
program_avg_total_query_times_all=()
program_avg_average_FGS_times_all=()
program_avg_average_FGS_times_filtered=()
program_avg_average_FGS_times_unfiltered=()
program_avg_runtimes=()


echo "Run Results:" >> "$RESULTS_FILE"

echo >> "$RESULTS_FILE"

echo "Programs are:" >> "$RESULTS_FILE"

for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"
    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))): $PROGRAM" >> "$RESULTS_FILE"
    else 
        echo "Program $((p+1)): $PROGRAM" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"

# Run each program
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"

    program_total_recall_all_sum=0
    program_average_recall_all_sum=0
    program_total_recall_filtered_sum=0
    program_average_recall_filtered_sum=0
    program_total_recall_unfiltered_sum=0
    program_average_recall_unfiltered_sum=0
    program_index_time_sum=0
    program_total_query_time_all_sum=0
    program_average_FGS_time_all_sum=0
    program_average_FGS_time_filtered_sum=0
    program_average_FGS_time_unfiltered_sum=0
    program_runtime_sum=0

    echo "Running Program $((p+1))/${#PROGRAMS[@]}: $PROGRAM" >> "$RESULTS_FILE"
    echo "Running Program $((p+1))/${#PROGRAMS[@]}..."

    for ((i=1; i<=RUNS; i++)); do
        echo "Running the program (Run $i/$RUNS)..."

        # Run the program and capture output
        $PROGRAM > "$TEMP_FILE"

        # Extract recalls and timing summary using grep and awk
        total_recall_all=$(awk '/Total recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_all=$(awk '/Average recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_filtered=$(awk '/Total recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_filtered=$(awk '/Average recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_unfiltered=$(awk '/Total recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_unfiltered=$(awk '/Average recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        index_time=$(awk '/Index build time/ {print $(NF-4)}' "$TEMP_FILE")
        total_query_time_all=$(awk '/Total time Query calculation took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_all=$(awk '/Average time FilteredGreadySearch took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_filtered=$(awk '/Average time FilteredGreadySearch took for FILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_unfiltered=$(awk '/Average time FilteredGreadySearch took for UNFILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        runtime=$(awk '/Program ran for/ {print $4}' "$TEMP_FILE")

        # Append run results to file
        echo "Run $i:" >> "$RESULTS_FILE"
        echo "Total Recall (ALL): $total_recall_all%" >> "$RESULTS_FILE"
        echo "Average Recall (ALL): $average_recall_all%" >> "$RESULTS_FILE"
        echo "Total Recall (Filtered): $total_recall_filtered%" >> "$RESULTS_FILE"
        echo "Average Recall (Filtered): $average_recall_filtered%" >> "$RESULTS_FILE"
        echo "Total Recall (Unfiltered): $total_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Average Recall (Unfiltered): $average_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Index Time: $index_time seconds" >> "$RESULTS_FILE"
        echo "Total Query Time (ALL): $total_query_time_all seconds" >> "$RESULTS_FILE" 
        echo "Average FGS Time (ALL): $average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Filtered): $average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Unfiltered): $average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Program Runtime: $runtime seconds" >> "$RESULTS_FILE"
        echo >> "$RESULTS_FILE"

        # Accumulate values
        program_total_recall_all_sum=$(echo "scale=12; $program_total_recall_all_sum + $total_recall_all" | bc)
        program_average_recall_all_sum=$(echo "scale=12; $program_average_recall_all_sum + $average_recall_all" | bc)
        program_total_recall_filtered_sum=$(echo "scale=12; $program_total_recall_filtered_sum + $total_recall_filtered" | bc)
        program_average_recall_filtered_sum=$(echo "scale=12; $program_average_recall_filtered_sum + $average_recall_filtered" | bc)
        program_total_recall_unfiltered_sum=$(echo "scale=12; $program_total_recall_unfiltered_sum + $total_recall_unfiltered" | bc)
        program_average_recall_unfiltered_sum=$(echo "scale=12; $program_average_recall_unfiltered_sum + $average_recall_unfiltered" | bc)
        program_index_time_sum=$(echo "scale=12; $program_index_time_sum + $index_time" | bc)
        program_total_query_time_all_sum=$(echo "scale=12; $program_total_query_time_all_sum + $total_query_time_all" | bc)
        program_average_FGS_time_all_sum=$(echo "scale=12; $program_average_FGS_time_all_sum + $average_FGS_time_all" | bc)
        program_average_FGS_time_filtered_sum=$(echo "scale=12; $program_average_FGS_time_filtered_sum + $average_FGS_time_filtered" | bc)
        program_average_FGS_time_unfiltered_sum=$(echo "scale=12; $program_average_FGS_time_unfiltered_sum + $average_FGS_time_unfiltered" | bc)
        program_runtime_sum=$(echo "scale=12; $program_runtime_sum + $runtime" | bc)
    done


    # Calculate average
    average_program_total_recall_all=$(echo "scale=10; $program_total_recall_all_sum / $RUNS" | bc)
    average_program_average_recall_all=$(echo "scale=10; $program_average_recall_all_sum / $RUNS" | bc)
    average_program_total_recall_filtered=$(echo "scale=10; $program_total_recall_filtered_sum / $RUNS" | bc)
    average_program_average_recall_filtered=$(echo "scale=10; $program_average_recall_filtered_sum / $RUNS" | bc)
    average_program_total_recall_unfiltered=$(echo "scale=10; $program_total_recall_unfiltered_sum / $RUNS" | bc)
    average_program_average_recall_unfiltered=$(echo "scale=10; $program_average_recall_unfiltered_sum / $RUNS" | bc)
    average_program_index_time=$(echo "scale=10; $program_index_time_sum / $RUNS" | bc)
    average_program_total_query_time_all=$(echo "scale=10; $program_total_query_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_all=$(echo "scale=10; $program_average_FGS_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_filtered=$(echo "scale=10; $program_average_FGS_time_filtered_sum / $RUNS" | bc)
    average_program_average_FGS_time_unfiltered=$(echo "scale=10; $program_average_FGS_time_unfiltered_sum / $RUNS" | bc)
    average_program_runtime=$(echo "scale=10; $program_runtime_sum / $RUNS" | bc)

    program_avg_total_recalls_all+=("$average_program_total_recall_all")
    program_avg_average_recalls_all+=("$average_program_average_recall_all")
    program_avg_total_recalls_filtered+=("$average_program_total_recall_filtered")
    program_avg_average_recalls_filtered+=("$average_program_average_recall_filtered")
    program_avg_total_recalls_unfiltered+=("$average_program_total_recall_unfiltered")
    program_avg_average_recalls_unfiltered+=("$average_program_average_recall_unfiltered")
    program_avg_index_times+=("$average_program_index_time")
    program_avg_total_query_times_all+=("$average_program_total_query_time_all")
    program_avg_average_FGS_times_all+=("$average_program_average_FGS_time_all")
    program_avg_average_FGS_times_filtered+=("$average_program_average_FGS_time_filtered")
    program_avg_average_FGS_times_unfiltered+=("$average_program_average_FGS_time_unfiltered")
    program_avg_runtimes+=("$average_program_runtime")

    # Save base program runtime
    if [ $p -eq 0 ]; then
        base_program_avg_total_recall_all=$average_program_total_recall_all
        base_program_avg_average_recall_all=$average_program_average_recall_all
        base_program_avg_total_recall_filtered=$average_program_total_recall_filtered
        base_program_avg_average_recall_filtered=$average_program_average_recall_filtered
        base_program_avg_total_recall_unfiltered=$average_program_total_recall_unfiltered
        base_program_avg_average_recall_unfiltered=$average_program_average_recall_unfiltered
        base_program_avg_index_time=$average_program_index_time
        base_program_avg_total_query_time_all=$average_program_total_query_time_all
        base_program_avg_average_FGS_time_all=$average_program_average_FGS_time_all
        base_program_avg_average_FGS_time_filtered=$average_program_average_FGS_time_filtered
        base_program_avg_average_FGS_time_unfiltered=$average_program_average_FGS_time_unfiltered
        base_program_avg_runtime=$average_program_runtime
    fi


    # Append average runtime to results file
    echo "Average Results for Program $((p+1)):" >> "$RESULTS_FILE"
    echo "Average Total Recall (ALL): $average_program_total_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (ALL): $average_program_average_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (FILTERED): $average_program_total_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (FILTERED): $average_program_average_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (UNFILTERED): $average_program_total_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (UNFILTERED): $average_program_average_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Index Time: $average_program_index_time seconds" >> "$RESULTS_FILE"
    echo "Average Total Query Time (ALL): $average_program_total_query_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (ALL): $average_program_average_FGS_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (FILTERED): $average_program_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (UNFILTERED): $average_program_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Runtime: $average_program_runtime seconds" >> "$RESULTS_FILE"
    echo >> "$RESULTS_FILE"
done

# Calculate percentage improvement
echo "Performance Comparison:" >> "$RESULTS_FILE"
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    average_total_recall_all="${program_avg_total_recalls_all[p]}"
    average_average_recall_all="${program_avg_average_recalls_all[p]}"
    average_total_recall_filtered="${program_avg_total_recalls_filtered[p]}"
    average_average_recall_filtered="${program_avg_average_recalls_filtered[p]}"
    average_total_recall_unfiltered="${program_avg_total_recalls_unfiltered[p]}"
    average_average_recall_unfiltered="${program_avg_average_recalls_unfiltered[p]}"
    average_index_time="${program_avg_index_times[p]}"
    average_total_query_time_all="${program_avg_total_query_times_all[p]}"
    average_average_FGS_time_all="${program_avg_average_FGS_times_all[p]}"
    average_average_FGS_time_filtered="${program_avg_average_FGS_times_filtered[p]}"
    average_average_FGS_time_unfiltered="${program_avg_average_FGS_times_unfiltered[p]}"
    average_runtime="${program_avg_runtimes[p]}"

    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))):" >> "$RESULTS_FILE"
        echo "Average Total Recall (ALL): $average_total_recall_all %" >> "$RESULTS_FILE"
        echo "Average Average Recall (ALL): $average_average_recall_all %" >> "$RESULTS_FILE"
        echo "Average Total Recall (FILTERED): $average_total_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (FILTERED): $average_average_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Total Recall (UNFILTERED): $average_total_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (UNFILTERED): $average_average_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Index Time: $average_index_time seconds" >> "$RESULTS_FILE"
        echo "Average Total Query Time (ALL): $average_total_query_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (ALL): $average_average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Average Runtime: $average_runtime seconds" >> "$RESULTS_FILE"
    else
        echo >> "$RESULTS_FILE"
        echo "Program $((p+1)):" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_all / $base_program_avg_total_recall_all) * 100" | bc)
        echo "$improvement% better Average Total Recall (ALL) than the Base Program with Average Total Recall (ALL): $average_total_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_all / $base_program_avg_average_recall_all) * 100" | bc)
        echo "$improvement% better Average Average Recall (ALL) than the Base Program with Average Average Recall (ALL): $average_average_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_filtered / $base_program_avg_total_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Total Recall (FILTERED) than the Base Program with Average Total Recall (FILTERED): $average_total_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_filtered / $base_program_avg_average_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Average Recall (FILTERED) than the Base Program with Average Average Recall (FILTERED): $average_average_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_unfiltered / $base_program_avg_total_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Total Recall (UNFILTERED) than the Base Program with Average Total Recall (UNFILTERED): $average_total_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_unfiltered / $base_program_avg_average_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average Recall (UNFILTERED) than the Base Program with Average Average Recall (UNFILTERED): $average_average_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_index_time / $base_program_avg_index_time) * 100" | bc)
        echo "$improvement% faster Average Index Time than the Base Program with Average Index Time: $average_index_time" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_query_time_all / $base_program_avg_total_query_time_all) * 100" | bc)
        echo "$improvement% better Average Total Query Time (ALL) than the Base Program with Average Total Query Time (ALL): $average_total_query_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_all / $base_program_avg_average_FGS_time_all) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (ALL) than the Base Program with Average Average FGS Time (ALL): $average_average_FGS_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_filtered / $base_program_avg_average_FGS_time_filtered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (FILTERED) than the Base Program with Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_unfiltered / $base_program_avg_average_FGS_time_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (UNFILTERED) than the Base Program with Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_runtime / $base_program_avg_runtime) * 100" | bc)
        echo "$improvement% faster Average Runtime than the Base Program with Average Runtime: $average_runtime" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"
echo >> "$RESULTS_FILE"
test_5_finish=$(date +%s)
test_5_time_diff=$(echo "$test_5_finish-test_5_start" | bc)
echo "Test 5 took $test_5_time_diff seconds."
echo

#TEST 6 (parallel_filtered_main different k)

test_6_start=$(date +%s)

# Program to execute
PROGRAMS=( \
"./bin/parallel_filtered_main 20 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=20.bin" \
"./bin/parallel_filtered_main 40 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=40.bin" \
"./bin/parallel_filtered_main 60 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=60.bin" \
"./bin/parallel_filtered_main 80 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=80.bin" \
"./bin/parallel_filtered_main 100 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
)
RESULTS_FILE="results.txt"
TEMP_FILE="temp_output.txt"
RUNS=5

echo "Test 6 (parallel_filtered_main different k)" >> "$RESULTS_FILE"
echo "Running Test 6/12 (parallel_filtered_main different k)"

# Initialize variables
base_program_avg_total_recall_all=0
base_program_avg_average_recall_all=0
base_program_avg_total_recall_filtered=0
base_program_avg_average_recall_filtered=0
base_program_avg_total_recall_unfiltered=0
base_program_avg_average_recall_unfiltered=0
base_program_avg_index_time=0
base_program_avg_total_query_time_all=0
base_program_avg_average_FGS_time_all=0
base_program_avg_average_FGS_time_filtered=0
base_program_avg_average_FGS_time_unfiltered=0
base_program_avg_runtime=0

program_avg_total_recalls_all=()
program_avg_average_recalls_all=()
program_avg_total_recalls_filtered=()
program_avg_average_recalls_filtered=()
program_avg_total_recalls_unfiltered=()
program_avg_average_recalls_unfiltered=()
program_avg_index_times=()
program_avg_total_query_times_all=()
program_avg_average_FGS_times_all=()
program_avg_average_FGS_times_filtered=()
program_avg_average_FGS_times_unfiltered=()
program_avg_runtimes=()


echo "Run Results:" >> "$RESULTS_FILE"

echo >> "$RESULTS_FILE"

echo "Programs are:" >> "$RESULTS_FILE"

for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"
    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))): $PROGRAM" >> "$RESULTS_FILE"
    else 
        echo "Program $((p+1)): $PROGRAM" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"

# Run each program
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"

    program_total_recall_all_sum=0
    program_average_recall_all_sum=0
    program_total_recall_filtered_sum=0
    program_average_recall_filtered_sum=0
    program_total_recall_unfiltered_sum=0
    program_average_recall_unfiltered_sum=0
    program_index_time_sum=0
    program_total_query_time_all_sum=0
    program_average_FGS_time_all_sum=0
    program_average_FGS_time_filtered_sum=0
    program_average_FGS_time_unfiltered_sum=0
    program_runtime_sum=0

    echo "Running Program $((p+1))/${#PROGRAMS[@]}: $PROGRAM" >> "$RESULTS_FILE"
    echo "Running Program $((p+1))/${#PROGRAMS[@]}..."

    for ((i=1; i<=RUNS; i++)); do
        echo "Running the program (Run $i/$RUNS)..."

        # Run the program and capture output
        $PROGRAM > "$TEMP_FILE"

        # Extract recalls and timing summary using grep and awk
        total_recall_all=$(awk '/Total recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_all=$(awk '/Average recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_filtered=$(awk '/Total recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_filtered=$(awk '/Average recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_unfiltered=$(awk '/Total recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_unfiltered=$(awk '/Average recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        index_time=$(awk '/Index build time/ {print $(NF-4)}' "$TEMP_FILE")
        total_query_time_all=$(awk '/Total time Query calculation took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_all=$(awk '/Average time FilteredGreadySearch took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_filtered=$(awk '/Average time FilteredGreadySearch took for FILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_unfiltered=$(awk '/Average time FilteredGreadySearch took for UNFILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        runtime=$(awk '/Program ran for/ {print $4}' "$TEMP_FILE")

        # Append run results to file
        echo "Run $i:" >> "$RESULTS_FILE"
        echo "Total Recall (ALL): $total_recall_all%" >> "$RESULTS_FILE"
        echo "Average Recall (ALL): $average_recall_all%" >> "$RESULTS_FILE"
        echo "Total Recall (Filtered): $total_recall_filtered%" >> "$RESULTS_FILE"
        echo "Average Recall (Filtered): $average_recall_filtered%" >> "$RESULTS_FILE"
        echo "Total Recall (Unfiltered): $total_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Average Recall (Unfiltered): $average_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Index Time: $index_time seconds" >> "$RESULTS_FILE"
        echo "Total Query Time (ALL): $total_query_time_all seconds" >> "$RESULTS_FILE" 
        echo "Average FGS Time (ALL): $average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Filtered): $average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Unfiltered): $average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Program Runtime: $runtime seconds" >> "$RESULTS_FILE"
        echo >> "$RESULTS_FILE"

        # Accumulate values
        program_total_recall_all_sum=$(echo "scale=12; $program_total_recall_all_sum + $total_recall_all" | bc)
        program_average_recall_all_sum=$(echo "scale=12; $program_average_recall_all_sum + $average_recall_all" | bc)
        program_total_recall_filtered_sum=$(echo "scale=12; $program_total_recall_filtered_sum + $total_recall_filtered" | bc)
        program_average_recall_filtered_sum=$(echo "scale=12; $program_average_recall_filtered_sum + $average_recall_filtered" | bc)
        program_total_recall_unfiltered_sum=$(echo "scale=12; $program_total_recall_unfiltered_sum + $total_recall_unfiltered" | bc)
        program_average_recall_unfiltered_sum=$(echo "scale=12; $program_average_recall_unfiltered_sum + $average_recall_unfiltered" | bc)
        program_index_time_sum=$(echo "scale=12; $program_index_time_sum + $index_time" | bc)
        program_total_query_time_all_sum=$(echo "scale=12; $program_total_query_time_all_sum + $total_query_time_all" | bc)
        program_average_FGS_time_all_sum=$(echo "scale=12; $program_average_FGS_time_all_sum + $average_FGS_time_all" | bc)
        program_average_FGS_time_filtered_sum=$(echo "scale=12; $program_average_FGS_time_filtered_sum + $average_FGS_time_filtered" | bc)
        program_average_FGS_time_unfiltered_sum=$(echo "scale=12; $program_average_FGS_time_unfiltered_sum + $average_FGS_time_unfiltered" | bc)
        program_runtime_sum=$(echo "scale=12; $program_runtime_sum + $runtime" | bc)
    done


    # Calculate average
    average_program_total_recall_all=$(echo "scale=10; $program_total_recall_all_sum / $RUNS" | bc)
    average_program_average_recall_all=$(echo "scale=10; $program_average_recall_all_sum / $RUNS" | bc)
    average_program_total_recall_filtered=$(echo "scale=10; $program_total_recall_filtered_sum / $RUNS" | bc)
    average_program_average_recall_filtered=$(echo "scale=10; $program_average_recall_filtered_sum / $RUNS" | bc)
    average_program_total_recall_unfiltered=$(echo "scale=10; $program_total_recall_unfiltered_sum / $RUNS" | bc)
    average_program_average_recall_unfiltered=$(echo "scale=10; $program_average_recall_unfiltered_sum / $RUNS" | bc)
    average_program_index_time=$(echo "scale=10; $program_index_time_sum / $RUNS" | bc)
    average_program_total_query_time_all=$(echo "scale=10; $program_total_query_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_all=$(echo "scale=10; $program_average_FGS_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_filtered=$(echo "scale=10; $program_average_FGS_time_filtered_sum / $RUNS" | bc)
    average_program_average_FGS_time_unfiltered=$(echo "scale=10; $program_average_FGS_time_unfiltered_sum / $RUNS" | bc)
    average_program_runtime=$(echo "scale=10; $program_runtime_sum / $RUNS" | bc)

    program_avg_total_recalls_all+=("$average_program_total_recall_all")
    program_avg_average_recalls_all+=("$average_program_average_recall_all")
    program_avg_total_recalls_filtered+=("$average_program_total_recall_filtered")
    program_avg_average_recalls_filtered+=("$average_program_average_recall_filtered")
    program_avg_total_recalls_unfiltered+=("$average_program_total_recall_unfiltered")
    program_avg_average_recalls_unfiltered+=("$average_program_average_recall_unfiltered")
    program_avg_index_times+=("$average_program_index_time")
    program_avg_total_query_times_all+=("$average_program_total_query_time_all")
    program_avg_average_FGS_times_all+=("$average_program_average_FGS_time_all")
    program_avg_average_FGS_times_filtered+=("$average_program_average_FGS_time_filtered")
    program_avg_average_FGS_times_unfiltered+=("$average_program_average_FGS_time_unfiltered")
    program_avg_runtimes+=("$average_program_runtime")

    # Save base program runtime
    if [ $p -eq 0 ]; then
        base_program_avg_total_recall_all=$average_program_total_recall_all
        base_program_avg_average_recall_all=$average_program_average_recall_all
        base_program_avg_total_recall_filtered=$average_program_total_recall_filtered
        base_program_avg_average_recall_filtered=$average_program_average_recall_filtered
        base_program_avg_total_recall_unfiltered=$average_program_total_recall_unfiltered
        base_program_avg_average_recall_unfiltered=$average_program_average_recall_unfiltered
        base_program_avg_index_time=$average_program_index_time
        base_program_avg_total_query_time_all=$average_program_total_query_time_all
        base_program_avg_average_FGS_time_all=$average_program_average_FGS_time_all
        base_program_avg_average_FGS_time_filtered=$average_program_average_FGS_time_filtered
        base_program_avg_average_FGS_time_unfiltered=$average_program_average_FGS_time_unfiltered
        base_program_avg_runtime=$average_program_runtime
    fi


    # Append average runtime to results file
    echo "Average Results for Program $((p+1)):" >> "$RESULTS_FILE"
    echo "Average Total Recall (ALL): $average_program_total_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (ALL): $average_program_average_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (FILTERED): $average_program_total_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (FILTERED): $average_program_average_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (UNFILTERED): $average_program_total_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (UNFILTERED): $average_program_average_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Index Time: $average_program_index_time seconds" >> "$RESULTS_FILE"
    echo "Average Total Query Time (ALL): $average_program_total_query_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (ALL): $average_program_average_FGS_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (FILTERED): $average_program_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (UNFILTERED): $average_program_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Runtime: $average_program_runtime seconds" >> "$RESULTS_FILE"
    echo >> "$RESULTS_FILE"
done

# Calculate percentage improvement
echo "Performance Comparison:" >> "$RESULTS_FILE"
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    average_total_recall_all="${program_avg_total_recalls_all[p]}"
    average_average_recall_all="${program_avg_average_recalls_all[p]}"
    average_total_recall_filtered="${program_avg_total_recalls_filtered[p]}"
    average_average_recall_filtered="${program_avg_average_recalls_filtered[p]}"
    average_total_recall_unfiltered="${program_avg_total_recalls_unfiltered[p]}"
    average_average_recall_unfiltered="${program_avg_average_recalls_unfiltered[p]}"
    average_index_time="${program_avg_index_times[p]}"
    average_total_query_time_all="${program_avg_total_query_times_all[p]}"
    average_average_FGS_time_all="${program_avg_average_FGS_times_all[p]}"
    average_average_FGS_time_filtered="${program_avg_average_FGS_times_filtered[p]}"
    average_average_FGS_time_unfiltered="${program_avg_average_FGS_times_unfiltered[p]}"
    average_runtime="${program_avg_runtimes[p]}"

    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))):" >> "$RESULTS_FILE"
        echo "Average Total Recall (ALL): $average_total_recall_all %" >> "$RESULTS_FILE"
        echo "Average Average Recall (ALL): $average_average_recall_all %" >> "$RESULTS_FILE"
        echo "Average Total Recall (FILTERED): $average_total_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (FILTERED): $average_average_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Total Recall (UNFILTERED): $average_total_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (UNFILTERED): $average_average_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Index Time: $average_index_time seconds" >> "$RESULTS_FILE"
        echo "Average Total Query Time (ALL): $average_total_query_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (ALL): $average_average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Average Runtime: $average_runtime seconds" >> "$RESULTS_FILE"
    else
        echo >> "$RESULTS_FILE"
        echo "Program $((p+1)):" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_all / $base_program_avg_total_recall_all) * 100" | bc)
        echo "$improvement% better Average Total Recall (ALL) than the Base Program with Average Total Recall (ALL): $average_total_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_all / $base_program_avg_average_recall_all) * 100" | bc)
        echo "$improvement% better Average Average Recall (ALL) than the Base Program with Average Average Recall (ALL): $average_average_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_filtered / $base_program_avg_total_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Total Recall (FILTERED) than the Base Program with Average Total Recall (FILTERED): $average_total_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_filtered / $base_program_avg_average_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Average Recall (FILTERED) than the Base Program with Average Average Recall (FILTERED): $average_average_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_unfiltered / $base_program_avg_total_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Total Recall (UNFILTERED) than the Base Program with Average Total Recall (UNFILTERED): $average_total_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_unfiltered / $base_program_avg_average_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average Recall (UNFILTERED) than the Base Program with Average Average Recall (UNFILTERED): $average_average_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_index_time / $base_program_avg_index_time) * 100" | bc)
        echo "$improvement% faster Average Index Time than the Base Program with Average Index Time: $average_index_time" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_query_time_all / $base_program_avg_total_query_time_all) * 100" | bc)
        echo "$improvement% better Average Total Query Time (ALL) than the Base Program with Average Total Query Time (ALL): $average_total_query_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_all / $base_program_avg_average_FGS_time_all) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (ALL) than the Base Program with Average Average FGS Time (ALL): $average_average_FGS_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_filtered / $base_program_avg_average_FGS_time_filtered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (FILTERED) than the Base Program with Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_unfiltered / $base_program_avg_average_FGS_time_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (UNFILTERED) than the Base Program with Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_runtime / $base_program_avg_runtime) * 100" | bc)
        echo "$improvement% faster Average Runtime than the Base Program with Average Runtime: $average_runtime" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"
echo >> "$RESULTS_FILE"
test_6_finish=$(date +%s)
test_6_time_diff=$(echo "$test_6_finish-test_6_start" | bc)
echo "Test 6 took $test_6_time_diff seconds."
echo

#TEST 7 (parallel_filtered_main different R)

test_7_start=$(date +%s)

# Program to execute
PROGRAMS=( \
"./bin/parallel_filtered_main 100 110 50 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_filtered_main 100 110 70 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_filtered_main 100 110 90 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_filtered_main 100 110 110 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_filtered_main 100 110 130 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_filtered_main 100 110 150 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
)
RESULTS_FILE="results.txt"
TEMP_FILE="temp_output.txt"
RUNS=5

echo "Test 7 (parallel_filtered_main different R)" >> "$RESULTS_FILE"
echo "Running Test 7/12 (parallel_filtered_main different R)"

# Initialize variables
base_program_avg_total_recall_all=0
base_program_avg_average_recall_all=0
base_program_avg_total_recall_filtered=0
base_program_avg_average_recall_filtered=0
base_program_avg_total_recall_unfiltered=0
base_program_avg_average_recall_unfiltered=0
base_program_avg_index_time=0
base_program_avg_total_query_time_all=0
base_program_avg_average_FGS_time_all=0
base_program_avg_average_FGS_time_filtered=0
base_program_avg_average_FGS_time_unfiltered=0
base_program_avg_runtime=0

program_avg_total_recalls_all=()
program_avg_average_recalls_all=()
program_avg_total_recalls_filtered=()
program_avg_average_recalls_filtered=()
program_avg_total_recalls_unfiltered=()
program_avg_average_recalls_unfiltered=()
program_avg_index_times=()
program_avg_total_query_times_all=()
program_avg_average_FGS_times_all=()
program_avg_average_FGS_times_filtered=()
program_avg_average_FGS_times_unfiltered=()
program_avg_runtimes=()


echo "Run Results:" >> "$RESULTS_FILE"

echo >> "$RESULTS_FILE"

echo "Programs are:" >> "$RESULTS_FILE"

for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"
    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))): $PROGRAM" >> "$RESULTS_FILE"
    else 
        echo "Program $((p+1)): $PROGRAM" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"

# Run each program
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"

    program_total_recall_all_sum=0
    program_average_recall_all_sum=0
    program_total_recall_filtered_sum=0
    program_average_recall_filtered_sum=0
    program_total_recall_unfiltered_sum=0
    program_average_recall_unfiltered_sum=0
    program_index_time_sum=0
    program_total_query_time_all_sum=0
    program_average_FGS_time_all_sum=0
    program_average_FGS_time_filtered_sum=0
    program_average_FGS_time_unfiltered_sum=0
    program_runtime_sum=0

    echo "Running Program $((p+1))/${#PROGRAMS[@]}: $PROGRAM" >> "$RESULTS_FILE"
    echo "Running Program $((p+1))/${#PROGRAMS[@]}..."

    for ((i=1; i<=RUNS; i++)); do
        echo "Running the program (Run $i/$RUNS)..."

        # Run the program and capture output
        $PROGRAM > "$TEMP_FILE"

        # Extract recalls and timing summary using grep and awk
        total_recall_all=$(awk '/Total recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_all=$(awk '/Average recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_filtered=$(awk '/Total recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_filtered=$(awk '/Average recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_unfiltered=$(awk '/Total recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_unfiltered=$(awk '/Average recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        index_time=$(awk '/Index build time/ {print $(NF-4)}' "$TEMP_FILE")
        total_query_time_all=$(awk '/Total time Query calculation took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_all=$(awk '/Average time FilteredGreadySearch took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_filtered=$(awk '/Average time FilteredGreadySearch took for FILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_unfiltered=$(awk '/Average time FilteredGreadySearch took for UNFILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        runtime=$(awk '/Program ran for/ {print $4}' "$TEMP_FILE")

        # Append run results to file
        echo "Run $i:" >> "$RESULTS_FILE"
        echo "Total Recall (ALL): $total_recall_all%" >> "$RESULTS_FILE"
        echo "Average Recall (ALL): $average_recall_all%" >> "$RESULTS_FILE"
        echo "Total Recall (Filtered): $total_recall_filtered%" >> "$RESULTS_FILE"
        echo "Average Recall (Filtered): $average_recall_filtered%" >> "$RESULTS_FILE"
        echo "Total Recall (Unfiltered): $total_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Average Recall (Unfiltered): $average_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Index Time: $index_time seconds" >> "$RESULTS_FILE"
        echo "Total Query Time (ALL): $total_query_time_all seconds" >> "$RESULTS_FILE" 
        echo "Average FGS Time (ALL): $average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Filtered): $average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Unfiltered): $average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Program Runtime: $runtime seconds" >> "$RESULTS_FILE"
        echo >> "$RESULTS_FILE"

        # Accumulate values
        program_total_recall_all_sum=$(echo "scale=12; $program_total_recall_all_sum + $total_recall_all" | bc)
        program_average_recall_all_sum=$(echo "scale=12; $program_average_recall_all_sum + $average_recall_all" | bc)
        program_total_recall_filtered_sum=$(echo "scale=12; $program_total_recall_filtered_sum + $total_recall_filtered" | bc)
        program_average_recall_filtered_sum=$(echo "scale=12; $program_average_recall_filtered_sum + $average_recall_filtered" | bc)
        program_total_recall_unfiltered_sum=$(echo "scale=12; $program_total_recall_unfiltered_sum + $total_recall_unfiltered" | bc)
        program_average_recall_unfiltered_sum=$(echo "scale=12; $program_average_recall_unfiltered_sum + $average_recall_unfiltered" | bc)
        program_index_time_sum=$(echo "scale=12; $program_index_time_sum + $index_time" | bc)
        program_total_query_time_all_sum=$(echo "scale=12; $program_total_query_time_all_sum + $total_query_time_all" | bc)
        program_average_FGS_time_all_sum=$(echo "scale=12; $program_average_FGS_time_all_sum + $average_FGS_time_all" | bc)
        program_average_FGS_time_filtered_sum=$(echo "scale=12; $program_average_FGS_time_filtered_sum + $average_FGS_time_filtered" | bc)
        program_average_FGS_time_unfiltered_sum=$(echo "scale=12; $program_average_FGS_time_unfiltered_sum + $average_FGS_time_unfiltered" | bc)
        program_runtime_sum=$(echo "scale=12; $program_runtime_sum + $runtime" | bc)
    done


    # Calculate average
    average_program_total_recall_all=$(echo "scale=10; $program_total_recall_all_sum / $RUNS" | bc)
    average_program_average_recall_all=$(echo "scale=10; $program_average_recall_all_sum / $RUNS" | bc)
    average_program_total_recall_filtered=$(echo "scale=10; $program_total_recall_filtered_sum / $RUNS" | bc)
    average_program_average_recall_filtered=$(echo "scale=10; $program_average_recall_filtered_sum / $RUNS" | bc)
    average_program_total_recall_unfiltered=$(echo "scale=10; $program_total_recall_unfiltered_sum / $RUNS" | bc)
    average_program_average_recall_unfiltered=$(echo "scale=10; $program_average_recall_unfiltered_sum / $RUNS" | bc)
    average_program_index_time=$(echo "scale=10; $program_index_time_sum / $RUNS" | bc)
    average_program_total_query_time_all=$(echo "scale=10; $program_total_query_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_all=$(echo "scale=10; $program_average_FGS_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_filtered=$(echo "scale=10; $program_average_FGS_time_filtered_sum / $RUNS" | bc)
    average_program_average_FGS_time_unfiltered=$(echo "scale=10; $program_average_FGS_time_unfiltered_sum / $RUNS" | bc)
    average_program_runtime=$(echo "scale=10; $program_runtime_sum / $RUNS" | bc)

    program_avg_total_recalls_all+=("$average_program_total_recall_all")
    program_avg_average_recalls_all+=("$average_program_average_recall_all")
    program_avg_total_recalls_filtered+=("$average_program_total_recall_filtered")
    program_avg_average_recalls_filtered+=("$average_program_average_recall_filtered")
    program_avg_total_recalls_unfiltered+=("$average_program_total_recall_unfiltered")
    program_avg_average_recalls_unfiltered+=("$average_program_average_recall_unfiltered")
    program_avg_index_times+=("$average_program_index_time")
    program_avg_total_query_times_all+=("$average_program_total_query_time_all")
    program_avg_average_FGS_times_all+=("$average_program_average_FGS_time_all")
    program_avg_average_FGS_times_filtered+=("$average_program_average_FGS_time_filtered")
    program_avg_average_FGS_times_unfiltered+=("$average_program_average_FGS_time_unfiltered")
    program_avg_runtimes+=("$average_program_runtime")

    # Save base program runtime
    if [ $p -eq 0 ]; then
        base_program_avg_total_recall_all=$average_program_total_recall_all
        base_program_avg_average_recall_all=$average_program_average_recall_all
        base_program_avg_total_recall_filtered=$average_program_total_recall_filtered
        base_program_avg_average_recall_filtered=$average_program_average_recall_filtered
        base_program_avg_total_recall_unfiltered=$average_program_total_recall_unfiltered
        base_program_avg_average_recall_unfiltered=$average_program_average_recall_unfiltered
        base_program_avg_index_time=$average_program_index_time
        base_program_avg_total_query_time_all=$average_program_total_query_time_all
        base_program_avg_average_FGS_time_all=$average_program_average_FGS_time_all
        base_program_avg_average_FGS_time_filtered=$average_program_average_FGS_time_filtered
        base_program_avg_average_FGS_time_unfiltered=$average_program_average_FGS_time_unfiltered
        base_program_avg_runtime=$average_program_runtime
    fi


    # Append average runtime to results file
    echo "Average Results for Program $((p+1)):" >> "$RESULTS_FILE"
    echo "Average Total Recall (ALL): $average_program_total_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (ALL): $average_program_average_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (FILTERED): $average_program_total_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (FILTERED): $average_program_average_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (UNFILTERED): $average_program_total_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (UNFILTERED): $average_program_average_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Index Time: $average_program_index_time seconds" >> "$RESULTS_FILE"
    echo "Average Total Query Time (ALL): $average_program_total_query_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (ALL): $average_program_average_FGS_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (FILTERED): $average_program_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (UNFILTERED): $average_program_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Runtime: $average_program_runtime seconds" >> "$RESULTS_FILE"
    echo >> "$RESULTS_FILE"
done

# Calculate percentage improvement
echo "Performance Comparison:" >> "$RESULTS_FILE"
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    average_total_recall_all="${program_avg_total_recalls_all[p]}"
    average_average_recall_all="${program_avg_average_recalls_all[p]}"
    average_total_recall_filtered="${program_avg_total_recalls_filtered[p]}"
    average_average_recall_filtered="${program_avg_average_recalls_filtered[p]}"
    average_total_recall_unfiltered="${program_avg_total_recalls_unfiltered[p]}"
    average_average_recall_unfiltered="${program_avg_average_recalls_unfiltered[p]}"
    average_index_time="${program_avg_index_times[p]}"
    average_total_query_time_all="${program_avg_total_query_times_all[p]}"
    average_average_FGS_time_all="${program_avg_average_FGS_times_all[p]}"
    average_average_FGS_time_filtered="${program_avg_average_FGS_times_filtered[p]}"
    average_average_FGS_time_unfiltered="${program_avg_average_FGS_times_unfiltered[p]}"
    average_runtime="${program_avg_runtimes[p]}"

    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))):" >> "$RESULTS_FILE"
        echo "Average Total Recall (ALL): $average_total_recall_all %" >> "$RESULTS_FILE"
        echo "Average Average Recall (ALL): $average_average_recall_all %" >> "$RESULTS_FILE"
        echo "Average Total Recall (FILTERED): $average_total_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (FILTERED): $average_average_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Total Recall (UNFILTERED): $average_total_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (UNFILTERED): $average_average_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Index Time: $average_index_time seconds" >> "$RESULTS_FILE"
        echo "Average Total Query Time (ALL): $average_total_query_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (ALL): $average_average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Average Runtime: $average_runtime seconds" >> "$RESULTS_FILE"
    else
        echo >> "$RESULTS_FILE"
        echo "Program $((p+1)):" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_all / $base_program_avg_total_recall_all) * 100" | bc)
        echo "$improvement% better Average Total Recall (ALL) than the Base Program with Average Total Recall (ALL): $average_total_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_all / $base_program_avg_average_recall_all) * 100" | bc)
        echo "$improvement% better Average Average Recall (ALL) than the Base Program with Average Average Recall (ALL): $average_average_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_filtered / $base_program_avg_total_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Total Recall (FILTERED) than the Base Program with Average Total Recall (FILTERED): $average_total_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_filtered / $base_program_avg_average_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Average Recall (FILTERED) than the Base Program with Average Average Recall (FILTERED): $average_average_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_unfiltered / $base_program_avg_total_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Total Recall (UNFILTERED) than the Base Program with Average Total Recall (UNFILTERED): $average_total_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_unfiltered / $base_program_avg_average_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average Recall (UNFILTERED) than the Base Program with Average Average Recall (UNFILTERED): $average_average_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_index_time / $base_program_avg_index_time) * 100" | bc)
        echo "$improvement% faster Average Index Time than the Base Program with Average Index Time: $average_index_time" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_query_time_all / $base_program_avg_total_query_time_all) * 100" | bc)
        echo "$improvement% better Average Total Query Time (ALL) than the Base Program with Average Total Query Time (ALL): $average_total_query_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_all / $base_program_avg_average_FGS_time_all) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (ALL) than the Base Program with Average Average FGS Time (ALL): $average_average_FGS_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_filtered / $base_program_avg_average_FGS_time_filtered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (FILTERED) than the Base Program with Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_unfiltered / $base_program_avg_average_FGS_time_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (UNFILTERED) than the Base Program with Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_runtime / $base_program_avg_runtime) * 100" | bc)
        echo "$improvement% faster Average Runtime than the Base Program with Average Runtime: $average_runtime" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"
echo >> "$RESULTS_FILE"
test_7_finish=$(date +%s)
test_7_time_diff=$(echo "$test_7_finish-test_7_start" | bc)
echo "Test 7 took $test_7_time_diff seconds."
echo

#TEST 8 (parallel_filtered_main different a)

test_8_start=$(date +%s)

# Program to execute
PROGRAMS=( \
"./bin/parallel_filtered_main 100 110 96 0.8 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_filtered_main 100 110 96 1.0 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_filtered_main 100 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_filtered_main 100 110 96 1.4 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_filtered_main 100 110 96 1.6 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_filtered_main 100 110 96 1.8 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_filtered_main 100 110 96 2.0 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
)
RESULTS_FILE="results.txt"
TEMP_FILE="temp_output.txt"
RUNS=5

echo "Test 8 (parallel_filtered_main different a)" >> "$RESULTS_FILE"
echo "Running Test 8/12 (parallel_filtered_main different a)"

# Initialize variables
base_program_avg_total_recall_all=0
base_program_avg_average_recall_all=0
base_program_avg_total_recall_filtered=0
base_program_avg_average_recall_filtered=0
base_program_avg_total_recall_unfiltered=0
base_program_avg_average_recall_unfiltered=0
base_program_avg_index_time=0
base_program_avg_total_query_time_all=0
base_program_avg_average_FGS_time_all=0
base_program_avg_average_FGS_time_filtered=0
base_program_avg_average_FGS_time_unfiltered=0
base_program_avg_runtime=0

program_avg_total_recalls_all=()
program_avg_average_recalls_all=()
program_avg_total_recalls_filtered=()
program_avg_average_recalls_filtered=()
program_avg_total_recalls_unfiltered=()
program_avg_average_recalls_unfiltered=()
program_avg_index_times=()
program_avg_total_query_times_all=()
program_avg_average_FGS_times_all=()
program_avg_average_FGS_times_filtered=()
program_avg_average_FGS_times_unfiltered=()
program_avg_runtimes=()


echo "Run Results:" >> "$RESULTS_FILE"

echo >> "$RESULTS_FILE"

echo "Programs are:" >> "$RESULTS_FILE"

for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"
    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))): $PROGRAM" >> "$RESULTS_FILE"
    else 
        echo "Program $((p+1)): $PROGRAM" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"

# Run each program
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"

    program_total_recall_all_sum=0
    program_average_recall_all_sum=0
    program_total_recall_filtered_sum=0
    program_average_recall_filtered_sum=0
    program_total_recall_unfiltered_sum=0
    program_average_recall_unfiltered_sum=0
    program_index_time_sum=0
    program_total_query_time_all_sum=0
    program_average_FGS_time_all_sum=0
    program_average_FGS_time_filtered_sum=0
    program_average_FGS_time_unfiltered_sum=0
    program_runtime_sum=0

    echo "Running Program $((p+1))/${#PROGRAMS[@]}: $PROGRAM" >> "$RESULTS_FILE"
    echo "Running Program $((p+1))/${#PROGRAMS[@]}..."

    for ((i=1; i<=RUNS; i++)); do
        echo "Running the program (Run $i/$RUNS)..."

        # Run the program and capture output
        $PROGRAM > "$TEMP_FILE"

        # Extract recalls and timing summary using grep and awk
        total_recall_all=$(awk '/Total recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_all=$(awk '/Average recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_filtered=$(awk '/Total recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_filtered=$(awk '/Average recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_unfiltered=$(awk '/Total recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_unfiltered=$(awk '/Average recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        index_time=$(awk '/Index build time/ {print $(NF-4)}' "$TEMP_FILE")
        total_query_time_all=$(awk '/Total time Query calculation took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_all=$(awk '/Average time FilteredGreadySearch took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_filtered=$(awk '/Average time FilteredGreadySearch took for FILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_unfiltered=$(awk '/Average time FilteredGreadySearch took for UNFILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        runtime=$(awk '/Program ran for/ {print $4}' "$TEMP_FILE")

        # Append run results to file
        echo "Run $i:" >> "$RESULTS_FILE"
        echo "Total Recall (ALL): $total_recall_all%" >> "$RESULTS_FILE"
        echo "Average Recall (ALL): $average_recall_all%" >> "$RESULTS_FILE"
        echo "Total Recall (Filtered): $total_recall_filtered%" >> "$RESULTS_FILE"
        echo "Average Recall (Filtered): $average_recall_filtered%" >> "$RESULTS_FILE"
        echo "Total Recall (Unfiltered): $total_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Average Recall (Unfiltered): $average_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Index Time: $index_time seconds" >> "$RESULTS_FILE"
        echo "Total Query Time (ALL): $total_query_time_all seconds" >> "$RESULTS_FILE" 
        echo "Average FGS Time (ALL): $average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Filtered): $average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Unfiltered): $average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Program Runtime: $runtime seconds" >> "$RESULTS_FILE"
        echo >> "$RESULTS_FILE"

        # Accumulate values
        program_total_recall_all_sum=$(echo "scale=12; $program_total_recall_all_sum + $total_recall_all" | bc)
        program_average_recall_all_sum=$(echo "scale=12; $program_average_recall_all_sum + $average_recall_all" | bc)
        program_total_recall_filtered_sum=$(echo "scale=12; $program_total_recall_filtered_sum + $total_recall_filtered" | bc)
        program_average_recall_filtered_sum=$(echo "scale=12; $program_average_recall_filtered_sum + $average_recall_filtered" | bc)
        program_total_recall_unfiltered_sum=$(echo "scale=12; $program_total_recall_unfiltered_sum + $total_recall_unfiltered" | bc)
        program_average_recall_unfiltered_sum=$(echo "scale=12; $program_average_recall_unfiltered_sum + $average_recall_unfiltered" | bc)
        program_index_time_sum=$(echo "scale=12; $program_index_time_sum + $index_time" | bc)
        program_total_query_time_all_sum=$(echo "scale=12; $program_total_query_time_all_sum + $total_query_time_all" | bc)
        program_average_FGS_time_all_sum=$(echo "scale=12; $program_average_FGS_time_all_sum + $average_FGS_time_all" | bc)
        program_average_FGS_time_filtered_sum=$(echo "scale=12; $program_average_FGS_time_filtered_sum + $average_FGS_time_filtered" | bc)
        program_average_FGS_time_unfiltered_sum=$(echo "scale=12; $program_average_FGS_time_unfiltered_sum + $average_FGS_time_unfiltered" | bc)
        program_runtime_sum=$(echo "scale=12; $program_runtime_sum + $runtime" | bc)
    done


    # Calculate average
    average_program_total_recall_all=$(echo "scale=10; $program_total_recall_all_sum / $RUNS" | bc)
    average_program_average_recall_all=$(echo "scale=10; $program_average_recall_all_sum / $RUNS" | bc)
    average_program_total_recall_filtered=$(echo "scale=10; $program_total_recall_filtered_sum / $RUNS" | bc)
    average_program_average_recall_filtered=$(echo "scale=10; $program_average_recall_filtered_sum / $RUNS" | bc)
    average_program_total_recall_unfiltered=$(echo "scale=10; $program_total_recall_unfiltered_sum / $RUNS" | bc)
    average_program_average_recall_unfiltered=$(echo "scale=10; $program_average_recall_unfiltered_sum / $RUNS" | bc)
    average_program_index_time=$(echo "scale=10; $program_index_time_sum / $RUNS" | bc)
    average_program_total_query_time_all=$(echo "scale=10; $program_total_query_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_all=$(echo "scale=10; $program_average_FGS_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_filtered=$(echo "scale=10; $program_average_FGS_time_filtered_sum / $RUNS" | bc)
    average_program_average_FGS_time_unfiltered=$(echo "scale=10; $program_average_FGS_time_unfiltered_sum / $RUNS" | bc)
    average_program_runtime=$(echo "scale=10; $program_runtime_sum / $RUNS" | bc)

    program_avg_total_recalls_all+=("$average_program_total_recall_all")
    program_avg_average_recalls_all+=("$average_program_average_recall_all")
    program_avg_total_recalls_filtered+=("$average_program_total_recall_filtered")
    program_avg_average_recalls_filtered+=("$average_program_average_recall_filtered")
    program_avg_total_recalls_unfiltered+=("$average_program_total_recall_unfiltered")
    program_avg_average_recalls_unfiltered+=("$average_program_average_recall_unfiltered")
    program_avg_index_times+=("$average_program_index_time")
    program_avg_total_query_times_all+=("$average_program_total_query_time_all")
    program_avg_average_FGS_times_all+=("$average_program_average_FGS_time_all")
    program_avg_average_FGS_times_filtered+=("$average_program_average_FGS_time_filtered")
    program_avg_average_FGS_times_unfiltered+=("$average_program_average_FGS_time_unfiltered")
    program_avg_runtimes+=("$average_program_runtime")

    # Save base program runtime
    if [ $p -eq 0 ]; then
        base_program_avg_total_recall_all=$average_program_total_recall_all
        base_program_avg_average_recall_all=$average_program_average_recall_all
        base_program_avg_total_recall_filtered=$average_program_total_recall_filtered
        base_program_avg_average_recall_filtered=$average_program_average_recall_filtered
        base_program_avg_total_recall_unfiltered=$average_program_total_recall_unfiltered
        base_program_avg_average_recall_unfiltered=$average_program_average_recall_unfiltered
        base_program_avg_index_time=$average_program_index_time
        base_program_avg_total_query_time_all=$average_program_total_query_time_all
        base_program_avg_average_FGS_time_all=$average_program_average_FGS_time_all
        base_program_avg_average_FGS_time_filtered=$average_program_average_FGS_time_filtered
        base_program_avg_average_FGS_time_unfiltered=$average_program_average_FGS_time_unfiltered
        base_program_avg_runtime=$average_program_runtime
    fi


    # Append average runtime to results file
    echo "Average Results for Program $((p+1)):" >> "$RESULTS_FILE"
    echo "Average Total Recall (ALL): $average_program_total_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (ALL): $average_program_average_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (FILTERED): $average_program_total_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (FILTERED): $average_program_average_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (UNFILTERED): $average_program_total_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (UNFILTERED): $average_program_average_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Index Time: $average_program_index_time seconds" >> "$RESULTS_FILE"
    echo "Average Total Query Time (ALL): $average_program_total_query_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (ALL): $average_program_average_FGS_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (FILTERED): $average_program_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (UNFILTERED): $average_program_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Runtime: $average_program_runtime seconds" >> "$RESULTS_FILE"
    echo >> "$RESULTS_FILE"
done

# Calculate percentage improvement
echo "Performance Comparison:" >> "$RESULTS_FILE"
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    average_total_recall_all="${program_avg_total_recalls_all[p]}"
    average_average_recall_all="${program_avg_average_recalls_all[p]}"
    average_total_recall_filtered="${program_avg_total_recalls_filtered[p]}"
    average_average_recall_filtered="${program_avg_average_recalls_filtered[p]}"
    average_total_recall_unfiltered="${program_avg_total_recalls_unfiltered[p]}"
    average_average_recall_unfiltered="${program_avg_average_recalls_unfiltered[p]}"
    average_index_time="${program_avg_index_times[p]}"
    average_total_query_time_all="${program_avg_total_query_times_all[p]}"
    average_average_FGS_time_all="${program_avg_average_FGS_times_all[p]}"
    average_average_FGS_time_filtered="${program_avg_average_FGS_times_filtered[p]}"
    average_average_FGS_time_unfiltered="${program_avg_average_FGS_times_unfiltered[p]}"
    average_runtime="${program_avg_runtimes[p]}"

    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))):" >> "$RESULTS_FILE"
        echo "Average Total Recall (ALL): $average_total_recall_all %" >> "$RESULTS_FILE"
        echo "Average Average Recall (ALL): $average_average_recall_all %" >> "$RESULTS_FILE"
        echo "Average Total Recall (FILTERED): $average_total_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (FILTERED): $average_average_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Total Recall (UNFILTERED): $average_total_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (UNFILTERED): $average_average_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Index Time: $average_index_time seconds" >> "$RESULTS_FILE"
        echo "Average Total Query Time (ALL): $average_total_query_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (ALL): $average_average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Average Runtime: $average_runtime seconds" >> "$RESULTS_FILE"
    else
        echo >> "$RESULTS_FILE"
        echo "Program $((p+1)):" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_all / $base_program_avg_total_recall_all) * 100" | bc)
        echo "$improvement% better Average Total Recall (ALL) than the Base Program with Average Total Recall (ALL): $average_total_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_all / $base_program_avg_average_recall_all) * 100" | bc)
        echo "$improvement% better Average Average Recall (ALL) than the Base Program with Average Average Recall (ALL): $average_average_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_filtered / $base_program_avg_total_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Total Recall (FILTERED) than the Base Program with Average Total Recall (FILTERED): $average_total_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_filtered / $base_program_avg_average_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Average Recall (FILTERED) than the Base Program with Average Average Recall (FILTERED): $average_average_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_unfiltered / $base_program_avg_total_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Total Recall (UNFILTERED) than the Base Program with Average Total Recall (UNFILTERED): $average_total_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_unfiltered / $base_program_avg_average_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average Recall (UNFILTERED) than the Base Program with Average Average Recall (UNFILTERED): $average_average_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_index_time / $base_program_avg_index_time) * 100" | bc)
        echo "$improvement% faster Average Index Time than the Base Program with Average Index Time: $average_index_time" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_query_time_all / $base_program_avg_total_query_time_all) * 100" | bc)
        echo "$improvement% better Average Total Query Time (ALL) than the Base Program with Average Total Query Time (ALL): $average_total_query_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_all / $base_program_avg_average_FGS_time_all) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (ALL) than the Base Program with Average Average FGS Time (ALL): $average_average_FGS_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_filtered / $base_program_avg_average_FGS_time_filtered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (FILTERED) than the Base Program with Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_unfiltered / $base_program_avg_average_FGS_time_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (UNFILTERED) than the Base Program with Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_runtime / $base_program_avg_runtime) * 100" | bc)
        echo "$improvement% faster Average Runtime than the Base Program with Average Runtime: $average_runtime" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"
echo >> "$RESULTS_FILE"
test_8_finish=$(date +%s)
test_8_time_diff=$(echo "$test_8_finish-test_8_start" | bc)
echo "Test 8 took $test_8_time_diff seconds."
echo

#TEST 9 (parallel_stitched_main different L)

test_9_start=$(date +%s)

# Program to execute
PROGRAMS=( \
"./bin/parallel_stitched_main 100 50 96 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main 100 70 96 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main 100 90 96 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main 100 110 96 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main 100 130 96 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main 100 150 96 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
)
RESULTS_FILE="results.txt"
TEMP_FILE="temp_output.txt"
RUNS=5

echo "Test 9 (parallel_stitched_main different L)" >> "$RESULTS_FILE"
echo "Running Test 9/12 (parallel_stitched_main different L)"

# Initialize variables
base_program_avg_total_recall_all=0
base_program_avg_average_recall_all=0
base_program_avg_total_recall_filtered=0
base_program_avg_average_recall_filtered=0
base_program_avg_total_recall_unfiltered=0
base_program_avg_average_recall_unfiltered=0
base_program_avg_index_time=0
base_program_avg_total_query_time_all=0
base_program_avg_average_FGS_time_all=0
base_program_avg_average_FGS_time_filtered=0
base_program_avg_average_FGS_time_unfiltered=0
base_program_avg_runtime=0

program_avg_total_recalls_all=()
program_avg_average_recalls_all=()
program_avg_total_recalls_filtered=()
program_avg_average_recalls_filtered=()
program_avg_total_recalls_unfiltered=()
program_avg_average_recalls_unfiltered=()
program_avg_index_times=()
program_avg_total_query_times_all=()
program_avg_average_FGS_times_all=()
program_avg_average_FGS_times_filtered=()
program_avg_average_FGS_times_unfiltered=()
program_avg_runtimes=()


echo "Run Results:" >> "$RESULTS_FILE"

echo >> "$RESULTS_FILE"

echo "Programs are:" >> "$RESULTS_FILE"

for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"
    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))): $PROGRAM" >> "$RESULTS_FILE"
    else 
        echo "Program $((p+1)): $PROGRAM" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"

# Run each program
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"

    program_total_recall_all_sum=0
    program_average_recall_all_sum=0
    program_total_recall_filtered_sum=0
    program_average_recall_filtered_sum=0
    program_total_recall_unfiltered_sum=0
    program_average_recall_unfiltered_sum=0
    program_index_time_sum=0
    program_total_query_time_all_sum=0
    program_average_FGS_time_all_sum=0
    program_average_FGS_time_filtered_sum=0
    program_average_FGS_time_unfiltered_sum=0
    program_runtime_sum=0

    echo "Running Program $((p+1))/${#PROGRAMS[@]}: $PROGRAM" >> "$RESULTS_FILE"
    echo "Running Program $((p+1))/${#PROGRAMS[@]}..."

    for ((i=1; i<=RUNS; i++)); do
        echo "Running the program (Run $i/$RUNS)..."

        # Run the program and capture output
        $PROGRAM > "$TEMP_FILE"

        # Extract recalls and timing summary using grep and awk
        total_recall_all=$(awk '/Total recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_all=$(awk '/Average recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_filtered=$(awk '/Total recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_filtered=$(awk '/Average recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_unfiltered=$(awk '/Total recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_unfiltered=$(awk '/Average recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        index_time=$(awk '/Index build time/ {print $(NF-4)}' "$TEMP_FILE")
        total_query_time_all=$(awk '/Total time Query calculation took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_all=$(awk '/Average time FilteredGreadySearch took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_filtered=$(awk '/Average time FilteredGreadySearch took for FILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_unfiltered=$(awk '/Average time FilteredGreadySearch took for UNFILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        runtime=$(awk '/Program ran for/ {print $4}' "$TEMP_FILE")

        # Append run results to file
        echo "Run $i:" >> "$RESULTS_FILE"
        echo "Total Recall (ALL): $total_recall_all%" >> "$RESULTS_FILE"
        echo "Average Recall (ALL): $average_recall_all%" >> "$RESULTS_FILE"
        echo "Total Recall (Filtered): $total_recall_filtered%" >> "$RESULTS_FILE"
        echo "Average Recall (Filtered): $average_recall_filtered%" >> "$RESULTS_FILE"
        echo "Total Recall (Unfiltered): $total_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Average Recall (Unfiltered): $average_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Index Time: $index_time seconds" >> "$RESULTS_FILE"
        echo "Total Query Time (ALL): $total_query_time_all seconds" >> "$RESULTS_FILE" 
        echo "Average FGS Time (ALL): $average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Filtered): $average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Unfiltered): $average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Program Runtime: $runtime seconds" >> "$RESULTS_FILE"
        echo >> "$RESULTS_FILE"

        # Accumulate values
        program_total_recall_all_sum=$(echo "scale=12; $program_total_recall_all_sum + $total_recall_all" | bc)
        program_average_recall_all_sum=$(echo "scale=12; $program_average_recall_all_sum + $average_recall_all" | bc)
        program_total_recall_filtered_sum=$(echo "scale=12; $program_total_recall_filtered_sum + $total_recall_filtered" | bc)
        program_average_recall_filtered_sum=$(echo "scale=12; $program_average_recall_filtered_sum + $average_recall_filtered" | bc)
        program_total_recall_unfiltered_sum=$(echo "scale=12; $program_total_recall_unfiltered_sum + $total_recall_unfiltered" | bc)
        program_average_recall_unfiltered_sum=$(echo "scale=12; $program_average_recall_unfiltered_sum + $average_recall_unfiltered" | bc)
        program_index_time_sum=$(echo "scale=12; $program_index_time_sum + $index_time" | bc)
        program_total_query_time_all_sum=$(echo "scale=12; $program_total_query_time_all_sum + $total_query_time_all" | bc)
        program_average_FGS_time_all_sum=$(echo "scale=12; $program_average_FGS_time_all_sum + $average_FGS_time_all" | bc)
        program_average_FGS_time_filtered_sum=$(echo "scale=12; $program_average_FGS_time_filtered_sum + $average_FGS_time_filtered" | bc)
        program_average_FGS_time_unfiltered_sum=$(echo "scale=12; $program_average_FGS_time_unfiltered_sum + $average_FGS_time_unfiltered" | bc)
        program_runtime_sum=$(echo "scale=12; $program_runtime_sum + $runtime" | bc)
    done


    # Calculate average
    average_program_total_recall_all=$(echo "scale=10; $program_total_recall_all_sum / $RUNS" | bc)
    average_program_average_recall_all=$(echo "scale=10; $program_average_recall_all_sum / $RUNS" | bc)
    average_program_total_recall_filtered=$(echo "scale=10; $program_total_recall_filtered_sum / $RUNS" | bc)
    average_program_average_recall_filtered=$(echo "scale=10; $program_average_recall_filtered_sum / $RUNS" | bc)
    average_program_total_recall_unfiltered=$(echo "scale=10; $program_total_recall_unfiltered_sum / $RUNS" | bc)
    average_program_average_recall_unfiltered=$(echo "scale=10; $program_average_recall_unfiltered_sum / $RUNS" | bc)
    average_program_index_time=$(echo "scale=10; $program_index_time_sum / $RUNS" | bc)
    average_program_total_query_time_all=$(echo "scale=10; $program_total_query_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_all=$(echo "scale=10; $program_average_FGS_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_filtered=$(echo "scale=10; $program_average_FGS_time_filtered_sum / $RUNS" | bc)
    average_program_average_FGS_time_unfiltered=$(echo "scale=10; $program_average_FGS_time_unfiltered_sum / $RUNS" | bc)
    average_program_runtime=$(echo "scale=10; $program_runtime_sum / $RUNS" | bc)

    program_avg_total_recalls_all+=("$average_program_total_recall_all")
    program_avg_average_recalls_all+=("$average_program_average_recall_all")
    program_avg_total_recalls_filtered+=("$average_program_total_recall_filtered")
    program_avg_average_recalls_filtered+=("$average_program_average_recall_filtered")
    program_avg_total_recalls_unfiltered+=("$average_program_total_recall_unfiltered")
    program_avg_average_recalls_unfiltered+=("$average_program_average_recall_unfiltered")
    program_avg_index_times+=("$average_program_index_time")
    program_avg_total_query_times_all+=("$average_program_total_query_time_all")
    program_avg_average_FGS_times_all+=("$average_program_average_FGS_time_all")
    program_avg_average_FGS_times_filtered+=("$average_program_average_FGS_time_filtered")
    program_avg_average_FGS_times_unfiltered+=("$average_program_average_FGS_time_unfiltered")
    program_avg_runtimes+=("$average_program_runtime")

    # Save base program runtime
    if [ $p -eq 0 ]; then
        base_program_avg_total_recall_all=$average_program_total_recall_all
        base_program_avg_average_recall_all=$average_program_average_recall_all
        base_program_avg_total_recall_filtered=$average_program_total_recall_filtered
        base_program_avg_average_recall_filtered=$average_program_average_recall_filtered
        base_program_avg_total_recall_unfiltered=$average_program_total_recall_unfiltered
        base_program_avg_average_recall_unfiltered=$average_program_average_recall_unfiltered
        base_program_avg_index_time=$average_program_index_time
        base_program_avg_total_query_time_all=$average_program_total_query_time_all
        base_program_avg_average_FGS_time_all=$average_program_average_FGS_time_all
        base_program_avg_average_FGS_time_filtered=$average_program_average_FGS_time_filtered
        base_program_avg_average_FGS_time_unfiltered=$average_program_average_FGS_time_unfiltered
        base_program_avg_runtime=$average_program_runtime
    fi


    # Append average runtime to results file
    echo "Average Results for Program $((p+1)):" >> "$RESULTS_FILE"
    echo "Average Total Recall (ALL): $average_program_total_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (ALL): $average_program_average_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (FILTERED): $average_program_total_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (FILTERED): $average_program_average_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (UNFILTERED): $average_program_total_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (UNFILTERED): $average_program_average_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Index Time: $average_program_index_time seconds" >> "$RESULTS_FILE"
    echo "Average Total Query Time (ALL): $average_program_total_query_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (ALL): $average_program_average_FGS_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (FILTERED): $average_program_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (UNFILTERED): $average_program_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Runtime: $average_program_runtime seconds" >> "$RESULTS_FILE"
    echo >> "$RESULTS_FILE"
done

# Calculate percentage improvement
echo "Performance Comparison:" >> "$RESULTS_FILE"
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    average_total_recall_all="${program_avg_total_recalls_all[p]}"
    average_average_recall_all="${program_avg_average_recalls_all[p]}"
    average_total_recall_filtered="${program_avg_total_recalls_filtered[p]}"
    average_average_recall_filtered="${program_avg_average_recalls_filtered[p]}"
    average_total_recall_unfiltered="${program_avg_total_recalls_unfiltered[p]}"
    average_average_recall_unfiltered="${program_avg_average_recalls_unfiltered[p]}"
    average_index_time="${program_avg_index_times[p]}"
    average_total_query_time_all="${program_avg_total_query_times_all[p]}"
    average_average_FGS_time_all="${program_avg_average_FGS_times_all[p]}"
    average_average_FGS_time_filtered="${program_avg_average_FGS_times_filtered[p]}"
    average_average_FGS_time_unfiltered="${program_avg_average_FGS_times_unfiltered[p]}"
    average_runtime="${program_avg_runtimes[p]}"

    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))):" >> "$RESULTS_FILE"
        echo "Average Total Recall (ALL): $average_total_recall_all %" >> "$RESULTS_FILE"
        echo "Average Average Recall (ALL): $average_average_recall_all %" >> "$RESULTS_FILE"
        echo "Average Total Recall (FILTERED): $average_total_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (FILTERED): $average_average_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Total Recall (UNFILTERED): $average_total_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (UNFILTERED): $average_average_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Index Time: $average_index_time seconds" >> "$RESULTS_FILE"
        echo "Average Total Query Time (ALL): $average_total_query_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (ALL): $average_average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Average Runtime: $average_runtime seconds" >> "$RESULTS_FILE"
    else
        echo >> "$RESULTS_FILE"
        echo "Program $((p+1)):" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_all / $base_program_avg_total_recall_all) * 100" | bc)
        echo "$improvement% better Average Total Recall (ALL) than the Base Program with Average Total Recall (ALL): $average_total_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_all / $base_program_avg_average_recall_all) * 100" | bc)
        echo "$improvement% better Average Average Recall (ALL) than the Base Program with Average Average Recall (ALL): $average_average_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_filtered / $base_program_avg_total_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Total Recall (FILTERED) than the Base Program with Average Total Recall (FILTERED): $average_total_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_filtered / $base_program_avg_average_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Average Recall (FILTERED) than the Base Program with Average Average Recall (FILTERED): $average_average_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_unfiltered / $base_program_avg_total_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Total Recall (UNFILTERED) than the Base Program with Average Total Recall (UNFILTERED): $average_total_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_unfiltered / $base_program_avg_average_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average Recall (UNFILTERED) than the Base Program with Average Average Recall (UNFILTERED): $average_average_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_index_time / $base_program_avg_index_time) * 100" | bc)
        echo "$improvement% faster Average Index Time than the Base Program with Average Index Time: $average_index_time" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_query_time_all / $base_program_avg_total_query_time_all) * 100" | bc)
        echo "$improvement% better Average Total Query Time (ALL) than the Base Program with Average Total Query Time (ALL): $average_total_query_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_all / $base_program_avg_average_FGS_time_all) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (ALL) than the Base Program with Average Average FGS Time (ALL): $average_average_FGS_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_filtered / $base_program_avg_average_FGS_time_filtered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (FILTERED) than the Base Program with Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_unfiltered / $base_program_avg_average_FGS_time_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (UNFILTERED) than the Base Program with Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_runtime / $base_program_avg_runtime) * 100" | bc)
        echo "$improvement% faster Average Runtime than the Base Program with Average Runtime: $average_runtime" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"
echo >> "$RESULTS_FILE"
test_9_finish=$(date +%s)
test_9_time_diff=$(echo "$test_9_finish-test_9_start" | bc)
echo "Test 9 took $test_9_time_diff seconds."
echo

#TEST 10 (parallel_stitched_main different k)

test_10_start=$(date +%s)

# Program to execute
PROGRAMS=( \
"./bin/parallel_stitched_main 20 110 96 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=20.bin" \
"./bin/parallel_stitched_main 40 110 96 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=40.bin" \
"./bin/parallel_stitched_main 60 110 96 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=60.bin" \
"./bin/parallel_stitched_main 80 110 96 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=80.bin" \
"./bin/parallel_stitched_main 100 110 96 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
)
RESULTS_FILE="results.txt"
TEMP_FILE="temp_output.txt"
RUNS=5

echo "Test 10 (parallel_stitched_main different k)" >> "$RESULTS_FILE"
echo "Running Test 10/12 (parallel_stitched_main different k)"

# Initialize variables
base_program_avg_total_recall_all=0
base_program_avg_average_recall_all=0
base_program_avg_total_recall_filtered=0
base_program_avg_average_recall_filtered=0
base_program_avg_total_recall_unfiltered=0
base_program_avg_average_recall_unfiltered=0
base_program_avg_index_time=0
base_program_avg_total_query_time_all=0
base_program_avg_average_FGS_time_all=0
base_program_avg_average_FGS_time_filtered=0
base_program_avg_average_FGS_time_unfiltered=0
base_program_avg_runtime=0

program_avg_total_recalls_all=()
program_avg_average_recalls_all=()
program_avg_total_recalls_filtered=()
program_avg_average_recalls_filtered=()
program_avg_total_recalls_unfiltered=()
program_avg_average_recalls_unfiltered=()
program_avg_index_times=()
program_avg_total_query_times_all=()
program_avg_average_FGS_times_all=()
program_avg_average_FGS_times_filtered=()
program_avg_average_FGS_times_unfiltered=()
program_avg_runtimes=()


echo "Run Results:" >> "$RESULTS_FILE"

echo >> "$RESULTS_FILE"

echo "Programs are:" >> "$RESULTS_FILE"

for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"
    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))): $PROGRAM" >> "$RESULTS_FILE"
    else 
        echo "Program $((p+1)): $PROGRAM" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"

# Run each program
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"

    program_total_recall_all_sum=0
    program_average_recall_all_sum=0
    program_total_recall_filtered_sum=0
    program_average_recall_filtered_sum=0
    program_total_recall_unfiltered_sum=0
    program_average_recall_unfiltered_sum=0
    program_index_time_sum=0
    program_total_query_time_all_sum=0
    program_average_FGS_time_all_sum=0
    program_average_FGS_time_filtered_sum=0
    program_average_FGS_time_unfiltered_sum=0
    program_runtime_sum=0

    echo "Running Program $((p+1))/${#PROGRAMS[@]}: $PROGRAM" >> "$RESULTS_FILE"
    echo "Running Program $((p+1))/${#PROGRAMS[@]}..."

    for ((i=1; i<=RUNS; i++)); do
        echo "Running the program (Run $i/$RUNS)..."

        # Run the program and capture output
        $PROGRAM > "$TEMP_FILE"

        # Extract recalls and timing summary using grep and awk
        total_recall_all=$(awk '/Total recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_all=$(awk '/Average recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_filtered=$(awk '/Total recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_filtered=$(awk '/Average recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_unfiltered=$(awk '/Total recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_unfiltered=$(awk '/Average recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        index_time=$(awk '/Index build time/ {print $(NF-4)}' "$TEMP_FILE")
        total_query_time_all=$(awk '/Total time Query calculation took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_all=$(awk '/Average time FilteredGreadySearch took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_filtered=$(awk '/Average time FilteredGreadySearch took for FILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_unfiltered=$(awk '/Average time FilteredGreadySearch took for UNFILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        runtime=$(awk '/Program ran for/ {print $4}' "$TEMP_FILE")

        # Append run results to file
        echo "Run $i:" >> "$RESULTS_FILE"
        echo "Total Recall (ALL): $total_recall_all%" >> "$RESULTS_FILE"
        echo "Average Recall (ALL): $average_recall_all%" >> "$RESULTS_FILE"
        echo "Total Recall (Filtered): $total_recall_filtered%" >> "$RESULTS_FILE"
        echo "Average Recall (Filtered): $average_recall_filtered%" >> "$RESULTS_FILE"
        echo "Total Recall (Unfiltered): $total_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Average Recall (Unfiltered): $average_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Index Time: $index_time seconds" >> "$RESULTS_FILE"
        echo "Total Query Time (ALL): $total_query_time_all seconds" >> "$RESULTS_FILE" 
        echo "Average FGS Time (ALL): $average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Filtered): $average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Unfiltered): $average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Program Runtime: $runtime seconds" >> "$RESULTS_FILE"
        echo >> "$RESULTS_FILE"

        # Accumulate values
        program_total_recall_all_sum=$(echo "scale=12; $program_total_recall_all_sum + $total_recall_all" | bc)
        program_average_recall_all_sum=$(echo "scale=12; $program_average_recall_all_sum + $average_recall_all" | bc)
        program_total_recall_filtered_sum=$(echo "scale=12; $program_total_recall_filtered_sum + $total_recall_filtered" | bc)
        program_average_recall_filtered_sum=$(echo "scale=12; $program_average_recall_filtered_sum + $average_recall_filtered" | bc)
        program_total_recall_unfiltered_sum=$(echo "scale=12; $program_total_recall_unfiltered_sum + $total_recall_unfiltered" | bc)
        program_average_recall_unfiltered_sum=$(echo "scale=12; $program_average_recall_unfiltered_sum + $average_recall_unfiltered" | bc)
        program_index_time_sum=$(echo "scale=12; $program_index_time_sum + $index_time" | bc)
        program_total_query_time_all_sum=$(echo "scale=12; $program_total_query_time_all_sum + $total_query_time_all" | bc)
        program_average_FGS_time_all_sum=$(echo "scale=12; $program_average_FGS_time_all_sum + $average_FGS_time_all" | bc)
        program_average_FGS_time_filtered_sum=$(echo "scale=12; $program_average_FGS_time_filtered_sum + $average_FGS_time_filtered" | bc)
        program_average_FGS_time_unfiltered_sum=$(echo "scale=12; $program_average_FGS_time_unfiltered_sum + $average_FGS_time_unfiltered" | bc)
        program_runtime_sum=$(echo "scale=12; $program_runtime_sum + $runtime" | bc)
    done


    # Calculate average
    average_program_total_recall_all=$(echo "scale=10; $program_total_recall_all_sum / $RUNS" | bc)
    average_program_average_recall_all=$(echo "scale=10; $program_average_recall_all_sum / $RUNS" | bc)
    average_program_total_recall_filtered=$(echo "scale=10; $program_total_recall_filtered_sum / $RUNS" | bc)
    average_program_average_recall_filtered=$(echo "scale=10; $program_average_recall_filtered_sum / $RUNS" | bc)
    average_program_total_recall_unfiltered=$(echo "scale=10; $program_total_recall_unfiltered_sum / $RUNS" | bc)
    average_program_average_recall_unfiltered=$(echo "scale=10; $program_average_recall_unfiltered_sum / $RUNS" | bc)
    average_program_index_time=$(echo "scale=10; $program_index_time_sum / $RUNS" | bc)
    average_program_total_query_time_all=$(echo "scale=10; $program_total_query_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_all=$(echo "scale=10; $program_average_FGS_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_filtered=$(echo "scale=10; $program_average_FGS_time_filtered_sum / $RUNS" | bc)
    average_program_average_FGS_time_unfiltered=$(echo "scale=10; $program_average_FGS_time_unfiltered_sum / $RUNS" | bc)
    average_program_runtime=$(echo "scale=10; $program_runtime_sum / $RUNS" | bc)

    program_avg_total_recalls_all+=("$average_program_total_recall_all")
    program_avg_average_recalls_all+=("$average_program_average_recall_all")
    program_avg_total_recalls_filtered+=("$average_program_total_recall_filtered")
    program_avg_average_recalls_filtered+=("$average_program_average_recall_filtered")
    program_avg_total_recalls_unfiltered+=("$average_program_total_recall_unfiltered")
    program_avg_average_recalls_unfiltered+=("$average_program_average_recall_unfiltered")
    program_avg_index_times+=("$average_program_index_time")
    program_avg_total_query_times_all+=("$average_program_total_query_time_all")
    program_avg_average_FGS_times_all+=("$average_program_average_FGS_time_all")
    program_avg_average_FGS_times_filtered+=("$average_program_average_FGS_time_filtered")
    program_avg_average_FGS_times_unfiltered+=("$average_program_average_FGS_time_unfiltered")
    program_avg_runtimes+=("$average_program_runtime")

    # Save base program runtime
    if [ $p -eq 0 ]; then
        base_program_avg_total_recall_all=$average_program_total_recall_all
        base_program_avg_average_recall_all=$average_program_average_recall_all
        base_program_avg_total_recall_filtered=$average_program_total_recall_filtered
        base_program_avg_average_recall_filtered=$average_program_average_recall_filtered
        base_program_avg_total_recall_unfiltered=$average_program_total_recall_unfiltered
        base_program_avg_average_recall_unfiltered=$average_program_average_recall_unfiltered
        base_program_avg_index_time=$average_program_index_time
        base_program_avg_total_query_time_all=$average_program_total_query_time_all
        base_program_avg_average_FGS_time_all=$average_program_average_FGS_time_all
        base_program_avg_average_FGS_time_filtered=$average_program_average_FGS_time_filtered
        base_program_avg_average_FGS_time_unfiltered=$average_program_average_FGS_time_unfiltered
        base_program_avg_runtime=$average_program_runtime
    fi


    # Append average runtime to results file
    echo "Average Results for Program $((p+1)):" >> "$RESULTS_FILE"
    echo "Average Total Recall (ALL): $average_program_total_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (ALL): $average_program_average_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (FILTERED): $average_program_total_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (FILTERED): $average_program_average_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (UNFILTERED): $average_program_total_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (UNFILTERED): $average_program_average_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Index Time: $average_program_index_time seconds" >> "$RESULTS_FILE"
    echo "Average Total Query Time (ALL): $average_program_total_query_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (ALL): $average_program_average_FGS_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (FILTERED): $average_program_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (UNFILTERED): $average_program_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Runtime: $average_program_runtime seconds" >> "$RESULTS_FILE"
    echo >> "$RESULTS_FILE"
done

# Calculate percentage improvement
echo "Performance Comparison:" >> "$RESULTS_FILE"
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    average_total_recall_all="${program_avg_total_recalls_all[p]}"
    average_average_recall_all="${program_avg_average_recalls_all[p]}"
    average_total_recall_filtered="${program_avg_total_recalls_filtered[p]}"
    average_average_recall_filtered="${program_avg_average_recalls_filtered[p]}"
    average_total_recall_unfiltered="${program_avg_total_recalls_unfiltered[p]}"
    average_average_recall_unfiltered="${program_avg_average_recalls_unfiltered[p]}"
    average_index_time="${program_avg_index_times[p]}"
    average_total_query_time_all="${program_avg_total_query_times_all[p]}"
    average_average_FGS_time_all="${program_avg_average_FGS_times_all[p]}"
    average_average_FGS_time_filtered="${program_avg_average_FGS_times_filtered[p]}"
    average_average_FGS_time_unfiltered="${program_avg_average_FGS_times_unfiltered[p]}"
    average_runtime="${program_avg_runtimes[p]}"

    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))):" >> "$RESULTS_FILE"
        echo "Average Total Recall (ALL): $average_total_recall_all %" >> "$RESULTS_FILE"
        echo "Average Average Recall (ALL): $average_average_recall_all %" >> "$RESULTS_FILE"
        echo "Average Total Recall (FILTERED): $average_total_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (FILTERED): $average_average_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Total Recall (UNFILTERED): $average_total_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (UNFILTERED): $average_average_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Index Time: $average_index_time seconds" >> "$RESULTS_FILE"
        echo "Average Total Query Time (ALL): $average_total_query_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (ALL): $average_average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Average Runtime: $average_runtime seconds" >> "$RESULTS_FILE"
    else
        echo >> "$RESULTS_FILE"
        echo "Program $((p+1)):" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_all / $base_program_avg_total_recall_all) * 100" | bc)
        echo "$improvement% better Average Total Recall (ALL) than the Base Program with Average Total Recall (ALL): $average_total_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_all / $base_program_avg_average_recall_all) * 100" | bc)
        echo "$improvement% better Average Average Recall (ALL) than the Base Program with Average Average Recall (ALL): $average_average_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_filtered / $base_program_avg_total_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Total Recall (FILTERED) than the Base Program with Average Total Recall (FILTERED): $average_total_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_filtered / $base_program_avg_average_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Average Recall (FILTERED) than the Base Program with Average Average Recall (FILTERED): $average_average_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_unfiltered / $base_program_avg_total_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Total Recall (UNFILTERED) than the Base Program with Average Total Recall (UNFILTERED): $average_total_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_unfiltered / $base_program_avg_average_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average Recall (UNFILTERED) than the Base Program with Average Average Recall (UNFILTERED): $average_average_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_index_time / $base_program_avg_index_time) * 100" | bc)
        echo "$improvement% faster Average Index Time than the Base Program with Average Index Time: $average_index_time" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_query_time_all / $base_program_avg_total_query_time_all) * 100" | bc)
        echo "$improvement% better Average Total Query Time (ALL) than the Base Program with Average Total Query Time (ALL): $average_total_query_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_all / $base_program_avg_average_FGS_time_all) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (ALL) than the Base Program with Average Average FGS Time (ALL): $average_average_FGS_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_filtered / $base_program_avg_average_FGS_time_filtered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (FILTERED) than the Base Program with Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_unfiltered / $base_program_avg_average_FGS_time_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (UNFILTERED) than the Base Program with Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_runtime / $base_program_avg_runtime) * 100" | bc)
        echo "$improvement% faster Average Runtime than the Base Program with Average Runtime: $average_runtime" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"
echo >> "$RESULTS_FILE"
test_10_finish=$(date +%s)
test_10_time_diff=$(echo "$test_10_finish-test_10_start" | bc)
echo "Test 10 took $test_10_time_diff seconds."
echo

#TEST 11 (parallel_stitched_main different R)

test_11_start=$(date +%s)

# Program to execute
PROGRAMS=( \
"./bin/parallel_stitched_main 100 110 50 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main 100 110 70 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main 100 110 90 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main 100 110 110 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main 100 110 130 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main 100 110 150 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
)
RESULTS_FILE="results.txt"
TEMP_FILE="temp_output.txt"
RUNS=5

echo "Test 11 (parallel_stitched_main different R)" >> "$RESULTS_FILE"
echo "Running Test 11/12 (parallel_stitched_main different R)"

# Initialize variables
base_program_avg_total_recall_all=0
base_program_avg_average_recall_all=0
base_program_avg_total_recall_filtered=0
base_program_avg_average_recall_filtered=0
base_program_avg_total_recall_unfiltered=0
base_program_avg_average_recall_unfiltered=0
base_program_avg_index_time=0
base_program_avg_total_query_time_all=0
base_program_avg_average_FGS_time_all=0
base_program_avg_average_FGS_time_filtered=0
base_program_avg_average_FGS_time_unfiltered=0
base_program_avg_runtime=0

program_avg_total_recalls_all=()
program_avg_average_recalls_all=()
program_avg_total_recalls_filtered=()
program_avg_average_recalls_filtered=()
program_avg_total_recalls_unfiltered=()
program_avg_average_recalls_unfiltered=()
program_avg_index_times=()
program_avg_total_query_times_all=()
program_avg_average_FGS_times_all=()
program_avg_average_FGS_times_filtered=()
program_avg_average_FGS_times_unfiltered=()
program_avg_runtimes=()


echo "Run Results:" >> "$RESULTS_FILE"

echo >> "$RESULTS_FILE"

echo "Programs are:" >> "$RESULTS_FILE"

for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"
    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))): $PROGRAM" >> "$RESULTS_FILE"
    else 
        echo "Program $((p+1)): $PROGRAM" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"

# Run each program
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"

    program_total_recall_all_sum=0
    program_average_recall_all_sum=0
    program_total_recall_filtered_sum=0
    program_average_recall_filtered_sum=0
    program_total_recall_unfiltered_sum=0
    program_average_recall_unfiltered_sum=0
    program_index_time_sum=0
    program_total_query_time_all_sum=0
    program_average_FGS_time_all_sum=0
    program_average_FGS_time_filtered_sum=0
    program_average_FGS_time_unfiltered_sum=0
    program_runtime_sum=0

    echo "Running Program $((p+1))/${#PROGRAMS[@]}: $PROGRAM" >> "$RESULTS_FILE"
    echo "Running Program $((p+1))/${#PROGRAMS[@]}..."

    for ((i=1; i<=RUNS; i++)); do
        echo "Running the program (Run $i/$RUNS)..."

        # Run the program and capture output
        $PROGRAM > "$TEMP_FILE"

        # Extract recalls and timing summary using grep and awk
        total_recall_all=$(awk '/Total recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_all=$(awk '/Average recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_filtered=$(awk '/Total recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_filtered=$(awk '/Average recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_unfiltered=$(awk '/Total recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_unfiltered=$(awk '/Average recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        index_time=$(awk '/Index build time/ {print $(NF-4)}' "$TEMP_FILE")
        total_query_time_all=$(awk '/Total time Query calculation took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_all=$(awk '/Average time FilteredGreadySearch took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_filtered=$(awk '/Average time FilteredGreadySearch took for FILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_unfiltered=$(awk '/Average time FilteredGreadySearch took for UNFILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        runtime=$(awk '/Program ran for/ {print $4}' "$TEMP_FILE")

        # Append run results to file
        echo "Run $i:" >> "$RESULTS_FILE"
        echo "Total Recall (ALL): $total_recall_all%" >> "$RESULTS_FILE"
        echo "Average Recall (ALL): $average_recall_all%" >> "$RESULTS_FILE"
        echo "Total Recall (Filtered): $total_recall_filtered%" >> "$RESULTS_FILE"
        echo "Average Recall (Filtered): $average_recall_filtered%" >> "$RESULTS_FILE"
        echo "Total Recall (Unfiltered): $total_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Average Recall (Unfiltered): $average_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Index Time: $index_time seconds" >> "$RESULTS_FILE"
        echo "Total Query Time (ALL): $total_query_time_all seconds" >> "$RESULTS_FILE" 
        echo "Average FGS Time (ALL): $average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Filtered): $average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Unfiltered): $average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Program Runtime: $runtime seconds" >> "$RESULTS_FILE"
        echo >> "$RESULTS_FILE"

        # Accumulate values
        program_total_recall_all_sum=$(echo "scale=12; $program_total_recall_all_sum + $total_recall_all" | bc)
        program_average_recall_all_sum=$(echo "scale=12; $program_average_recall_all_sum + $average_recall_all" | bc)
        program_total_recall_filtered_sum=$(echo "scale=12; $program_total_recall_filtered_sum + $total_recall_filtered" | bc)
        program_average_recall_filtered_sum=$(echo "scale=12; $program_average_recall_filtered_sum + $average_recall_filtered" | bc)
        program_total_recall_unfiltered_sum=$(echo "scale=12; $program_total_recall_unfiltered_sum + $total_recall_unfiltered" | bc)
        program_average_recall_unfiltered_sum=$(echo "scale=12; $program_average_recall_unfiltered_sum + $average_recall_unfiltered" | bc)
        program_index_time_sum=$(echo "scale=12; $program_index_time_sum + $index_time" | bc)
        program_total_query_time_all_sum=$(echo "scale=12; $program_total_query_time_all_sum + $total_query_time_all" | bc)
        program_average_FGS_time_all_sum=$(echo "scale=12; $program_average_FGS_time_all_sum + $average_FGS_time_all" | bc)
        program_average_FGS_time_filtered_sum=$(echo "scale=12; $program_average_FGS_time_filtered_sum + $average_FGS_time_filtered" | bc)
        program_average_FGS_time_unfiltered_sum=$(echo "scale=12; $program_average_FGS_time_unfiltered_sum + $average_FGS_time_unfiltered" | bc)
        program_runtime_sum=$(echo "scale=12; $program_runtime_sum + $runtime" | bc)
    done


    # Calculate average
    average_program_total_recall_all=$(echo "scale=10; $program_total_recall_all_sum / $RUNS" | bc)
    average_program_average_recall_all=$(echo "scale=10; $program_average_recall_all_sum / $RUNS" | bc)
    average_program_total_recall_filtered=$(echo "scale=10; $program_total_recall_filtered_sum / $RUNS" | bc)
    average_program_average_recall_filtered=$(echo "scale=10; $program_average_recall_filtered_sum / $RUNS" | bc)
    average_program_total_recall_unfiltered=$(echo "scale=10; $program_total_recall_unfiltered_sum / $RUNS" | bc)
    average_program_average_recall_unfiltered=$(echo "scale=10; $program_average_recall_unfiltered_sum / $RUNS" | bc)
    average_program_index_time=$(echo "scale=10; $program_index_time_sum / $RUNS" | bc)
    average_program_total_query_time_all=$(echo "scale=10; $program_total_query_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_all=$(echo "scale=10; $program_average_FGS_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_filtered=$(echo "scale=10; $program_average_FGS_time_filtered_sum / $RUNS" | bc)
    average_program_average_FGS_time_unfiltered=$(echo "scale=10; $program_average_FGS_time_unfiltered_sum / $RUNS" | bc)
    average_program_runtime=$(echo "scale=10; $program_runtime_sum / $RUNS" | bc)

    program_avg_total_recalls_all+=("$average_program_total_recall_all")
    program_avg_average_recalls_all+=("$average_program_average_recall_all")
    program_avg_total_recalls_filtered+=("$average_program_total_recall_filtered")
    program_avg_average_recalls_filtered+=("$average_program_average_recall_filtered")
    program_avg_total_recalls_unfiltered+=("$average_program_total_recall_unfiltered")
    program_avg_average_recalls_unfiltered+=("$average_program_average_recall_unfiltered")
    program_avg_index_times+=("$average_program_index_time")
    program_avg_total_query_times_all+=("$average_program_total_query_time_all")
    program_avg_average_FGS_times_all+=("$average_program_average_FGS_time_all")
    program_avg_average_FGS_times_filtered+=("$average_program_average_FGS_time_filtered")
    program_avg_average_FGS_times_unfiltered+=("$average_program_average_FGS_time_unfiltered")
    program_avg_runtimes+=("$average_program_runtime")

    # Save base program runtime
    if [ $p -eq 0 ]; then
        base_program_avg_total_recall_all=$average_program_total_recall_all
        base_program_avg_average_recall_all=$average_program_average_recall_all
        base_program_avg_total_recall_filtered=$average_program_total_recall_filtered
        base_program_avg_average_recall_filtered=$average_program_average_recall_filtered
        base_program_avg_total_recall_unfiltered=$average_program_total_recall_unfiltered
        base_program_avg_average_recall_unfiltered=$average_program_average_recall_unfiltered
        base_program_avg_index_time=$average_program_index_time
        base_program_avg_total_query_time_all=$average_program_total_query_time_all
        base_program_avg_average_FGS_time_all=$average_program_average_FGS_time_all
        base_program_avg_average_FGS_time_filtered=$average_program_average_FGS_time_filtered
        base_program_avg_average_FGS_time_unfiltered=$average_program_average_FGS_time_unfiltered
        base_program_avg_runtime=$average_program_runtime
    fi


    # Append average runtime to results file
    echo "Average Results for Program $((p+1)):" >> "$RESULTS_FILE"
    echo "Average Total Recall (ALL): $average_program_total_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (ALL): $average_program_average_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (FILTERED): $average_program_total_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (FILTERED): $average_program_average_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (UNFILTERED): $average_program_total_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (UNFILTERED): $average_program_average_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Index Time: $average_program_index_time seconds" >> "$RESULTS_FILE"
    echo "Average Total Query Time (ALL): $average_program_total_query_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (ALL): $average_program_average_FGS_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (FILTERED): $average_program_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (UNFILTERED): $average_program_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Runtime: $average_program_runtime seconds" >> "$RESULTS_FILE"
    echo >> "$RESULTS_FILE"
done

# Calculate percentage improvement
echo "Performance Comparison:" >> "$RESULTS_FILE"
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    average_total_recall_all="${program_avg_total_recalls_all[p]}"
    average_average_recall_all="${program_avg_average_recalls_all[p]}"
    average_total_recall_filtered="${program_avg_total_recalls_filtered[p]}"
    average_average_recall_filtered="${program_avg_average_recalls_filtered[p]}"
    average_total_recall_unfiltered="${program_avg_total_recalls_unfiltered[p]}"
    average_average_recall_unfiltered="${program_avg_average_recalls_unfiltered[p]}"
    average_index_time="${program_avg_index_times[p]}"
    average_total_query_time_all="${program_avg_total_query_times_all[p]}"
    average_average_FGS_time_all="${program_avg_average_FGS_times_all[p]}"
    average_average_FGS_time_filtered="${program_avg_average_FGS_times_filtered[p]}"
    average_average_FGS_time_unfiltered="${program_avg_average_FGS_times_unfiltered[p]}"
    average_runtime="${program_avg_runtimes[p]}"

    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))):" >> "$RESULTS_FILE"
        echo "Average Total Recall (ALL): $average_total_recall_all %" >> "$RESULTS_FILE"
        echo "Average Average Recall (ALL): $average_average_recall_all %" >> "$RESULTS_FILE"
        echo "Average Total Recall (FILTERED): $average_total_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (FILTERED): $average_average_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Total Recall (UNFILTERED): $average_total_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (UNFILTERED): $average_average_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Index Time: $average_index_time seconds" >> "$RESULTS_FILE"
        echo "Average Total Query Time (ALL): $average_total_query_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (ALL): $average_average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Average Runtime: $average_runtime seconds" >> "$RESULTS_FILE"
    else
        echo >> "$RESULTS_FILE"
        echo "Program $((p+1)):" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_all / $base_program_avg_total_recall_all) * 100" | bc)
        echo "$improvement% better Average Total Recall (ALL) than the Base Program with Average Total Recall (ALL): $average_total_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_all / $base_program_avg_average_recall_all) * 100" | bc)
        echo "$improvement% better Average Average Recall (ALL) than the Base Program with Average Average Recall (ALL): $average_average_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_filtered / $base_program_avg_total_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Total Recall (FILTERED) than the Base Program with Average Total Recall (FILTERED): $average_total_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_filtered / $base_program_avg_average_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Average Recall (FILTERED) than the Base Program with Average Average Recall (FILTERED): $average_average_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_unfiltered / $base_program_avg_total_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Total Recall (UNFILTERED) than the Base Program with Average Total Recall (UNFILTERED): $average_total_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_unfiltered / $base_program_avg_average_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average Recall (UNFILTERED) than the Base Program with Average Average Recall (UNFILTERED): $average_average_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_index_time / $base_program_avg_index_time) * 100" | bc)
        echo "$improvement% faster Average Index Time than the Base Program with Average Index Time: $average_index_time" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_query_time_all / $base_program_avg_total_query_time_all) * 100" | bc)
        echo "$improvement% better Average Total Query Time (ALL) than the Base Program with Average Total Query Time (ALL): $average_total_query_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_all / $base_program_avg_average_FGS_time_all) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (ALL) than the Base Program with Average Average FGS Time (ALL): $average_average_FGS_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_filtered / $base_program_avg_average_FGS_time_filtered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (FILTERED) than the Base Program with Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_unfiltered / $base_program_avg_average_FGS_time_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (UNFILTERED) than the Base Program with Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_runtime / $base_program_avg_runtime) * 100" | bc)
        echo "$improvement% faster Average Runtime than the Base Program with Average Runtime: $average_runtime" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"
echo >> "$RESULTS_FILE"
test_11_finish=$(date +%s)
test_11_time_diff=$(echo "$test_11_finish-test_11_start" | bc)
echo "Test 11 took $test_11_time_diff seconds."
echo

#TEST 12 (parallel_stitched_main different a)
test_12_start=$(date +%s)

# Program to execute
PROGRAMS=( \
"./bin/parallel_stitched_main 100 110 96 0.8 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main 100 110 96 1.0 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main 100 110 96 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main 100 110 96 1.4 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main 100 110 96 1.6 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main 100 110 96 1.8 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main 100 110 96 2.0 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
)
RESULTS_FILE="results.txt"
TEMP_FILE="temp_output.txt"
RUNS=5

echo "Test 12 (parallel_stitched_main different a)" >> "$RESULTS_FILE"
echo "Running Test 12/12 (parallel_stitched_main different a)"

# Initialize variables
base_program_avg_total_recall_all=0
base_program_avg_average_recall_all=0
base_program_avg_total_recall_filtered=0
base_program_avg_average_recall_filtered=0
base_program_avg_total_recall_unfiltered=0
base_program_avg_average_recall_unfiltered=0
base_program_avg_index_time=0
base_program_avg_total_query_time_all=0
base_program_avg_average_FGS_time_all=0
base_program_avg_average_FGS_time_filtered=0
base_program_avg_average_FGS_time_unfiltered=0
base_program_avg_runtime=0

program_avg_total_recalls_all=()
program_avg_average_recalls_all=()
program_avg_total_recalls_filtered=()
program_avg_average_recalls_filtered=()
program_avg_total_recalls_unfiltered=()
program_avg_average_recalls_unfiltered=()
program_avg_index_times=()
program_avg_total_query_times_all=()
program_avg_average_FGS_times_all=()
program_avg_average_FGS_times_filtered=()
program_avg_average_FGS_times_unfiltered=()
program_avg_runtimes=()


echo "Run Results:" >> "$RESULTS_FILE"

echo >> "$RESULTS_FILE"

echo "Programs are:" >> "$RESULTS_FILE"

for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"
    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))): $PROGRAM" >> "$RESULTS_FILE"
    else 
        echo "Program $((p+1)): $PROGRAM" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"

# Run each program
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"

    program_total_recall_all_sum=0
    program_average_recall_all_sum=0
    program_total_recall_filtered_sum=0
    program_average_recall_filtered_sum=0
    program_total_recall_unfiltered_sum=0
    program_average_recall_unfiltered_sum=0
    program_index_time_sum=0
    program_total_query_time_all_sum=0
    program_average_FGS_time_all_sum=0
    program_average_FGS_time_filtered_sum=0
    program_average_FGS_time_unfiltered_sum=0
    program_runtime_sum=0

    echo "Running Program $((p+1))/${#PROGRAMS[@]}: $PROGRAM" >> "$RESULTS_FILE"
    echo "Running Program $((p+1))/${#PROGRAMS[@]}..."

    for ((i=1; i<=RUNS; i++)); do
        echo "Running the program (Run $i/$RUNS)..."

        # Run the program and capture output
        $PROGRAM > "$TEMP_FILE"

        # Extract recalls and timing summary using grep and awk
        total_recall_all=$(awk '/Total recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_all=$(awk '/Average recall across ALL queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_filtered=$(awk '/Total recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_filtered=$(awk '/Average recall across FILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        total_recall_unfiltered=$(awk '/Total recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        average_recall_unfiltered=$(awk '/Average recall across UNFILTERED queries/ {print $NF}' "$TEMP_FILE" | sed 's/%//')
        index_time=$(awk '/Index build time/ {print $(NF-4)}' "$TEMP_FILE")
        total_query_time_all=$(awk '/Total time Query calculation took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_all=$(awk '/Average time FilteredGreadySearch took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_filtered=$(awk '/Average time FilteredGreadySearch took for FILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        average_FGS_time_unfiltered=$(awk '/Average time FilteredGreadySearch took for UNFILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        runtime=$(awk '/Program ran for/ {print $4}' "$TEMP_FILE")

        # Append run results to file
        echo "Run $i:" >> "$RESULTS_FILE"
        echo "Total Recall (ALL): $total_recall_all%" >> "$RESULTS_FILE"
        echo "Average Recall (ALL): $average_recall_all%" >> "$RESULTS_FILE"
        echo "Total Recall (Filtered): $total_recall_filtered%" >> "$RESULTS_FILE"
        echo "Average Recall (Filtered): $average_recall_filtered%" >> "$RESULTS_FILE"
        echo "Total Recall (Unfiltered): $total_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Average Recall (Unfiltered): $average_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Index Time: $index_time seconds" >> "$RESULTS_FILE"
        echo "Total Query Time (ALL): $total_query_time_all seconds" >> "$RESULTS_FILE" 
        echo "Average FGS Time (ALL): $average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Filtered): $average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average FGS Time (Unfiltered): $average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Program Runtime: $runtime seconds" >> "$RESULTS_FILE"
        echo >> "$RESULTS_FILE"

        # Accumulate values
        program_total_recall_all_sum=$(echo "scale=12; $program_total_recall_all_sum + $total_recall_all" | bc)
        program_average_recall_all_sum=$(echo "scale=12; $program_average_recall_all_sum + $average_recall_all" | bc)
        program_total_recall_filtered_sum=$(echo "scale=12; $program_total_recall_filtered_sum + $total_recall_filtered" | bc)
        program_average_recall_filtered_sum=$(echo "scale=12; $program_average_recall_filtered_sum + $average_recall_filtered" | bc)
        program_total_recall_unfiltered_sum=$(echo "scale=12; $program_total_recall_unfiltered_sum + $total_recall_unfiltered" | bc)
        program_average_recall_unfiltered_sum=$(echo "scale=12; $program_average_recall_unfiltered_sum + $average_recall_unfiltered" | bc)
        program_index_time_sum=$(echo "scale=12; $program_index_time_sum + $index_time" | bc)
        program_total_query_time_all_sum=$(echo "scale=12; $program_total_query_time_all_sum + $total_query_time_all" | bc)
        program_average_FGS_time_all_sum=$(echo "scale=12; $program_average_FGS_time_all_sum + $average_FGS_time_all" | bc)
        program_average_FGS_time_filtered_sum=$(echo "scale=12; $program_average_FGS_time_filtered_sum + $average_FGS_time_filtered" | bc)
        program_average_FGS_time_unfiltered_sum=$(echo "scale=12; $program_average_FGS_time_unfiltered_sum + $average_FGS_time_unfiltered" | bc)
        program_runtime_sum=$(echo "scale=12; $program_runtime_sum + $runtime" | bc)
    done


    # Calculate average
    average_program_total_recall_all=$(echo "scale=10; $program_total_recall_all_sum / $RUNS" | bc)
    average_program_average_recall_all=$(echo "scale=10; $program_average_recall_all_sum / $RUNS" | bc)
    average_program_total_recall_filtered=$(echo "scale=10; $program_total_recall_filtered_sum / $RUNS" | bc)
    average_program_average_recall_filtered=$(echo "scale=10; $program_average_recall_filtered_sum / $RUNS" | bc)
    average_program_total_recall_unfiltered=$(echo "scale=10; $program_total_recall_unfiltered_sum / $RUNS" | bc)
    average_program_average_recall_unfiltered=$(echo "scale=10; $program_average_recall_unfiltered_sum / $RUNS" | bc)
    average_program_index_time=$(echo "scale=10; $program_index_time_sum / $RUNS" | bc)
    average_program_total_query_time_all=$(echo "scale=10; $program_total_query_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_all=$(echo "scale=10; $program_average_FGS_time_all_sum / $RUNS" | bc)
    average_program_average_FGS_time_filtered=$(echo "scale=10; $program_average_FGS_time_filtered_sum / $RUNS" | bc)
    average_program_average_FGS_time_unfiltered=$(echo "scale=10; $program_average_FGS_time_unfiltered_sum / $RUNS" | bc)
    average_program_runtime=$(echo "scale=10; $program_runtime_sum / $RUNS" | bc)

    program_avg_total_recalls_all+=("$average_program_total_recall_all")
    program_avg_average_recalls_all+=("$average_program_average_recall_all")
    program_avg_total_recalls_filtered+=("$average_program_total_recall_filtered")
    program_avg_average_recalls_filtered+=("$average_program_average_recall_filtered")
    program_avg_total_recalls_unfiltered+=("$average_program_total_recall_unfiltered")
    program_avg_average_recalls_unfiltered+=("$average_program_average_recall_unfiltered")
    program_avg_index_times+=("$average_program_index_time")
    program_avg_total_query_times_all+=("$average_program_total_query_time_all")
    program_avg_average_FGS_times_all+=("$average_program_average_FGS_time_all")
    program_avg_average_FGS_times_filtered+=("$average_program_average_FGS_time_filtered")
    program_avg_average_FGS_times_unfiltered+=("$average_program_average_FGS_time_unfiltered")
    program_avg_runtimes+=("$average_program_runtime")

    # Save base program runtime
    if [ $p -eq 0 ]; then
        base_program_avg_total_recall_all=$average_program_total_recall_all
        base_program_avg_average_recall_all=$average_program_average_recall_all
        base_program_avg_total_recall_filtered=$average_program_total_recall_filtered
        base_program_avg_average_recall_filtered=$average_program_average_recall_filtered
        base_program_avg_total_recall_unfiltered=$average_program_total_recall_unfiltered
        base_program_avg_average_recall_unfiltered=$average_program_average_recall_unfiltered
        base_program_avg_index_time=$average_program_index_time
        base_program_avg_total_query_time_all=$average_program_total_query_time_all
        base_program_avg_average_FGS_time_all=$average_program_average_FGS_time_all
        base_program_avg_average_FGS_time_filtered=$average_program_average_FGS_time_filtered
        base_program_avg_average_FGS_time_unfiltered=$average_program_average_FGS_time_unfiltered
        base_program_avg_runtime=$average_program_runtime
    fi


    # Append average runtime to results file
    echo "Average Results for Program $((p+1)):" >> "$RESULTS_FILE"
    echo "Average Total Recall (ALL): $average_program_total_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (ALL): $average_program_average_recall_all seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (FILTERED): $average_program_total_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (FILTERED): $average_program_average_recall_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Total Recall (UNFILTERED): $average_program_total_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Average Recall (UNFILTERED): $average_program_average_recall_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Index Time: $average_program_index_time seconds" >> "$RESULTS_FILE"
    echo "Average Total Query Time (ALL): $average_program_total_query_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (ALL): $average_program_average_FGS_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (FILTERED): $average_program_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Average FGS Time (UNFILTERED): $average_program_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Runtime: $average_program_runtime seconds" >> "$RESULTS_FILE"
    echo >> "$RESULTS_FILE"
done

# Calculate percentage improvement
echo "Performance Comparison:" >> "$RESULTS_FILE"
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    average_total_recall_all="${program_avg_total_recalls_all[p]}"
    average_average_recall_all="${program_avg_average_recalls_all[p]}"
    average_total_recall_filtered="${program_avg_total_recalls_filtered[p]}"
    average_average_recall_filtered="${program_avg_average_recalls_filtered[p]}"
    average_total_recall_unfiltered="${program_avg_total_recalls_unfiltered[p]}"
    average_average_recall_unfiltered="${program_avg_average_recalls_unfiltered[p]}"
    average_index_time="${program_avg_index_times[p]}"
    average_total_query_time_all="${program_avg_total_query_times_all[p]}"
    average_average_FGS_time_all="${program_avg_average_FGS_times_all[p]}"
    average_average_FGS_time_filtered="${program_avg_average_FGS_times_filtered[p]}"
    average_average_FGS_time_unfiltered="${program_avg_average_FGS_times_unfiltered[p]}"
    average_runtime="${program_avg_runtimes[p]}"

    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))):" >> "$RESULTS_FILE"
        echo "Average Total Recall (ALL): $average_total_recall_all %" >> "$RESULTS_FILE"
        echo "Average Average Recall (ALL): $average_average_recall_all %" >> "$RESULTS_FILE"
        echo "Average Total Recall (FILTERED): $average_total_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (FILTERED): $average_average_recall_filtered %" >> "$RESULTS_FILE"
        echo "Average Total Recall (UNFILTERED): $average_total_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Average Recall (UNFILTERED): $average_average_recall_unfiltered %" >> "$RESULTS_FILE"
        echo "Average Index Time: $average_index_time seconds" >> "$RESULTS_FILE"
        echo "Average Total Query Time (ALL): $average_total_query_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (ALL): $average_average_FGS_time_all seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Average Runtime: $average_runtime seconds" >> "$RESULTS_FILE"
    else
        echo >> "$RESULTS_FILE"
        echo "Program $((p+1)):" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_all / $base_program_avg_total_recall_all) * 100" | bc)
        echo "$improvement% better Average Total Recall (ALL) than the Base Program with Average Total Recall (ALL): $average_total_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_all / $base_program_avg_average_recall_all) * 100" | bc)
        echo "$improvement% better Average Average Recall (ALL) than the Base Program with Average Average Recall (ALL): $average_average_recall_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_filtered / $base_program_avg_total_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Total Recall (FILTERED) than the Base Program with Average Total Recall (FILTERED): $average_total_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_filtered / $base_program_avg_average_recall_filtered) * 100" | bc)
        echo "$improvement% better Average Average Recall (FILTERED) than the Base Program with Average Average Recall (FILTERED): $average_average_recall_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_recall_unfiltered / $base_program_avg_total_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Total Recall (UNFILTERED) than the Base Program with Average Total Recall (UNFILTERED): $average_total_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_recall_unfiltered / $base_program_avg_average_recall_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average Recall (UNFILTERED) than the Base Program with Average Average Recall (UNFILTERED): $average_average_recall_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_index_time / $base_program_avg_index_time) * 100" | bc)
        echo "$improvement% faster Average Index Time than the Base Program with Average Index Time: $average_index_time" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_total_query_time_all / $base_program_avg_total_query_time_all) * 100" | bc)
        echo "$improvement% better Average Total Query Time (ALL) than the Base Program with Average Total Query Time (ALL): $average_total_query_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_all / $base_program_avg_average_FGS_time_all) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (ALL) than the Base Program with Average Average FGS Time (ALL): $average_average_FGS_time_all" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_filtered / $base_program_avg_average_FGS_time_filtered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (FILTERED) than the Base Program with Average Average FGS Time (FILTERED): $average_average_FGS_time_filtered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_average_FGS_time_unfiltered / $base_program_avg_average_FGS_time_unfiltered) * 100" | bc)
        echo "$improvement% better Average Average FGS Time (UNFILTERED) than the Base Program with Average Average FGS Time (UNFILTERED): $average_average_FGS_time_unfiltered" >> "$RESULTS_FILE"
        improvement=$(echo "scale=5; (1 - $average_runtime / $base_program_avg_runtime) * 100" | bc)
        echo "$improvement% faster Average Runtime than the Base Program with Average Runtime: $average_runtime" >> "$RESULTS_FILE"
    fi
done

echo >> "$RESULTS_FILE"
echo >> "$RESULTS_FILE"
test_12_finish=$(date +%s)
test_12_time_diff=$(echo "$test_12_finish-test_12_start" | bc)
echo "Test 12 took $test_12_time_diff seconds."
echo

# Clean up
rm "$TEMP_FILE"

echo "Results saved to $RESULTS_FILE"
current_time_finish=$(date +%s)

time_diff=$(echo "$current_time_finish-$current_time_start" | bc)
echo "Testing took $time_diff seconds."