
import random
import sys
import seaborn as sns
import pandas as pd    
import matplotlib.pyplot as plt
sns.categorical._Old_Violin = sns.categorical._ViolinPlotter
class _My_ViolinPlotter(sns.categorical._Old_Violin):

    def __init__(self, *args, **kwargs):
        super(_My_ViolinPlotter, self).__init__(*args, **kwargs)
        self.gray='red'


ls = ["-","--", ":","-.", " "]
mks = ["o", "D","P", "^", "X"]

def extract_data():
    with open("result.txt", 'w+') as f:
        f.write("TOPO TIME\n")
        for resfile in ["../RLT/results.txt"]:
    	    with open(resfile) as f2:
                for line in f2.readlines():
                    if "TIME" in line: continue
                    time = line.strip().split()[3]
                    name = "Large-ISP"
                    f.write("{} {}\n".format(name, float(time)/1000))
                         

def main():
    extract_data()
    sns.set_style("whitegrid")
    palette = sns.color_palette("colorblind", n_colors=3)
    data = pd.read_csv("result.txt", sep=" ")
    
    fig, ax = plt.subplots()
    #ax.set(yscale="log")
    sns.violinplot(x=data["TOPO"],  y=data["TIME"], palette=palette,linewidth=1, scale="width", yscale="log")
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
    
    
    ax.set(ylabel='Time (ms)', xlabel="")
    #plt.legend(title="T")
    #plt.show()
    plt.savefig('RLT.pdf', dpi=300)

if __name__ == "__main__":
    main()
