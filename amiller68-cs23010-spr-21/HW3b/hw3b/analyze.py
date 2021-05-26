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

fig = plt.figure(figsize = (15,15))

#Extract Relevant data from CSV
p_data = overhead_data[overhead_data["L"] == 'p']
a_data = overhead_data[overhead_data["L"] == 'a']

p_speedup = p_data["Speedup"]
a_speedup = a_data["Speedup"]


plt.xlabel('W')
plt.ylabel('Speedup (Lock-Free throughput / Home-Queue throughput)')
plt.title('Idle Lock Overhead: M = 2000 ms, N = 1, U = t, D = 8')
plt.xticks(ind + width / 2, W)
plt.yticks(np.arange(0.0, 1.5, 0.1))
plt.ylim([0, 1.5])


plt.bar(ind, p_speedup, width, color='royalblue', label = 'Mutex')

plt.bar(ind + width, a_speedup, width, color='seagreen', label = "Anderson")

plt.legend(loc='best')

plt.axhline(y=1.0, color='r', linestyle='--', label = "Ideal Performance")

plt.savefig('../Docs/graphs/overhead.png')
plt.clf()


#Speedup Experiments

W = [1000, 2000, 4000, 8000]
N = np.array([2, 3, 4, 8, 14, 28])

U = ['t', 'f']
L = ['p', 'a']
S = ['H', 'A'] #Comparison Strategies

packet_method = ""
lock_type = ""
strategy = ""

speedup_data  = pd.read_csv("exp_data/speedup.csv")

#Extract Strategy Relevant Data
L_data = speedup_data[speedup_data["S"] == 'L']
L_speedup = L_data["Speedup"]
H_data = speedup_data[speedup_data["S"] == 'H']
H_speedup = H_data["Speedup"]
A_data = speedup_data[speedup_data["S"] == 'A']
A_speedup = A_data["Speedup"]

for u in U:
    #Set our Packet retrieval method
    if u == 't':
        packet_method = "Uniform"
    else:
        packet_method = "Exponential"

    #Set our Strategy and where were reading data
    for s in S:
        if s == 'H':
            strategy = "Home-Queue"
            Comp_data = H_data
        else:
            strategy = "Awesome"
            Comp_data = A_data

        #Define a figure to hold a graph for each value of W
        fig, axs = plt.subplots(2, 2, figsize=(15,15))

        #Set a Figure title
        fig.suptitle('Speedup Results: Load = ' + packet_method + ', Comparison Strategy = ' + strategy)


        for ax in axs.flat:
            ax.set(xlabel='Number of Threads (N)', ylabel='Speedup (Parallel throughput / Serial throughput)')
            ax.set_xticks(N)
            ax.set_xlim([0, 30])
            ax.set_yticks(np.arange(0, 13.5, 0.5))
            ax.set_ylim([0, 13.5])
            ax.axhline(y=1.0, color='r', linestyle='--', label = "Serial Performance")


        # Hide x labels and tick labels for top plots and y ticks for right plots.
        for ax in axs.flat:
            ax.label_outer()


        #Used to index through W
        i = 0

        #Iterate through subplots
        for x in range(2):
            for y in range(2):
                w = W[i]
                ax = axs[x, y]
                i = i + 1
                ax.set_title("W = " + str(w))

                #Plot data for Lock-Free Speedup
                L_speedup = np.array((L_data[(L_data['W'] == w) & (L_data['U'] == u)])["Speedup"].values.tolist())
                ax.plot(N, L_speedup, 'tab:blue', label = "Straetgy = Lock-Free, Lock = N/A")
                for l in L:
                    #Set The lock type label
                    if l == 'p':
                        lock_type = "Mutex"
                        color = 'tab:green'
                    else:
                        lock_type = "Anderson"
                        color = 'tab:red'

                    #Get the comparison speedup data for
                    Comp_speedup = np.array((Comp_data[(Comp_data['W'] == w) & (Comp_data['U'] == u) & (Comp_data['L'] == l)])["Speedup"].values.tolist())
                    ax.plot(N, Comp_speedup, color, label = "Straetgy = " + strategy + ", Lock = " + lock_type)
        #Done With plotting, save an move onto the next expierment

        handles, labels = ax.get_legend_handles_labels()
        fig.legend(handles, labels, loc='lower right')
        #plt.legend(loc=1)
        plt.savefig('../Docs/graphs/speedup_' + u + ':' + s + '.png')
        plt.clf()
