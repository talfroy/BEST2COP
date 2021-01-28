import random
import sys
import seaborn as sns
import pandas as pd    
import matplotlib.pyplot as plt
import random
import numpy as np
import matplotlib.ticker as ticker

def extract_data():
    with open("results.txt", 'w+') as f:
        f.write("TOPO C1 NB_ITER IS_FEASIBLE\n")
        for c2index in [5,10,50,100]:
            for resfile in ["../NBSEG/res_rand_dclc{}.txt".format(c2index)]:#,"gblx_res.txt"]:
    	        with open(resfile) as f2:
                    for line in f2.readlines():
                        if "C2" in line: continue
                        c2, nb_iter, is_feasible = line.strip().split()[2:5]
                        #if nb_iter == "-1":print("fuck")
                        #if int(nb_iter) > 15: continue
                        if resfile == "realistic_sprint_res.txt":
                            name = "ISP-1"
                        else:
                            name = "ISP-big"
                        if int(is_feasible) == 1 and nb_iter != "-1":
                    	    is_feasible = nb_iter 
                        else:
                            #continue 
                    	    is_feasible = -1
                        f.write("{} {} {} {}\n".format(name, c2, nb_iter, is_feasible))
                   # f.write("{} {} {} {}\n".format(name, int(c2)-500, nb_iter, random.choice([-1,nb_iter])))




extract_data()

# feasible_dict = {}
# nb_seg_dict = {}
# with open("results.txt", 'r') as f:
#     for line in f.readlines():
#         if "C1" in line: continue
#         c2, nb_iter, is_feasible = line.strip().split()[1:4]
#         if int(nb_iter) not in nb_seg_dict:
#             nb_seg_dict[int(nb_iter)] = 0
#         if int(nb_iter) not in feasible_dict:
#             feasible_dict[int(nb_iter)] = 0
#         nb_seg_dict[int(nb_iter)] += 1
#         feasible_dict[int(nb_iter)] += 1



sns.set_style("whitegrid")
palette_bright = sns.color_palette("bright", n_colors=4)
palette_dark = sns.color_palette("dark", n_colors=4)
data = pd.read_csv("results.txt", sep=" ")

fig, ax = plt.subplots()
#ax.xaxis.LABELPAD=10
#ax.set(yscale="log")
#print(data["NB_ITER"])
sns.histplot(x = data["NB_ITER"] ,bins=range(-1, data["NB_ITER"].max()+1, 1), hue=data["C1"], multiple="dodge", ax=ax, palette=palette_bright, discrete=True, shrink=.8, stat="density")
sns.histplot(x = data["IS_FEASIBLE"] ,bins=range(10, data["NB_ITER"].max()+1, 1), hue=data["C1"], multiple="dodge", ax=ax, palette=palette_dark, discrete=True, shrink=.8, stat="density")
sns.despine()


plt.xticks(np.arange(-1, data["NB_ITER"].max()+1, 1))

ax.set(ylabel='', xlabel="# Segments")
#plt.legend(title="T")
#plt.show()

ticks_y = ticker.FuncFormatter(lambda x, pos: '{0:g}'.format(x*4))
ax.yaxis.set_major_formatter(ticks_y)

plt.savefig('NBSEG-DCLC-RAND.pdf', dpi=300)





