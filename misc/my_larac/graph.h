#include <vector>
#include <set>
#include <map>
#include <utility>

#if 0
class Id
{
public:
  const unsigned int GRAPH_NULL_ID = static_cast<unsigned int>(-1);
protected:
  Id(unsigned int id = 0) : id_ (id) {}
  Id(const Id& ID) : id_(ID.id_) {}
private:
  unsigned int id_;
};

class Eid : public Id
{
public:
  Eid(unsigned int id = 0) : Id(id) {}
};
#endif

typedef unsigned int uint;

class Graph
{
public:
  static const uint GRAPH_NULL_ID = static_cast<uint>(-1);
  
  Graph(uint node_num) : node_num_(node_num), link_num_(0) {
    adjs_.resize(node_num);
  }
  
  uint getLinkNum() const { return link_num_; }
  uint getNodeNum() const { return node_num_; }
  uint getSrc(uint link) const { return link < link_num_ ? links_[link].first : GRAPH_NULL_ID; }
  uint getDst(uint link) const { return link < link_num_ ? links_[link].second : GRAPH_NULL_ID; }
  
  uint addEdge(uint src, uint dst) {
    if(src >= node_num_ || dst >= node_num_)
      return GRAPH_NULL_ID;
      
    auto & adj = adjs_[src];
    if(adj.find(dst) != adj.end())
      return GRAPH_NULL_ID;
     
    adj.insert(dst);
     
    links_.push_back(std::make_pair(src, dst));
    
    auto link_id = link_num_++;
    rlinks_[std::make_pair(src, dst)] = link_id;
    
    return link_id;
  }
  
  uint getEdge(uint src, uint dst) const {
    auto i = rlinks_.find(std::make_pair(src, dst));
    return i != rlinks_.end() ? i->second : GRAPH_NULL_ID;
  }
  
  const std::set<uint>& getAdj(uint src) const {
    return (src < node_num_) ? adjs_[src] : empty_adj_;
  }

private:
  uint node_num_;
  uint link_num_;
  std::vector<std::pair<uint, uint>> links_; // eid -> (nid_src, nid_dst)
  std::map<std::pair<uint, uint>, uint> rlinks_; // (nid_src, nid_dst) -> eid
  std::vector<std::set<uint>> adjs_;
  const std::set<uint> empty_adj_;
};
