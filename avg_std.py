import glob
import statistics

# Adjust this path pattern to match your files
files = glob.glob("results/100/cs2/r*")

costs = []

for filename in files:
    with open(filename, 'r') as f:
        first_line = f.readline().strip()
        if first_line.startswith("Cost:"):
            # Extract numeric value after "Cost:"
            value = float(first_line.split(":")[1].strip())
            costs.append(value)

if costs:
    avg = statistics.mean(costs)
    stddev = statistics.stdev(costs) if len(costs) > 1 else 0.0
    print(f"Processed {len(costs)} files.")
    print(f"Average cost: {avg:.2f}")
    print(f"Standard deviation: {stddev:.2f}")
else:
    print("No valid 'Cost:' lines found.")
