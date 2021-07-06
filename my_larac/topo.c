#include <stdio.h>
#include <stdlib.h>


#define START_MAIN \
"#include \"graph.h\"\n\
#include <vector>\n\
#include <list>\n\
#include <map>\n\
#include <set>\n\
#include <assert.h>\n\
#include <iostream>\n\
#include <math.h>\n\
\n\
bool dijkstra(const Graph& g, uint src, uint dst, const std::vector<double>& w, std::list<uint>& path)\n\
{\n\
  uint node_num = g.getNodeNum();\n\
  uint link_num = g.getLinkNum();\n\
  \n\
  if(src >= node_num || dst >= node_num || w.size() != link_num)\n\
    return false;\n\
    \n\
  std::set<std::pair<double, uint>> setds;\n\
  std::map<uint, double> dist;\n\
  std::map<uint, uint> pre; // <node, precursor-node-in-spf-tree>\n\
  std::set<uint> visited;\n\
    \n\
  setds.insert(std::make_pair(0.0, src));\n\
  \n\
  bool found = false;\n\
  while(!setds.empty())\n\
  {\n\
    auto i = setds.begin();\n\
    auto distance = i->first;\n\
    auto node = i->second;\n\
    visited.insert(node);\n\
    dist[node] = distance;\n\
    \n\
    if(node == dst)\n\
    {\n\
      found = true;\n\
      break;\n\
    }\n\
    \n\
    setds.erase(i);\n\
    \n\
    auto adj = g.getAdj(node);\n\
    for(auto peer : adj)\n\
    {\n\
      if(visited.find(peer) != visited.end())\n\
        continue;\n\
       \n\
      auto j = dist.find(peer);\n\
      auto e = g.getEdge(node, peer);\n\
      assert(e != Graph::GRAPH_NULL_ID);\n\
      \n\
      double dist2 = distance + w[e];\n\
      if(j == dist.end() || j->second > dist2)\n\
      {\n\
        if(j != dist.end())\n\
          setds.erase(std::make_pair(j->second, peer));\n\
        \n\
        setds.insert(std::make_pair(dist2, peer));\n\
         \n\
        dist[peer] = dist2;\n\
        pre[peer] = node;\n\
      }\n\
    }\n\
  }\n\
\n\
  path.clear();\n\
  \n\
  if(found)\n\
  {\n\
    uint suc = dst;\n\
    for(auto k = pre[dst]; k != src; k = pre[k])\n\
    {\n\
      path.push_front(g.getEdge(k, suc));\n\
      suc = k;\n\
    }\n\
    path.push_front(g.getEdge(src, suc));\n\
  }\n\
  \n\
  return found;\n\
}\n\
\n\
double cost(const std::list<uint>& path, const std::vector<double>& w)\n\
{\n\
   uint link_num = w.size();\n\
   double c = 0.0;\n\
   for(auto e : path)\n\
   {\n\
     assert(e < link_num);\n\
     c += w[e];\n\
   }\n\
   \n\
   return c;\n\
}\n\
\n\
void dump_path(const Graph& g, const std::list<uint>& path, uint shift = 0)\n\
{\n\
  auto e = path.front();\n\
  std::cout << g.getSrc(e) + shift;\n\
  for(auto e : path)\n\
    std::cout << \" \" << g.getDst(e) + shift;\n\
  std::cout << std::endl;\n\
}\n\
\n\
bool larac(const Graph& g, uint src, uint dst, const std::vector<double>& w, const std::vector<double>& d,\n \
  double max_delay, std::list<uint>& path)\n\
{\n\
  uint link_num = g.getLinkNum();\n\
  \n\
  std::vector<double> w_lamda;\n\
  w_lamda.assign(w.size(), 0.0);\n\
  \n\
  std::list<uint> path_c;\n\
  if(!dijkstra(g, src, dst, w, path_c))\n\
    return false;\n\
  \n\
  double delay_c = cost(path_c, d);\n\
  \n\
  path.clear();\n\
  if(delay_c <= max_delay)\n\
  {\n\
    //std::cout << \"min cost path ok\" << std::endl;\n\
    path = path_c;\n\
    return true;\n\
  }\n\
  \n\
  std::list<uint> path_d;\n\
  if(!dijkstra(g, src, dst, d, path_d))\n\
    return false;\n\
\n\
  double delay_d = cost(path_d, d);\n\
  if(delay_d > max_delay)\n\
  {\n\
    //std::cout << \"min delay path bad\" << std::endl;\n\
    return false;\n\
  }\n\
  \n\
  int count_dijkstra = 2;\n\
  \n\
  for(;;)\n\
  {\n\
    double cost_c = cost(path_c, w);\n\
    double cost_d = cost(path_d, w);\n\
    double delay_c = cost(path_c, d);\n\
    double delay_d = cost(path_d, d);\n\
  \n\
    double lamda = (cost_c - cost_d) / (delay_d - delay_c);\n\
    //std::cout << \"lamda: \" << lamda << std::endl;\n\
    \n\
    for(uint i = 0; i < link_num; i++)\n\
      w_lamda[i] = w[i] + lamda * d[i];\n\
    \n\
    std::list<uint> path_r;\n\
    if(!dijkstra(g, src, dst, w_lamda, path_r))\n\
      return false;\n\
    \n\
    count_dijkstra++;\n\
   \n\
    double cost_lamda_c = cost(path_c, w_lamda);\n\
    double cost_lamda_r = cost(path_r, w_lamda);\n\
    double cost_lamda_d = cost(path_d, w_lamda);\n\
    if(cost_lamda_c == cost_lamda_r || cost_lamda_d == cost_lamda_r )\n\
    {\n\
      break;\n\
    }\n\
    \n\
    double delay_r = cost(path_r, d);\n\
    \n\
    if(delay_r <= max_delay)\n\
    {\n\
      path_d = path_r;\n\
      cost_d = cost(path_r, w);\n\
      \n\
      delay_d = delay_r;\n\
    }\n\
    else\n\
    {\n\
      path_c = path_r;\n\
      cost_c = cost(path_r, w);\n\
\n\
      delay_c = delay_r;\n\
    }\n\
\n\
  }\n\
\n\
  path = path_d;\n\
  \n\
  return true;\n\
}\n\
\n\
int main()\n\
{\n\
  const uint N = 6, L = 10;\n\
  Graph g(N);\n\
"

