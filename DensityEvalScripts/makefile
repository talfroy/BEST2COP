SRCDIR=../src
OBJDIR=../obj
SRC := $(wildcard $(SRCDIR)/*.c)
OBJ  = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
# -pg 
# -p -g on mac

CC ?= gcc-11

LIBS := -lm -fopenmp
FLAGS := -Wall -Wextra -Werror -O3 -fopenmp


vertclair=\033[1;32m
neutre=\033[0;m
cyanfonce=\033[0;36m

info="[${vertclair}INFO${neutre}]"
eval="[${cyanfonce}EVAL${neutre}]"
XPNAME="DENSITY"

all : objects 

genTopo : $(OBJ) genTopoDensity.o
	$(CC) $(FLAGS) -I../include -o $@ $^ $(LIBS)
	@echo ${info} "Topology Generator compiled"

runXP : $(OBJ) densityTopo.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)
	@echo ${info} "Exp script compiled"

%.o: %.c
	$(CC) $(FLAGS) -I../include -o $@ -c $^

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(FLAGS) -I../include -o $@ -c $^

objects:
	@mkdir -p obj

clean:
	@rm -rf *.o
	@echo ${info} "Main program clean succesfully done"

xpdirs:
	@for i in 100 500 1000 ; do \
		for j in `seq 1 10 100` ; do \
			echo $$j;\
			mkdir -p $$XPNAME/resultsSpread$$i/resultsTime$$j ; \
			mkdir -p $$XPNAME/resultsSpread$$i/topo$$j ; \
		done; \
	done;

# RND: rnddirs randomTopo
# 	@echo ${eval} "Start eval for RND"
# 	@./randomTopo 100
# 	@./randomTopo 500
# 	@./randomTopo 1000
# 	@echo ${eval} "Eval for RND completed"


