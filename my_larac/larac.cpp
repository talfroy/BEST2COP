#include "graph.h"
#include <vector>
#include <list>
#include <map>
#include <set>
#include <assert.h>
#include <iostream>
#include <math.h>



bool dijkstra(const Graph& g, uint src, uint dst, const std::vector<double>& w, std::list<uint>& path)
{
  uint node_num = g.getNodeNum();
  uint link_num = g.getLinkNum();
  
  if(src >= node_num || dst >= node_num || w.size() != link_num)
    return false;
    
  std::set<std::pair<double, uint>> setds; // <distance, dest>
  std::map<uint, double> dist;
  std::map<uint, uint> pre; // <node, precursor-node-in-spf-tree>
  std::set<uint> visited;
  
  setds.insert(std::make_pair(0.0, src));
  
  bool found = false;
  while(!setds.empty())
  {
    auto i = setds.begin();
    auto distance = i->first;
    auto node = i->second;
    visited.insert(node);
    dist[node] = distance;
    
    if(node == dst)
    {
      found = true;
      break;
    }
    
    setds.erase(i);
    
    auto adj = g.getAdj(node);
    for(auto peer : adj)
    {
      if(visited.find(peer) != visited.end())
        continue;
      
      // std::cout << "(" << node << ", " << peer << ")" << std::endl;
       
      auto j = dist.find(peer);
      auto e = g.getEdge(node, peer);
      assert(e != Graph::GRAPH_NULL_ID);
      
      double dist2 = distance + w[e];
      if(j == dist.end() || j->second > dist2)
      {
        if(j != dist.end())
          setds.erase(std::make_pair(j->second, peer));
        
        setds.insert(std::make_pair(dist2, peer));
         
        dist[peer] = dist2;
        pre[peer] = node;
      }
    }
  } // while(!setds.empty())

  path.clear();
  
  if(found)
  {
    uint suc = dst;
    for(auto k = pre[dst]; k != src; k = pre[k])
    {
      path.push_front(g.getEdge(k, suc));
      suc = k;
    }
    path.push_front(g.getEdge(src, suc));
  }
  
  return found;
}

double cost(const std::list<uint>& path, const std::vector<double>& w)
{
   uint link_num = w.size();
   double c = 0.0;
   for(auto e : path)
   {
     assert(e < link_num);
     c += w[e];
   }
   
   return c;
}

void dump_path(const Graph& g, const std::list<uint>& path, uint shift = 0)
{
  auto e = path.front();
  std::cout << g.getSrc(e) + shift;
  for(auto e : path)
    std::cout << " " << g.getDst(e) + shift;
  std::cout << std::endl;
}

bool larac(const Graph& g, uint src, uint dst, const std::vector<double>& w, const std::vector<double>& d, 
  double max_delay, std::list<uint>& path)
{
  uint link_num = g.getLinkNum();
  
  std::vector<double> w_lamda;
  w_lamda.assign(w.size(), 0.0);
  
  std::list<uint> path_c;
  if(!dijkstra(g, src, dst, w, path_c))
    return false;
  
  double delay_c = cost(path_c, d);
  
  path.clear();
  if(delay_c <= max_delay)
  {
    //std::cout << "min cost path ok" << std::endl;
    path = path_c;
    return true;
  }
  
  std::list<uint> path_d;
  if(!dijkstra(g, src, dst, d, path_d))
    return false;
    
  double delay_d = cost(path_d, d);
  if(delay_d > max_delay)
  {
    //std::cout << "min delay path bad" << std::endl;
    return false;
  }
  
  int count_dijkstra = 2;
  
  for(;;)
  {
    double cost_c = cost(path_c, w);
    double cost_d = cost(path_d, w);
    double delay_c = cost(path_c, d);
    double delay_d = cost(path_d, d);
  
    double lamda = (cost_c - cost_d) / (delay_d - delay_c);
    //std::cout << "lamda: " << lamda << std::endl;
    
    for(uint i = 0; i < link_num; i++)
      w_lamda[i] = w[i] + lamda * d[i];
    
    std::list<uint> path_r;
    if(!dijkstra(g, src, dst, w_lamda, path_r))
      return false;
    
    count_dijkstra++;
   
    double cost_lamda_c = cost(path_c, w_lamda);
    double cost_lamda_r = cost(path_r, w_lamda);
    double cost_lamda_d = cost(path_d, w_lamda);
    if(cost_lamda_c == cost_lamda_r || cost_lamda_d == cost_lamda_r )
    {
      break;
    }
    
    double delay_r = cost(path_r, d);
    //std::cout << "delay_r : " << delay_r << std::endl;
    
    if(delay_r <= max_delay)
    {
      path_d = path_r;
      cost_d = cost(path_r, w);
      
      delay_d = delay_r;
    }
    else
    {
      path_c = path_r;
      cost_c = cost(path_r, w);
       
      delay_c = delay_r;
    }

/*    
    double gap = cost_d / cost_c - 1.0;
    std::cout << "gap-ratio: " << gap << std::endl;
    if(gap < 0.01)
      break;
*/
  }

  path = path_d;
  
  return true;
}

int main()
{
  //               (1,1)
  //          [1] ---------> [3]
  //          ^^ \ (2,3)    ^ ^  \ (1,7)
  //  (1,10) / |   \       /  |    \
  //        /  |     \   /    |      v
  //      [0]  |(1,2)  x  (10,|1)   [5]
  //        \  |     /  \     |      ^
  //  (10,3) \ |   /      \   |    /
  //          v| /(5,7)     v |  / (2,2)
  //          [2] ---------> [4]
  //               (12,3)
  const uint N = 6, L = 10;
  Graph g(N);
  g.addEdge(0, 1); // link #0
  g.addEdge(0, 2);
  g.addEdge(1, 3);
  g.addEdge(1, 4);
  g.addEdge(2, 1);
  g.addEdge(2, 3);
  g.addEdge(2, 4);
  g.addEdge(3, 5);
  g.addEdge(4, 3);
  g.addEdge(4, 5); // link #9
  
  uint link_num = g.getLinkNum();
  
  std::cout << "nodes: " << g.getNodeNum() << ", links: " << g.getLinkNum() << std::endl;
  
  std::vector<double> w, d;
  // costs
  w.push_back(1.0);
  w.push_back(10.0);
  w.push_back(1.0);
  w.push_back(2.0);
  w.push_back(1.0);
  w.push_back(5.0);
  w.push_back(12.0);
  w.push_back(1.0);
  w.push_back(10.0);
  w.push_back(2.0);
  
  // delay
  d.push_back(10.0);
  d.push_back(3.0);
  d.push_back(1.0);
  d.push_back(3.0);
  d.push_back(2.0);
  d.push_back(7.0);
  d.push_back(3.0);
  d.push_back(7.0);
  d.push_back(1.0);
  d.push_back(2.0);
  
  std::list<uint> path;
  
  for(int delay = 8; delay <= 18; delay++)
  {
	  if(larac(g, 0, 5, w, d, delay, path))
	  {
		std::cout << "max delay: " << delay << ", path_d is best path:";
		dump_path(g, path, 0);
	  }
  }
  
  return 0;
}
