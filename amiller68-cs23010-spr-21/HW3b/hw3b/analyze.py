import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from pathlib import Path
from statistics import variance
from scipy import stats

Path("../Docs/graphs").mkdir(parents=True, exist_ok=True)

#Overhead Experiment
ind = np.arange(6)  # the x locations for the groups
width = 0.35        # the width of the bars

W = ['25', '50', '100', '200', '400', '800']
L = ['p', 'a']

overhead_data = pd.read_csv("exp_data/overhead.csv")
D = overhead_data['D'][0]
M = overhead_data['M'][0]

fig = plt.figure(figsize = (15,15))

#Extract Relevant data from CSV
p_data = overhead_data[overhead_data["L"] == 'p']
a_data = overhead_data[overhead_data["L"] == 'a']

p_speedup = p_data["Speedup"]
a_speedup = a_data["Speedup"]


plt.xlabel('W')
plt.ylabel('Speedup (Lock-Free throughput / Home-Queue throughput)')
plt.title('Idle Lock Overhead: M = ' + str(M) + 'ms, N = 1, U = t, D = '+ str(D))
plt.xticks(ind + width / 2, W)
plt.yticks(np.arange(0.0, 1.5, 0.1))
plt.ylim([0, 1.5])


plt.bar(ind, p_speedup, width, color='royalblue', label = 'Mutex')

plt.bar(ind + width, a_speedup, width, color='seagreen', label = "Anderson")

plt.legend(loc='best')

plt.axhline(y=1.0, color='r', linestyle='--', label = "Ideal Performance")

