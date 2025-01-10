#!/bin/bash

# Program to execute
PROGRAMS=( \
"./bin/parallel_stitched_main 100 stitched_graph_L=110_a=1.200000_R=96_R_stitched=98 stitched_map_L=110_a=1.200000_R=96_R_stitched=98 ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin" \
"./bin/parallel_stitched_main 20 stitched_graph_L=40_a=1.200000_R=80_R_stitched=2 stitched_map_L=40_a=1.200000_R=80_R_stitched=2 ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=20.bin" \
)
RESULTS_FILE="results.txt"
TEMP_FILE="temp_output.txt"
RUNS=5

# Initialize accumulators
total_recall_all=0
average_recall_all=0
total_recall_filtered=0
average_recall_filtered=0
total_recall_unfiltered=0
average_recall_unfiltered=0
total_time_all=0
total_time_filtered=0
total_time_unfiltered=0
program_runtime=0

total_recall_all_sum=0
average_recall_all_sum=0
total_recall_filtered_sum=0
average_recall_filtered_sum=0
total_recall_unfiltered_sum=0
average_recall_unfiltered_sum=0
total_time_all_sum=0
total_time_filtered_sum=0
total_time_unfiltered_sum=0
program_runtime_sum=0

# Clear previous results
echo "Run Results:" > "$RESULTS_FILE"

# Run the program multiple times
for PROGRAM in "${PROGRAMS[@]}"; do
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
        total_time_all=$(awk '/Total time FilteredGreadySearch calculation took for ALL queries/ {print $(NF-1)}' "$TEMP_FILE")
        total_time_filtered=$(awk '/Total time FilteredGreadySearch calculation took for FILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        total_time_unfiltered=$(awk '/Total time FilteredGreadySearch calculation took for UNFILTERED queries/ {print $(NF-1)}' "$TEMP_FILE")
        runtime=$(awk '/Program ran for/ {print $4}' "$TEMP_FILE")

        # Append run results to file
        echo "Run $i:" >> "$RESULTS_FILE"
        echo "Total Recall (ALL): $total_recall_all%" >> "$RESULTS_FILE"
        echo "Average Recall (ALL): $average_recall_all%" >> "$RESULTS_FILE"
        echo "Total Recall (Filtered): $total_recall_filtered%" >> "$RESULTS_FILE"
        echo "Average Recall (Filtered): $average_recall_filtered%" >> "$RESULTS_FILE"
        echo "Total Recall (Unfiltered): $total_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Average Recall (Unfiltered): $average_recall_unfiltered%" >> "$RESULTS_FILE"
        echo "Total Time (ALL): $total_time_all seconds" >> "$RESULTS_FILE"
        echo "Total Time (Filtered): $total_time_filtered seconds" >> "$RESULTS_FILE"
        echo "Total Time (Unfiltered): $total_time_unfiltered seconds" >> "$RESULTS_FILE"
        echo "Program Runtime: $runtime seconds" >> "$RESULTS_FILE"
        echo >> "$RESULTS_FILE"

        # Accumulate values
        total_recall_all_sum=$(echo "scale=10; $total_recall_all_sum + $total_recall_all" | bc)
        average_recall_all_sum=$(echo "scale=10; $average_recall_all_sum + $average_recall_all" | bc)
        total_recall_filtered_sum=$(echo "scale=10; $total_recall_filtered_sum + $total_recall_filtered" | bc)
        average_recall_filtered_sum=$(echo "scale=10; $average_recall_filtered_sum + $average_recall_filtered" | bc)
        total_recall_unfiltered_sum=$(echo "scale=10; $total_recall_unfiltered_sum + $total_recall_unfiltered" | bc)
        average_recall_unfiltered_sum=$(echo "scale=10; $average_recall_unfiltered_sum + $average_recall_unfiltered" | bc)
        total_time_all_sum=$(echo "scale=10; $total_time_all_sum + $total_time_all" | bc)
        total_time_filtered_sum=$(echo "scale=10; $total_time_filtered_sum + $total_time_filtered" | bc)
        total_time_unfiltered_sum=$(echo "scale=10; $total_time_unfiltered_sum + $total_time_unfiltered" | bc)
        program_runtime_sum=$(echo "scale=10; $program_runtime_sum + $runtime" | bc)
    done

    # Calculate averages
    average_total_recall_all=$(echo "scale=2; $total_recall_all_sum / $RUNS" | bc)
    average_average_recall_all=$(echo "scale=2; $average_recall_all_sum / $RUNS" | bc)
    average_total_recall_filtered=$(echo "scale=2; $total_recall_filtered_sum / $RUNS" | bc)
    average_average_recall_filtered=$(echo "scale=2; $average_recall_filtered_sum / $RUNS" | bc)
    average_total_recall_unfiltered=$(echo "scale=2; $total_recall_unfiltered_sum / $RUNS" | bc)
    average_average_recall_unfiltered=$(echo "scale=2; $average_recall_unfiltered_sum / $RUNS" | bc)
    average_total_time_all=$(echo "scale=2; $total_time_all_sum / $RUNS" | bc)
    average_total_time_filtered=$(echo "scale=2; $total_time_filtered_sum / $RUNS" | bc)
    average_total_time_unfiltered=$(echo "scale=2; $total_time_unfiltered_sum / $RUNS" | bc)
    average_program_runtime=$(echo "scale=2; $program_runtime_sum / $RUNS" | bc)

    # Append averages to results file
    echo "Averages:" >> "$RESULTS_FILE"
    echo "Average Total Recall (ALL): $average_total_recall_all%" >> "$RESULTS_FILE"
    echo "Average Average Recall (ALL): $average_average_recall_all%" >> "$RESULTS_FILE"
    echo "Average Total Recall (Filtered): $average_total_recall_filtered%" >> "$RESULTS_FILE"
    echo "Average Average Recall (Filtered): $average_average_recall_filtered%" >> "$RESULTS_FILE"
    echo "Average Total Recall (Unfiltered): $average_total_recall_unfiltered%" >> "$RESULTS_FILE"
    echo "Average Average Recall (Unfiltered): $average_average_recall_unfiltered%" >> "$RESULTS_FILE"
    echo "Average Total Time (ALL): $average_total_time_all seconds" >> "$RESULTS_FILE"
    echo "Average Total Time (Filtered): $average_total_time_filtered seconds" >> "$RESULTS_FILE"
    echo "Average Total Time (Unfiltered): $average_total_time_unfiltered seconds" >> "$RESULTS_FILE"
    echo "Average Program Runtime: $average_program_runtime seconds" >> "$RESULTS_FILE"
done

# Clean up
rm "$TEMP_FILE"

echo "Results saved to $RESULTS_FILE"