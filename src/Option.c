#include "../include/Option.h"


struct Options opt;

int Option_command_parser (int argc, char** argv) {
    int optValue = 0;
    const char *optString = "f:a:o:b:1:2:tslui";
	const struct option long_options[] = {
        {"file",    required_argument,      NULL,       'f'},
        {"topo" ,   no_argument,            NULL,       't'},
        {"sr",      no_argument,            NULL,       's'},
        {"labels",  no_argument,            NULL,       'l'},
        {"id",      no_argument,            NULL,       'i'},
        {"2COP",    no_argument,            NULL,       'u'},
        {"DCLC",    no_argument,            NULL,       'w'},
        {"accu",    required_argument,      NULL,       'a'},
        {"output",  required_argument,      NULL,       'o'},
        {"bi-dir",  no_argument,            NULL,       'b'},
        {"cstr1",   required_argument,      NULL,       '1'},
        {"cstr2",   required_argument,      NULL,       '2'},
        {"src",     required_argument,      NULL,       'c'},
        {"interface", no_argument,          NULL,       'd'},
        {"all-nodes", no_argument,          NULL,       'e'},
        {"help",    no_argument,            NULL,       'h'},
        {"threads", required_argument,      NULL,       'j'},
        {"flex",    no_argument,            NULL,       'r'},
        {"pretty",    no_argument,            NULL,       'p'},
        {"sr-bin" ,   no_argument,            NULL,         'L'},
        {"save-sr-bin",    required_argument, NULL,       'S'},
        {0,0,0,0}
    };

    opt.output = NULL;
    opt.saveSrGraphBin = NULL;
    opt.biDir = UNI_DIRECTIANNAL;
    opt.filename = NULL;
    opt.cstr1 = 100;
    opt.cstr2 = INT_MAX;
    opt.loadingMode = -1;
    opt.accuracy = 1;
    opt.labelsOrId = -1;
    opt.analyse = 0;
    opt.src = -1;
    opt.interface = 0;
    opt.allNodes = 0;
    opt.nbThreads = -1;
    opt.randomTopo = 0;
    opt.flex = false;
    opt.pretty = false;


    while ((optValue = getopt_long(argc, argv, optString, long_options, NULL)) != -1) {
        switch(optValue)
        {
            case 'j' :
                opt.nbThreads = atoi(optarg);
                break;

            case 'r' :
                opt.flex = true;
                break;

            case 'h' :
                return -1;
                break;

            case 'e' :
                //the computation will be done on all nodes
                opt.allNodes = 1;
                break;

            case 'd' :
                //activate the interactive mode
                opt.interface = 1;
                break;

            case 'c' :
                //set the source node
                opt.src = atoi(optarg);
                break;

            case 't' :
                //we are going to load a topology
                opt.loadingMode = LOAD_TOPO;
                break;

            case 's' :
                //we are going to load directly a SR Graph
                opt.loadingMode = LOAD_SR;
                break;
            
            case 'L' :
                opt.loadingMode = LOAD_SR_BIN;
                break;

            case 'f' :
                //set the input file name
                opt.filename = optarg;
                break;

            case 'l' :
                //the nodes in the file are identified by labels
                opt.labelsOrId = LOAD_LABELS;
                break;

            case '1' :
                //change the delay cstr value
                opt.cstr1 = atoi(optarg);
                break;

            case 'p' :
                //change the delay cstr value
                opt.pretty = true;
                break;

            case '2' :
                //change the IGP ctsr value
                opt.cstr2 = atoi(optarg);
                break;

            case 'i' :
                //the nodes in the file are identified by IDs
                opt.labelsOrId = LOAD_IDS;
                break;

            case 'o' :
                //set the output file name
                opt.output = optarg;
                break;

            case 'S' :
                //set the output file name
                opt.saveSrGraphBin = optarg;
                break;

            case 'a' :
                //set the trueness
                opt.accuracy = atoi(optarg);
                break;

            case 'b' :
                opt.biDir = BI_DIRECTIONNAL;
                break;

            case 'u' :
                opt.analyse = ANALYSE_2COP;
                break;

            case 'w' :
                opt.analyse = ANALYSE_DCLC;
                break;

			case ':':

            default :
                return -1;
                break;
        }
    }
    if (opt.interface) {
        return 0;
    }

    if (!opt.allNodes && opt.src == -1) {
        return -1;
    }

    if ((opt.filename == NULL || opt.labelsOrId == -1 || opt.loadingMode == -1) && !opt.randomTopo) {
        return -1;
    }

    return 0;
}


void usage(char* name)
{
    printf ("Usage:\n");
	printf ("\t%s [options] --file input_file --topo/--sr --labels/--id --all-nodes/--src [source node]\n", name);

    printf("\nMandatory parameters :\n" \
            "You need to use --file, --topo or --sr, --labels or --id and --all-nodes or --src\n" \
           "\t--file [filename]\t\tUse the given file as graph file\n" \
           "\t--topo/--sr \t\t\tdefine which type of loading will be choosen\n" \
           "\t--labels/--id \t\t\tdefine if src and dst in the file are represented by ids or labels\n" \
           "\t--all-nodes/-src [source]\tdefine if the main function will be lauch on all nodes or only one\n");

    printf("\nOptions :\n"           \
    "\t--accu [value]\t\t\tSet the accuracy of delay to value. By default this parameter is 1 (0.1ms)\n"   \
    "\t--output [output_file]\t\tThe results are printed in output_file. The file is created if it doesn't exists\n" \
    "\t--bi_dir\t\t\tIf this option is activated, the link represented in input file are bi_directionals\n" \
    "\t--cstr1 [value]\t\t\tPut the first constraint to value\n" \
    "\t--cstr2 [value]\t\t\tPut the second constraint to value\n" \
    "\t--full\t\t\t\tPut SEGMAX to infinity and count the iterations instead of the execution time\n" \
    "\t--interface\t\t\tLauch the interactive mode\n" );
}