#define MID_LARNAC \
"\n\
  uint link_num = g.getLinkNum();\n\
  \n\
  std::cout << \"nodes: \" << g.getNodeNum() << \", links: \" << g.getLinkNum() << std::endl;\n\
  \n\
  std::vector<double> w, d;\n\
  // costs\"\n\
\n"

#define END_LARNAC \
"    std::list<uint> path;\n\
  \n\
  for(int delay = 8; delay <= 18; delay++)\n\
  {\n\
	  if(larac(g, 0, 5, w, d, delay, path))\n\
	  {\n\
		std::cout << \"max delay: \" << delay << \", path_d is best path:\";\n\
		dump_path(g, path, 0);\n\
	  }\n\
  }\n\
 \n\
  return 0;\n\
}\n"


#define NEW_EDGE(src, dst) \
"  g.addEdge(%d ,%d);\n", src, dst

#define NEW_COST(cost) \
"  w.push_back(%.2f);\n", cost

#define NEW_DELAY(delay) \
"  d.push_back(%.2f);\n", delay

int main (int argc, char **argv)
{
    if (argc != 3) {
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "w");


    int src, dst;
    double m2;
    char line[1024];
    double m1;
    int nbNode = 0;
    int nbLine = 1;
    int area;

    fprintf(output, START_MAIN);

    while (fgets(line, 1024,  file))
    {
        if (sscanf(line, "%d %d %lf %lf\n", &src, &dst, &m1, &m2) == 4) {
            fprintf(output, NEW_EDGE(src, dst));
        }
    }

    fprintf(output, MID_LARNAC);

    fclose(file);
    file = fopen(argv[1], "r");
    
    while (fgets(line, 1024,  file))
    {
        if (sscanf(line, "%d %d %lf %lf\n", &src, &dst, &m1, &m2) == 4) {
            fprintf(output, NEW_COST(m2));
            fprintf(output, NEW_DELAY(m1));
        }
    }

    fprintf(output, END_LARNAC);

    fclose(output);
    fclose(file);

    return 0;
}