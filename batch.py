import subprocess
import re
import matplotlib.pyplot as plt
import numpy as np
from collections import defaultdict

# Paramètres
N_EXECUTIONS = 100
CMD = "./lang test"

# Stockage des résultats
all_data = defaultdict(list)
max_turns = 0

# Expressions régulières pour parser le fichier log
dist_pattern = re.compile(r"([0-9.]+) pourcents de (\w+)")
distribution_marker = "La distribution des strategies est"

for run in range(N_EXECUTIONS):
    print(f"Lancement de l'exécution {run + 1}/{N_EXECUTIONS}")

    # On exécute le programme lang.y qui simule la population d'agents, et affiche les résultats
    # dans l'entrée standard.
    result = subprocess.run(CMD, shell=True, capture_output=True, text=True)

    lines = result.stdout.splitlines()
    temp_data = defaultdict(list)
    turn = -1
    parsing_distribution = False

    for line in lines:
        if distribution_marker in line:
            turn += 1
            parsing_distribution = True
            continue

        if parsing_distribution:
            match = dist_pattern.match(line.strip())
            if match:
                percent, strategy = float(match.group(1)), match.group(2)
                temp_data[strategy].append(percent)
            elif line.strip() == "":
                parsing_distribution = False

    max_turns = max(max_turns, turn + 1)
    for strategy, values in temp_data.items():
        for t, val in enumerate(values):
            if len(all_data[(strategy, t)]) < run + 1:
                all_data[(strategy, t)].extend([0.0] * (run + 1 - len(all_data[(strategy, t)])))
            all_data[(strategy, t)][run] = val

# Moyenne par stratégie et tour
strategy_names = sorted(set(k[0] for k in all_data))
averages = {strategy: [np.mean(all_data[(strategy, t)]) if (strategy, t) in all_data else 0.0
                       for t in range(max_turns)]
            for strategy in strategy_names}

# Tracé
plt.figure(figsize=(10, 6))
for strategy, values in averages.items():
    plt.plot(range(max_turns), values, label=strategy)

plt.xlabel("Tour")
plt.ylabel("Pourcentage moyen d'agents (%)")
plt.title(f"Moyenne sur {N_EXECUTIONS} exécutions de la distribution des stratégies")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()

