#!/bin/bash

# Program to execute
PROGRAMS=( \
"./bin/filtered_main_graph 40 80 1.2 2 ./datasets/smallscale/dummy-data.bin" \
"./bin/filtered_main_graph 60 80 1.2 2 ./datasets/smallscale/dummy-data.bin" \
)
RESULTS_FILE="results2.txt"
TEMP_FILE="temp_output2.txt"
RUNS=5

# Initialize variables
base_program_avg_runtime=0
program_avg_runtimes=()

# Clear previous results
echo "Run Results:" > "$RESULTS_FILE"

# Run each program
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    PROGRAM="${PROGRAMS[p]}"
    program_runtime_sum=0

    echo "Running $PROGRAM..." >> "$RESULTS_FILE"

    for ((i=1; i<=RUNS; i++)); do
        echo "Running the program (Run $i/$RUNS)..."

        # Run the program and capture output
        $PROGRAM > "$TEMP_FILE"

        # Extract timing summary using grep and awk
        runtime=$(awk '/Program ran in/ {print $4}' "$TEMP_FILE")

        # Append run results to file
        echo "Run $i: $runtime seconds" >> "$RESULTS_FILE"

        # Accumulate runtime
        program_runtime_sum=$(echo "scale=10; $program_runtime_sum + $runtime" | bc)
    done

    # Calculate average runtime
    average_program_runtime=$(echo "scale=2; $program_runtime_sum / $RUNS" | bc)
    program_avg_runtimes+=("$average_program_runtime")

    # Save base program runtime
    if [ $p -eq 0 ]; then
        base_program_avg_runtime=$average_program_runtime
    fi

    # Append average runtime to results file
    echo "Average Runtime for Program $((p+1)): $average_program_runtime seconds" >> "$RESULTS_FILE"
    echo >> "$RESULTS_FILE"
done

# Calculate percentage improvement
echo "Performance Comparison:" >> "$RESULTS_FILE"
for ((p=0; p<${#PROGRAMS[@]}; p++)); do
    average_runtime="${program_avg_runtimes[p]}"
    if [ $p -eq 0 ]; then
        echo "Base Program (Program $((p+1))): $average_runtime seconds" >> "$RESULTS_FILE"
    else
        improvement=$(echo "scale=2; (1 - $average_runtime / $base_program_avg_runtime) * 100" | bc)
        echo "Program $((p+1)) was $improvement% faster than the base program." >> "$RESULTS_FILE"
    fi
done

# Clean up
rm "$TEMP_FILE"

echo "Results saved to $RESULTS_FILE"
