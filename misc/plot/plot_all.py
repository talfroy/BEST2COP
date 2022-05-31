import os
import time

eval_folders = ["bramas-dream-team/v1","bramas-dream-team/v2", "pascal"]
exp_folders = ["CT_NODES", "CT_THREADS", "SPEEDUP","NBSEG", "RLT", "RND"]



to_pdf = {}

if not os.path.exists("all_pdf"):
	os.mkdir("all_pdf")

for ef in eval_folders:
	print("+ {}".format(ef))
	for exp in exp_folders:
		print("\t - {}".format(exp)) 
		if exp == "SPEEDUP":
			folder = ef+"/CT_THREADS/"
			script = "plot_speedup.py"
		else:
			folder = ef+'/'+exp+"/"
			script = "plot_{}.py".format(exp)
		
		if os.path.exists(folder):
			os.chdir("{}".format(folder))
			os.system("python3 {}".format(script))
			os.chdir("../"*(len(folder.split('/'))-1))
			#time.sleep(3)
			result = "{}/{}.pdf".format(folder, exp)
			os.system("cp {} all_pdf/{}.pdf".format(result, exp+'-'+ef.replace("/", "-")))
			keyone = ef.replace("/", "-")
			keytwo = exp
			if keyone not in to_pdf: 
				to_pdf[keyone] = {}
			if keytwo not in to_pdf[keyone]:
				to_pdf[keyone][keytwo] = None
			to_pdf[keyone][keytwo] = "all_pdf/{}.pdf".format(exp+'-'+ef.replace("/", "-"))
			

with open("all_pdf.tex", "w+") as f:
	f.write("\\documentclass{article}\n\\usepackage[utf8]{inputenc}\n\\usepackage{graphicx}\n\\usepackage{float}\n")
	f.write("\\begin{document}\n")
	for section in to_pdf:
		f.write("\section{{{}}}\n".format(section))
		for subsection in to_pdf[section]:
			f.write("\subsection{{{}}}\n".format(subsection.replace("_", " ")))
			f.write("\\begin{figure}[H]\n")
			f.write("\\centering\n")
			f.write("\\includegraphics[scale=0.65]{{{}}}\n".format(to_pdf[section][subsection]))
			f.write("\\end{figure}\n")
	f.write("\\end{document}\n")

			
