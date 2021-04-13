import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

if __name__ == "__main__":
    #A dataframe containing the formatted results
    df = pd.read_csv("results.csv")

    N = [16, 32, 64, 128, 256, 512, 1024]

    for n in N:
        df_trial = df[df['num_v'].map(lambda x: True if x == n else False)]
        serial = df_trial[df_trial['p'].map(lambda x: True if x == 1 else False)]
        serial_time = serial['time'].astype(float)
        print(serial_time)
        speedup = (map(lambda x: serial_time / x, df_trial['time']))
    #    for i in speedup:
    #        print("{0:0.2f}".format(i))
         #df_trial['speedup'] = (serial_time / df_trial['time'])
        #data_set = df_trial['num_v']['speedup']
        #print(data_set)
