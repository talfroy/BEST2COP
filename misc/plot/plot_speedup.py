
import random
import sys
import seaborn as sns
import pandas as pd    
import matplotlib.pyplot as plt

ls = ["-","--", ":","-.", " "]
mks = ["o", "D","P", "^", "X"]

def main():
    sns.set_style("whitegrid")
    palette = sns.color_palette("colorblind", n_colors=4)
    data = pd.read_csv("results.txt", sep=" ")
    
    #fig, ax = plt.subplots()
    ax = sns.lineplot(x=data["NB_THREADS"],  y=data["TIME"].iloc[0]/data["TIME"], palette=palette)
    sns.despine()
    
    for i,l in enumerate(ax.lines): 
    	l.set_linestyle(ls[i%4])
    	l.set_marker(mks[i%4])
    
    ax.set(ylabel='Speedup', xlabel="T")
    plt.savefig('SPEEDUP.pdf', dpi=300)

if __name__ == "__main__":
    main()
