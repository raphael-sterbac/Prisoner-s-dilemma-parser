import re
import matplotlib.pyplot as plt

# Chargement du fichier
with open("log", "r") as file:
    lines = file.readlines()

# Dictionnaire pour stocker les données
data = {}
turn = 0
turns = []

# Expressions régulières pour parser le fichier log
dist_pattern = re.compile(r"([0-9.]+) pourcents de (\w+)")

for line in lines:
    match = dist_pattern.match(line.strip())
    if match:
        percent, strategy = float(match.group(1)), match.group(2)
        if strategy not in data:
            data[strategy] = []
        data[strategy].append(percent)
    elif "La distribution des strategies est" in line:
        turns.append(turn)
        turn += 1

# Tracé
plt.figure(figsize=(10, 6))
for strategy, values in data.items():
    plt.plot(range(len(values)), values, label=strategy)

plt.xlabel("Tour")
plt.ylabel("Pourcentage d'agents (%)")
plt.title("Évolution des proportions de chaque stratégie")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()
