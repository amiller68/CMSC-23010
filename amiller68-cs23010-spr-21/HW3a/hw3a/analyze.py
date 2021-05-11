import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from pathlib import Path
Path("../Docs/graphs").mkdir(parents=True, exist_ok=True)

#Experiment 1:
opt = ['t', 'p', 'a']
N = [1, 2, 4, 8, 14]
T = [1, 2, 4, 8, 14]

exp1_data = pd.read_csv("exp_data/exp1.csv")

fig = plt.figure(figsize = (8,8))
ax = fig.add_subplot(111)
Locks = ['Serial', 'TAS', 'Mutex', 'A-Lock', 'MCS']
Speedup = exp1_data["Speedup"]
plt.ylabel('Speedup (parallel throughput / serial throughput)')
plt.title('Exp-1: B = 10000, N = 1')
ax.set_xticklabels(Locks, rotation = 45)
ax.set_yticks(np.arange(0, 5, 0.5))
ax.bar(Locks,Speedup)
plt.savefig('../Docs/graphs/exp1.png')
plt.clf()

#Experiment 2
fig = plt.figure(figsize = (8,8))
ideal=[1.0,1.0,1.0,1.0,1.0]
plt.plot(N, ideal, label ="Ideal Performance")
plt.ylabel('Speedup (parallel throughput / serial throughput)')
plt.xlabel('Number of thread (N)')
plt.title('Exp-2: B = 10000')

for l in opt:
    exp2_data = pd.read_csv("exp_data/exp2_" + l + ".csv")
    Speedup = exp2_data["Speedup"]
    plt.plot(N, Speedup, label = ("Lock = " + l))

plt.legend()
plt.savefig('../Docs/graphs/exp2.png')
plt.clf()

#Esperiment 3
for l in opt:
    fig = plt.figure(figsize = (8,8))
    plt.plot(N, ideal, label ="Ideal Performance")
    plt.ylabel('Speedup (parallel runtime / serial runtime)')
    plt.xlabel('Size of critical section (t ms)')
    plt.title('Exp-3: B = 3136 (ms), lock = ' + l)

    for n in N:
        exp3_data = pd.read_csv("exp_data/exp3_" + l + ":" + str(n) + ".csv")
        Speedup = exp3_data["Speedup"]
        plt.plot(T, Speedup, label = ("N = " + str(n)))

    plt.legend()
    plt.savefig('../Docs/graphs/exp3_' + l + '.png')
    plt.clf()
