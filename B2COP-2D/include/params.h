#pragma once // Include this file only once

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

typedef struct {
    uint64_t value;
} my_m2;

#define CAST_M2(x) (x.value)
#define TO_M2(x) (my_m2){x}
#define M2_INF TO_M2(UINT64_MAX)


typedef struct {
    uint64_t value;
} my_m1;

#define CAST_M1(x) (x.value)
#define TO_M1(x) (my_m1){x}
#define M1_INF TO_M1(UINT64_MAX)


#define MAX_SR_GRAPH_SIZE       1500


#define BI_DIRECTIONNAL         1
#define UNI_DIRECTIANNAL        0

#define LOAD_TOPO               0
#define LOAD_SR                 1
#define LOAD_SR_BIN             2
#define LOAD_TOPO_AREAS         3

#define LOAD_LABELS             0
#define LOAD_IDS                1


#define NB_THREADS              4

#define LINUX                   1
#define MACOS                   2
#define OS_TYPE                 MACOS

#define SEG_MULT                3
