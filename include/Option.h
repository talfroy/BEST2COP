#ifndef __OPTION_H__
#define __OPTION_H__

#include "base.h"
#include "SrGraph.h"
#include "Dict.h"



/**
 * @brief This structure represents all the availables Options
 */

struct Options {
    char loadingMode;       /**< define if you load a topology or directly a SR Graph */
    char labelsOrId;        /**< define if the nodes are identified by a label or an id */
    my_m1 cstr1;            /**< constraint on first component (delay) */
    my_m2 cstr2;            /**< constraint on second component (IGP weight) */
    char* filename;         /**< file to load */
    char* output;           /**< output file */
    int accuracy;           /**< delay trueness (1 means that accuracy is about 0.1ms) */
    char biDir;             /**< define if the links are uni-directionals or bi */
    char analyse;              /**< use to put SEG_MAX to INF*/
    int src;                /**< source node */
    char allNodes;          /**< define if we need to mesure on all nodes */
    char interface;         /**< activate the interactive mode */
    char nbThreads;         /**< set the number of threads to use (max_thread by default) */
    char randomTopo;            
    bool flex;              /**< enable flex algo (disble by default) */
};

struct Options opt;


/**
 * @brief fill the corrects Options. Use ./bellmanFork --help
 * to see the availables Options
 *
 * @param argc          number of arguments on the command line
 * @param argv          arguments
 * @return              return 1 if all the Options are available, -1 if not
 **/

int Option_command_parser(int argc, char** argv);


/**
 * @brief display user manual
 * @param name          programme name (argv[0])
 **/

void usage(char* name);


#endif
