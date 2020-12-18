
//number max of segments
#define SEG_MAX                 10

//max spread of the weight in SR graph
#define MAX_SPREAD              1000
#define MAX_SIZE                SEG_MAX * MAX_SPREAD

//Limit to the sort of the actives indexes
#define REDUC_LIMIT             1200

#define DICT_LIMIT              15000
#define DIABLO                  0
#define ISP                     1
#define TOPOLOGY_TYPE           ISP

//define which type of topology will be load
#if TOPOLOGY_TYPE == DIABLO

typedef long int my_m2;
typedef long int my_m1;

#endif


#if TOPOLOGY_TYPE == ISP

typedef int my_m2;
typedef int my_m1;

#endif


#define ARRAY_HEAP              0
#define PFRONT_HEAP             1

//define which type of heap will be used
#define HEAP_TYPE               ARRAY_HEAP

#define DIABLO_POWERTEN         0
#define DIABLO_65               1


#define MAX_SR_GRAPH_SIZE       1500


#define BI_DIRECTIONNAL         1
#define UNI_DIRECTIANNAL        0

#define LOAD_TOPO               0
#define LOAD_SR                 1
#define LOAD_LABELS             0
#define LOAD_IDS                1


#define NB_THREADS              4
