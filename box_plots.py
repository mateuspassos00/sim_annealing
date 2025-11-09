import glob
import matplotlib.pyplot as plt

files_cs1 = glob.glob("results/100/cs1/r*")
files_cs2 = glob.glob("results/100/cs2/r*")

costs_cs1 = []
costs_cs2 = []

for filename in files_cs1:
    with open(filename, 'r') as f:
        first_line = f.readline().strip()
        if first_line.startswith("Cost:"):
            # Extract numeric value after "Cost:"
            value = float(first_line.split(":")[1].strip())
            costs_cs1.append(value)

for filename in files_cs2:
    with open(filename, 'r') as f:
        first_line = f.readline().strip()
        if first_line.startswith("Cost:"):
            # Extract numeric value after "Cost:"
            value = float(first_line.split(":")[1].strip())
            costs_cs2.append(value)

costs_cs1.sort()
costs_cs2.sort()

costs = [costs_cs1, costs_cs2]

plt.boxplot(costs)
plt.title(f"100 Cities")
plt.show()