plt.savefig('../Docs/graphs/overhead_' + str(D) + '.png')
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
D = speedup_data['D'][0]
M = speedup_data['M'][0]

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
        fig.suptitle(packet_method + ' Packet Distrobution Speedup Results:  M = ' + str(M) + 'ms, U = ' + u + ', D = '+ str(D))


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

                p_speedup = np.array((Comp_data[(Comp_data['W'] == w) & (Comp_data['U'] == u) & (Comp_data['L'] == 'p')])["Speedup"].values.tolist())
                a_speedup = np.array((Comp_data[(Comp_data['W'] == w) & (Comp_data['U'] == u) & (Comp_data['L'] == 'a')])["Speedup"].values.tolist())

                ax.plot(N, L_speedup, 'tab:blue', label = "Straetgy = Lock-Free, Lock = N/A")
                ax.plot(N, p_speedup, color = 'tab:green', label = "Straetgy = " + strategy + ", Lock = Mutex")
                ax.plot(N, a_speedup, color = 'tab:red', label = "Straetgy = " + strategy + ", Lock = Anderson")

                #Label Variance data
                for (n,l,p,a) in zip(N,L_speedup, p_speedup, a_speedup):
                    if s == 'H':
                        var = variance([l,p,a])
                    else:
                        var = variance([p,a])

                    best = max(l, p, a)
                    if(p > a):
                        best_l = 'p'
                    else:
                        best_l = 'a'

                    if (l == best):
                        label = ("{:.5f},l," + best_l).format(var)
                    if (p == best):
                        label = ("{:.5f},p," + best_l).format(var)
                    if (a == best):
                        label = ("{:.5f},a," + best_l).format(var)


                    ax.annotate(label,(n,l), textcoords="offset points",  xytext=(0,10), ha='center')

        #Iterate through subplots
        N_ind = [2, 3, 4, 8, 14, 28]
        workers = [1, 2, 3, 7, 13, 27]
        if (s == 'H'):
            print("LockFree local maximums, " + packet_method + " load:")
            for w in W:
                #Plot data for Lock-Free Speedup
                L_speedup = (L_data[(L_data['W'] == w) & (L_data['U'] == u)])["Speedup"].values.tolist()
                #Print LockFree local maximums
                max_speedup = max(L_speedup)
                index = L_speedup.index(max_speedup)
                n = N_ind[index]
                label = "$W = " + str(w) + "$: " + str(max_speedup) + "x speedup at $n = " + str(n) + "$"
                print(label)

            print("LockFree performance drops N = 14 to N = 28, " + packet_method + " load:")
            for w in W:
                #Plot data for Lock-Free Speedup
                L_speedup = ((L_data[(L_data['W'] == w) & (L_data['U'] == u)])["Speedup"].values.tolist())
                #Print LockFree local maximums
                speedup_14 = L_speedup[N_ind.index(14)]
                speedup_28 = L_speedup[N_ind.index(28)]

                dec = ((speedup_14 - speedup_28) / speedup_14) * 100
                label = "$W = " + str(w) + "$: " + "{:.5f}".format(dec) + "\% drop in performance"
                print(label)

            L_speedup = ((L_data[(L_data['W'] == 8000) & (L_data['U'] == u)])["Speedup"].values.tolist())


            slope, intercept, r_value, p_value, std_err = stats.linregress(workers[0:4],L_speedup[0:4])

            print("W = 8000, Lin-Reg results: slope = " + str(slope) + "  r^2 = " + str(r_value ** 2))

            print("LockFree ideal scale performance, " + packet_method + " load:")
            for n in N_ind:
                l = L_speedup[N_ind.index(n)]
                shy = (((n - 1) - l) / (n - 1)) * 100
                label = "$n = " +str(n) + "$, " + str(n - 1) + " worker threads: " + "{:.5f}".format(l) + "x speedup "+ "{:.5f}".format(shy) + "\% shy of ideal scaling"
                print(label)

        else:
            print("Awesome local maximums, averaged between lock types, " + packet_method + " load:")
            for w in W:
                p_speedup = np.array((Comp_data[(Comp_data['W'] == w) & (Comp_data['U'] == u) & (Comp_data['L'] == 'p')])["Speedup"].values.tolist())
                a_speedup = np.array((Comp_data[(Comp_data['W'] == w) & (Comp_data['U'] == u) & (Comp_data['L'] == 'a')])["Speedup"].values.tolist())

                test_speedup = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
                i = 0
                for p,a in zip(p_speedup, a_speedup):
                    avg = (p + a) / 2
                    test_speedup[i] = avg
                    i = i + 1

                #Print LockFree local maximums
                max_speedup = max(test_speedup)
                index = test_speedup.index(max_speedup)
                n = N_ind[index]
                label = "$W = " + str(w) + "$: " + str(max_speedup) + "x speedup at $n = " + str(n) + "$"
                print(label)

            print("Awesome performance drops N = 14 to N = 28, " + packet_method + " load:")
            for w in W:
                p_speedup = np.array((Comp_data[(Comp_data['W'] == w) & (Comp_data['U'] == u) & (Comp_data['L'] == 'p')])["Speedup"].values.tolist())
                a_speedup = np.array((Comp_data[(Comp_data['W'] == w) & (Comp_data['U'] == u) & (Comp_data['L'] == 'a')])["Speedup"].values.tolist())

                test_speedup = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
                i = 0
                for p,a in zip(p_speedup, a_speedup):
                    avg = (p + a) / 2
                    test_speedup[i] = avg
                    i = i + 1
                #Print LockFree local maximums
                speedup_14 = test_speedup[N_ind.index(14)]
                speedup_28 = test_speedup[N_ind.index(28)]

                dec = ((speedup_14 - speedup_28) / speedup_14) * 100
                label = "$W = " + str(w) + "$: " + "{:.5f}".format(dec) + "\% drop in performance"
                print(label)


            slope, intercept, r_value, p_value, std_err = stats.linregress(workers[0:4],test_speedup[0:4])

            print("W = 8000, Lin-Reg results: slope = " + str(slope) + "  r^2 = " + str(r_value ** 2))

            print("Awesome ideal scale performance, " + packet_method + " load:")
            for n in N_ind:
                l = test_speedup[N_ind.index(n)]
                shy = (((n - 1) - l) / (n - 1)) * 100
                label = "$n = " +str(n) + "$, " + str(n - 1) + " worker threads: " + "{:.5f}".format(l) + "x speedup "+ "{:.5f}".format(shy) + "\% shy of ideal scaling"
                print(label)

        handles, labels = ax.get_legend_handles_labels()
        fig.legend(handles, labels, loc='lower right')
        #plt.legend(loc=1)
        plt.savefig('../Docs/graphs/speedup_' + u + ':' + s +  ':' + str(D) + '.png')
        plt.clf()
