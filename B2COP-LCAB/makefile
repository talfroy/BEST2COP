SRCDIR=src
OBJDIR= obj
SRC := $(wildcard $(SRCDIR)/*.c)
OBJ  = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

PG ?= -g
# -pg 
# -p -g on mac

#CC ?= gcc-11

# For M1 
# After brew install llvm libomp
CC ?= /usr/local/opt/llvm/bin/clang 
## for M1
LIBS := -L/opt/homebrew/lib -lomp

FLAGS := -Wall -Wextra  -O3 -Xpreprocessor -fopenmp -I/opt/homebrew/include -Iinclude 

# for address sanitizer
# -g -fsanitize=address -fno-omit-frame-pointer

## for clang
#LIB := -lm -lomp
#FLAGS := -Wall -Wextra -Werror -O3 -Xpreprocessor - fopenmp -Iinclude $(PG)

## for gcc
#LIBS := -lm -fopenmp
#FLAGS := -Wall -Wextra -Werror -O3 -fopenmp




vertclair=\033[1;32m
neutre=\033[0;m
cyanfonce=\033[0;36m

info="[${vertclair}INFO${neutre}]"
eval="[${cyanfonce}EVAL${neutre}]"

GBLX=glibx-connex
#name_of_gblx
FRANCOIS=sprint-modif-zipf.isp
WORST_SPRINT=sprint-modif-zipf.isp
#sprint-modif70.isp
REALISTIC_SPRINT=geoSprint
#sprint-modif70.isp
#sprint-real.isp



all : objects best2cop


testCart : $(OBJ) test_prod_cart.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)
	@echo ${info} "testCart succesfully compiled"

scriptCstr : $(OBJ) scriptCstr.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)
	@echo ${info} "Constraint script succesfully compiled"

worstSprint : $(OBJ) worstSprint.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)	
	@echo ${info} "Worst sprint succesfully compiled"

genRandom : $(OBJ) genTopoNonAlign.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)
	@echo ${info} "Worst sprint succesfully compiled"

unitFlex : $(OBJ) unitFlex.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)
	@echo ${info} "Test unit flex succesfully compiled"

unitBinHeap : $(OBJ) BinHeap.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)
	@echo ${info} "Test unit bin heap succesfully compiled"

worstRandom : $(OBJ) worstRandom.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)
	@echo ${info} "Worst random succesfully compiled"

randomTopo : $(OBJ) randomTopo.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)
	@echo ${info} "randomTopo succesfully compiled"

randomSpread : $(OBJ) randomSpread.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)
	@echo ${info} "randomSpread succesfully compiled"

crashTest : clean objects eval

eval : $(OBJ) eval.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)
	@echo ${info} "Crash test succesfully compiled"

best2cop : $(OBJ) main.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)
	@echo ${info} "Best2cop succesfully compiled"

%.o: %.c
	$(CC) $(FLAGS) -o $@ -c $^

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(FLAGS) -o $@ -c $^


objects:
	@mkdir -p obj

clean:
	@rm -rf obj/*.o
	@rm -rf best2cop
	@rm -rf main.o
	@rm -rf eval
	@rm -rf eval.o
	@rm -rf gmon.out
	@rm -rf profiler.txt
	@rm -rf unitFlex
	@rm -rf unitFlex.o
	@rm -rf unitBinHeap
	@rm -rf scriptCstr
	@rm -rf worstRandom.o
	@rm -rf worstRandom
	@rm -rf worstSprint.o
	@rm -rf worstSprint
	@rm -rf test_prod_cart.o
	@rm -rf testCart
	@echo ${info} "Main program clean succesfully done"


ct_nodesdirs:
	@mkdir -p CT_NODES

CT_NODES: crashTest ct_nodesdirs
	@echo ${eval} "Start eval for Ct_nodes"
	@echo "NB_NODES NB_PL NB_THREADS TIME" > CT_NODES/results.txt
	@for i in 100 200 300 400 500 600 700 800 900 1000 ; do \git commit -m "fix segfault on gen random"
		./eval $$i 2 1000 12 >> CT_NODES/results.txt ; \
	done;
	@echo ${eval} "Eval for Ct_nodes completed"

ct_nodesclean:
	@rm -rf CT_NODES
	@rm -rf eval
	@rm -rf eval.o


ct_threadsdirs:
	@mkdir -p CT_THREADS

CT_THREADS: crashTest ct_threadsdirs
	@echo ${eval} "Start eval for Ct_threads"
	@echo "NB_NODES NB_PL NB_THREADS TIME" > CT_THREADS/results.txt
	@for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 ; do \
		./eval 1000 2 1000 $$i >> CT_THREADS/results.txt ; \
	done;
	@echo ${eval} "Eval for Ct_threads completed"

ct_threadsclean:
	@rm -rf CT_THREADS
	@rm -rf eval
	@rm -rf eval.o

rnddirs:
	@for i in 100 500 1000 ; do \
		for j in $$(seq 1 10) ; do \
			mkdir -p RND/resultsSpread$$i/resultsTime$$(($$j * 100)) ; \
		done; \
	done;

rnddirsGen:
	@for i in 100 500 1000 ; do \
		for j in $$(seq 1 10) ; do \
			mkdir -p RND/resultsSpread$$i/topo$$(($$j * 100)) ; \
		done; \
	done;

RND: rnddirs randomTopo
	@echo ${eval} "Start eval for RND"
	@./randomTopo 100
	@./randomTopo 500
	@./randomTopo 1000
	@echo ${eval} "Eval for RND completed"

rndclean:
	@rm -rf RND
	@rm -rf randomTopo
	@rm -rf randomTopo.o

rltdirs:
	@mkdir -p RLT

RLT: best2cop rltdirs
	@echo ${eval} "Start eval for RLT"
	@./best2cop --file topos/${WORST_SPRINT} --topo --labels --all-nodes --output RLT/worst_sprint_res.txt --threads 12
	#@./best2cop --file topos/${FRANCOIS} --topo --labels --all-nodes --output --threads 12 RLT/topo_francois_res.txt
	@./best2cop --file topos/${GBLX} --topo --labels --all-nodes --output RLT/gblx_res.txt --threads 12
	@echo ${eval} "Eval for RLT completed"

rltclean:
	@rm -rf RLT
	@rm -rf best2cop
	@rm -rf main.o


nbsegdirs:
	@mkdir -p NBSEG

NBSEG: nbsegdirs best2cop
	@echo ${eval} "Start eval for NBSEG"
	@for i in 5 10 50 100 ; do \
		echo -e ${eval} "Start eval for $$i"; \
		./best2cop --file sr_topo21.isp --sr --id --cstr1 $$(($$i * 10)) --accu 0 --all-nodes --DCLC --output NBSEG/res_rand_dclc$$i.txt; \
		./best2cop --file ${REALISTIC_SPRINT} --topo --labels --cstr1 $$i --all-nodes --DCLC --output NBSEG/res_sprint_dclc$$i.txt; \
	done;
	@echo ${eval} "Eval for NBSEG completed"

FOR_FRANCOIS: best2cop nbsegdirs scriptCstr rltdirs
	@echo ${eval} "Start eval for FRANCOIS"
	@./best2cop --file topos/${FRANCOIS} --topo --labels --all-nodes --output RLT/topo_francois_res.txt --threads 12
	@./scriptCstr topos/${FRANCOIS} NBSEG/topo_francois_res.txt
	@echo ${eval} "Eval for FRANCOIS completed"

nbsegclean:
	@rm -rf NBSEG
	@rm -rf main.o
	@rm -rf best2cop

cleanFigures: ct_nodesclean ct_threadsclean rndclean rltclean nbsegclean

allFigures: CT_NODES CT_THREADS RND RLT NBSEG
