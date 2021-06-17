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
        {"areas",    required_argument, NULL,       'A'},
        {"src-lab",     required_argument,      NULL,       'B'},
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
    opt.nb_areas = 0;
    opt.src_lab = NULL;


    while ((optValue = getopt_long(argc, argv, optString, long_options, NULL)) != -1) {
        switch(optValue)
        {
            case 'B' :
                opt.src_lab = optarg;
                break;

            case 'A' :
                opt.loadingMode = LOAD_TOPO_AREAS;
                opt.nb_areas = atoi(optarg);
                break;
                
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
                if (opt.loadingMode != -1) {
                    WARNING("You are going to overide file loading mode with option --topo\n");
                }
                opt.loadingMode = LOAD_TOPO;
                break;

            case 's' :
                //we are going to load directly a SR Graph
                if (opt.loadingMode != -1) {
                    WARNING("You are going to overide file loading mode with option --sr\n");
                }
                opt.loadingMode = LOAD_SR;
                break;
            
            case 'L' :
                if (opt.loadingMode != -1) {
                    WARNING("You are going to overide file loading mode with option --sr-bin\n");
                }
                opt.loadingMode = LOAD_SR_BIN;
                break;

            case 'f' :
                //set the input file name
                opt.filename = optarg;
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

            case 'l' :
                //the nodes in the file are identified by labels
                if (opt.labelsOrId != -1) {
                    WARNING("You are going to overide the node loading mode with option --labels\n");
                }
                opt.labelsOrId = LOAD_LABELS;
                break;

            case 'i' :
                //the nodes in the file are identified by IDs
                if (opt.labelsOrId != -1) {
                    WARNING("You are going to overide the node loading mode with option --id\n");
                }
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
                if (opt.analyse != -1) {
                    WARNING("You are going to overide the analyse type with option --2COP\n");
                }
                opt.analyse = ANALYSE_2COP;
                break;

            case 'w' :
                if (opt.analyse != -1) {
                    WARNING("You are going to overide the analyse type with option --DCLC\n");
                }
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

    if (!opt.allNodes && opt.src == -1 && !opt.nb_areas && !opt.src_lab) {
        ERROR("Please provide option --src [node] or --all-nodes\n");
        return -1;
    }

    if (opt.filename == NULL) {
        ERROR("Please provide option --file with the topo file\n");
        return -1;
    }

    switch(opt.loadingMode)
    {
        case LOAD_TOPO:
            if (opt.loadingMode == -1) {
                ERROR("You tried to load a topology file, please specify if nodes are labels or ids\n");
                return -1;
            }
            break;

        case LOAD_SR:
            if (opt.loadingMode == -1) {
                ERROR("You tried to load a SR graph file, please specify if nodes are labels or ids\n");
                return -1;
            }
            break;

        case LOAD_SR_BIN:
            break;

        case LOAD_TOPO_AREAS:
            break;

        default:
            ERROR("Please profide the loading method in command line\n");
            return -1;
    }

    return 0;
}


void usage(char* name)
{
    printf ("Usage:\n");
	printf ("\t%s [options] --file input_file --loading_mode --execution_mode\n", name);

    printf("\nloading mode :\n" \
           "--topo\t\tUsed to specifie that the loaded file represents the topology\n" \
           "--sr\t\tUsed to specifie that the loaded file represents the Segment Routing Graph\n" \
           "--sr-bin\t\tUsed to specifie that the loaded file represents the Segment Routing Graph in binary format\n" \
           "");

    printf("\nOptions :\n"           \
    "\t--accu [value]\t\t\tSet the accuracy of delay to value. By default this parameter is 1 (0.1ms)\n"   \
    "\t--output [output_file]\t\tThe results are printed in output_file. The file is created if it doesn't exists\n" \
    "\t--bi_dir\t\t\tIf this option is activated, the link represented in input file are bi_directionals\n" \
    "\t--cstr1 [value]\t\t\tPut the first constraint to value\n" \
    "\t--cstr2 [value]\t\t\tPut the second constraint to value\n" \
    "\t--full\t\t\t\tPut SEGMAX to infinity and count the iterations instead of the execution time\n" \
    "\t--interface\t\t\tLauch the interactive mode\n" );
}
