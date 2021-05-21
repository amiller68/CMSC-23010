import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from pathlib import Path
Path("../Docs/graphs").mkdir(parents=True, exist_ok=True)

#Overhead Experiment
ind = np.arange(6)  # the x locations for the groups
width = 0.35        # the width of the bars

W = ['25', '50', '100', '200', '400', '800']
L = ['p', 'a']

overhead_data = pd.read_csv("exp_data/overhead.csv")

fig = plt.figure(figsize = (10,10))

#Extract Relevant data from CSV
p_data = overhead_data[overhead_data["L"] == 'p']
a_data = overhead_data[overhead_data["L"] == 'a']

p_speedup = p_data["Speedup"]
a_speedup = a_data["Speedup"]


plt.xlabel('W')
plt.ylabel('Speedup (Lock-Free throughput / Home-Queue throughput)')
plt.title('Idle Lock Overhead: M = 2000 ms, N = 1, U = t, D = 8')
plt.xticks(ind + width / 2, W)
plt.yticks(np.arange(0, 2, 0.1))

plt.bar(ind, p_speedup, width, color='royalblue', label = 'Mutex')

plt.bar(ind + width, a_speedup, width, color='seagreen', label = "Anderson")

plt.legend(loc='best')

plt.axhline(y=1.0, color='r', linestyle='--', label = "Ideal Performance")

plt.savefig('../Docs/graphs/overhead.png')
plt.clf()


#Speedup Experiments

W = [1000, 2000, 4000, 8000]
N = [2, 3, 4, 8, 14, 28]

U = ['t', 'f']
L = ['p', 'a']
S = ['H', 'A']

packet_method = ""
lock_type = ""
strategy = ""

speedup_data  = pd.read_csv("exp_data/speedup.csv")

L_data = speedup_data[speedup_data["S"] == 'L']
L_speedup
H_data = speedup_data[speedup_data["S"] == 'H']
A_data = speedup_data[speedup_data["S"] == 'A']


for u in U:
    #Set our Packet retrieval method
    if u == 't':
        packet_method = "Uniform"
    else:
        packet_method = "Exponential"

    #Set our Strategy
    for s in S:
        if s == 'H':
            strategy = "Home-Queue"
        else:
            strategy = "Anderon"

        fig = plt.figure(figsize = (10,10))
        plt.ylabel('Speedup (parallel throughput / serial throughput)')
        plt.xlabel('Number of thread (N)')






        for l in L:
            if l == 'p':
                packet_method = "Mutex"
            else:
                packet_method = "Anderon"
            for



plt.title('Exp-2: B = 10000')



for l in opt:
    exp2_data = pd.read_csv("exp_data/exp2_" + l + ".csv")
    Speedup = exp2_data["Speedup"]
    if l == 't':
        plt.plot(N, Speedup, label = ("Lock = Test And Set"))
    if l == 'p':
        plt.plot(N, Speedup, label = ("Lock = Mutex"))
    if l == 'a':
        plt.plot(N, Speedup, label = ("Lock = Anderson"))
    if l == 'm':
        plt.plot(N, Speedup, label = ("Lock = MCS"))

plt.legend()
plt.savefig('../Docs/graphs/exp2.png')
plt.clf()

#Esperiment 3
for l in opt:
    fig = plt.figure(figsize = (10,10))
    plt.plot(N, ideal, label ="Ideal Performance")
    plt.ylabel('Speedup (parallel runtime / serial runtime)')
    plt.xlabel('Size of critical section (t ms)')
    if l == 't':
        plt.title('Exp-3: B = 3136 (ms), lock = Test And Set')
    if l == 'p':
        plt.title('Exp-3: B = 3136 (ms), lock = Mutex')
    if l == 'a':
        plt.title('Exp-3: B = 3136 (ms), lock = Anderson')
    if l == 'm':
        plt.title('Exp-3: B = 3136 (ms), lock = MCS')

    for n in N:
        exp3_data = pd.read_csv("exp_data/exp3_" + l + ":" + str(n) + ".csv")
        Speedup = exp3_data["Speedup"]
        plt.plot(T, Speedup, label = ("N = " + str(n)))

    plt.legend()
    plt.savefig('../Docs/graphs/exp3_' + l + '.png')
    plt.clf()
