
import random
import sys
import seaborn as sns
import pandas as pd    
import matplotlib.pyplot as plt

ls = ["-","--", ":","-.", " "]
mks = ["o", "D","P", "^", "X"]

def extract_data():
    fold = "../RND/resultsSpread"
    resfold = "resultsTime"
    topofiles = "timeTopo"
    with open("results.txt", 'w+') as f:
        f.write("SPREAD SIZE TIME\n")
        for spread in ["1000"]:
    		#folder = fold + spread"/"
            for toposize in [str(x) for x in range(100, 1001, 100)]:
    			#folder += resfold+toposize+"/"
                for idtopo in [str(x) for x in range(0,30)]:
                     resfile = fold+spread+"/"+resfold+toposize+"/"+topofiles+idtopo+".res"
                     with open(resfile) as f2:
                         for line in f2.readlines():
                             time = line.strip()
                             f.write("{} {} {}\n".format(spread, toposize, float(time)/1000))
                         

def main():
    extract_data()
    sns.set_style("whitegrid")
    palette = sns.color_palette("colorblind")
    data = pd.read_csv("results.txt", sep=" ")
    
    #fig, ax = plt.subplots()
    ax = sns.boxplot(x=data["SIZE"],  y=data["TIME"], hue=data["SPREAD"], palette=palette,linewidth=1)
    sns.despine()
    
    for i,artist in enumerate(ax.artists):
        # Set the linecolor on the artist to the facecolor, and set the facecolor to None
        col = artist.get_facecolor()
        artist.set_edgecolor(col)
        artist.set_facecolor('None')

        # Each box has 6 associated Line2D objects (to make the whiskers, fliers, etc.)
        # Loop over them here, and use the same colour as above
        for j in range(i*6,i*6+6):
            line = ax.lines[j]
            line.set_color(col)
            line.set_mfc(col)
            line.set_mec(col)

    # Also fix the legend
    # for legpatch in ax.get_legend().get_patches():
    #     col = legpatch.get_facecolor()
    #     legpatch.set_edgecolor(col)
    #     legpatch.set_linewidth(2)
    #     legpatch.set_facecolor('None')
        
    #plt.legend(title="S")
    ax.set(ylabel='Time (ms)', xlabel="# Nodes")
    ax.get_legend().remove()
    #plt.legend(title="T")
    #plt.show()
    plt.savefig('RND.pdf', dpi=300)

if __name__ == "__main__":
    main()
