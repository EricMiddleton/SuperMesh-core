#include "Router.hpp"

#include <algorithm>

#include "util.hpp"

namespace OLSR {

Router::Router(NodeID myAddr, const std::vector<Interface>& interfaces, const DataHandler& handler,
  std::unique_ptr<ITimer>&& timer)
  : m_myAddr{myAddr}
  , m_interfaces(interfaces)
  , m_dataHandler{handler}
  , m_timer{std::move(timer)} {

/*
  //Configure and start update timer
  m_timer->setPeriod(UPDATE_PERIOD);
  m_timer->setFn([this](){ update(); });
  m_timer->start();
*/
}

void Router::update() {
  //Send Hello messages to neighbors
  sendHello();

  //Send TC messages
  sendTC();

  //Prune tuple sets (remove stale entries)
  pruneSets();
}

void Router::pruneSets() {
  //Prune link set
  for(int i = 0; i < m_linkSet.size(); ++i) {
    m_linkSet[i].time--;
    if(m_linkSet[i].time <= 0) {
      m_linkSet.erase(m_linkSet.begin()+i);
      --i;
    }
  }

  bool updateMPRs = false;

  //Prune neighbor set
  for(int i = 0; i < m_neighborSet.size(); ++i) {
    //Find associated links
    auto addr = m_neighborSet[i].main_addr;
    auto link = std::find_if(m_linkSet.begin(), m_linkSet.end(), [addr](const LinkTuple& lt) {
      return lt.neighbor_iface_addr == addr;
    });

    if(link == m_linkSet.end()) {
      //Neighbor has no links, remove
      m_neighborSet.erase(m_neighborSet.begin()+i);
      
      //Remove all two-hop entries that reference this neighbor
      for(int j = 0; j < m_twoHopSet.size(); ++j) {
        if(m_twoHopSet[j].main_addr == addr) {
          m_twoHopSet.erase(m_twoHopSet.begin()+j);
          --j;
        }
      }

      //Remove all MPR selector entries that reference this neighbor
      for(int j = 0; j < m_mprSelectorSet.size(); ++j) {
        if(m_mprSelectorSet[j].main_addr == addr) {
          m_mprSelectorSet.erase(m_mprSelectorSet.begin()+j);
          --j;
        }
      }

      --i;
      updateMPRs = true;
    }
  }

  //Update MPR set
  if(updateMPRs) {
    updateMPRSet();
  }
}

void Router::updateMPRSet() {
  struct Node {
    NodeID addr;
    std::vector<NodeID> edges;
  };

  //Clear MPR set
  m_mprSet.clear();

  //MPR selection graph
  std::vector<Node> selectionGraph;

  //Place Nodes in graph
  for(const auto& neighbor : m_neighborSet) {
    selectionGraph.push_back({neighbor.main_addr, {}});
  }

  //Populate adjacency lists
  for(const auto& twoHopNeighbor : m_twoHopSet) {
    auto addr = twoHopNeighbor.main_addr;
    auto node = std::find_if(selectionGraph.begin(), selectionGraph.end(),
      [addr](const Node& n) {
        return n.addr == addr;
      });
    node->edges.push_back(twoHopNeighbor.twohop_addr);
  }

  //Repeatedly extract the node with the most edges until the graph is empty
  while(!selectionGraph.empty()) {
    auto maxNode = std::max_element(selectionGraph.begin(), selectionGraph.end(),
      [](const Node& n1, const Node& n2) {
        return n1.edges.size() < n2.edges.size();
      });

    //Place node in MPR set
    m_mprSet[maxNode->addr] = true;

    //Remove node's two hop neighbors from graph
    size_t thisIndex = maxNode - selectionGraph.begin();
    for(int i = 0; i < selectionGraph.size(); ++i) {
      if(i == thisIndex) {
        continue;
      }
      
      //For each two hop edge in max node
      for(const auto& twoHop : maxNode->edges) {
        auto found = std::find(selectionGraph[i].edges.begin(), selectionGraph[i].edges.end(),
          twoHop);
        //Remove it from current node if found
        if(found != selectionGraph[i].edges.end()) {
          selectionGraph[i].edges.erase(found);
        }
      }

      //Remove empty nodes
      if(selectionGraph[i].edges.empty()) {
        selectionGraph.erase(selectionGraph.begin() + i);
      }
    }
  }
}

void Router::sendHello() {
  LinkInfo mpr{Hello::MPR_NEIGH, {}}, sym{Hello::SYM_NEIGH, {}};

  for(const auto& neighbor : m_neighborSet) {
    if(m_mprSet.count(neighbor.main_addr) > 0) {
      mpr.neighborInterfaces.push_back(neighbor.main_addr);
    }
    else {
      sym.neighborInterfaces.push_back(neighbor.main_addr);
    }
  }

  std::vector<LinkInfo> links;

  if(!mpr.neighborInterfaces.empty()) {
    links.emplace_back(std::move(mpr));
  }
  if(!sym.neighborInterfaces.empty()) {
    links.emplace_back(std::move(sym));
  }

  //Construct Hello message data
  Hello msgData{0, 0, links};
  
  //Construct Hello message
  Message msg{msgData};

  //Broadcast Hello message on all interfaces
  send(BroadcastAddress, msg);
}

void Router::sendTC() {
  //TODO
}

void Router::process(Packet&& p) {
  for(auto& msg : p) {
    //Find duplicate tuple entry for message
    auto* duplicate = duplicateExists(p.sendInterface(), msg.seqNum());

    //Processing condition
    if( (msg.ttl() > 0) && !duplicate ) {
      process(msg);


    }

    //Forwarding condition
    if(!duplicate || !addressInList(duplicate->D_iface_list, p.recvInterface())) {
      forward(msg, duplicate);
    }

    //Place in duplicate set
    //m_duplicateSet.emplace_back(p.originator(), msg.seqNum(), false, {p.recvInterface()}, 0);
  }
}

void Router::process(Message& m) {
  switch(m.messageType()) {
    case Message::Type::Hello: {
      auto* helloData = reinterpret_cast<Hello*>(m.payload());

      //Find entry in link set for the sender of this packet
      auto source = m.packet()->sendInterface();
      auto entry = std::find_if(m_linkSet.begin(), m_linkSet.end(), [source](const LinkTuple& lt) {
        return lt.neighbor_iface_addr == source;
      });
      bool newLink = entry == m_linkSet.end();
      bool updateMPRs = false;
      
      //Create new entry
      if(newLink) {
        m_linkSet.push_back({m.packet()->recvInterface(), source, 0, 0, TIMEOUT});
        entry = m_linkSet.end()-1;
        updateMPRs = true;
      }
      //Update old entry
      else {
        entry->time = TIMEOUT;
      }

      //Find entry in neighbor set for sender of this packet
      auto neighborEntry = std::find_if(m_neighborSet.begin(), m_neighborSet.end(),
        [source](const NeighborTuple& nt) {
          return nt.main_addr == source;
        });

      if(neighborEntry == m_neighborSet.end()) {
        m_neighborSet.push_back({source, 0, helloData->willingness()});
        updateMPRs = true;
      }

      //Update 2-hop neighbor set
      for(int i = 0; i < helloData->countLinks(); ++i) {
        auto linkInfo = helloData->getLinkInfo(i);
        for(const auto& addr : linkInfo.neighborInterfaces) {

          if(addr == m_myAddr) {
            if(linkInfo.linkCode == Hello::MPR_NEIGH) {
              //The source node is an MPR selector
              auto mprSelectorEntry = std::find_if(m_mprSelectorSet.begin(), m_mprSelectorSet.end(),
                [source](const MPRSelectorTuple& mst) {
                  return mst.main_addr == source;
                });
              //Create new entry
              if(mprSelectorEntry == m_mprSelectorSet.end()) {
                m_mprSelectorSet.push_back({source, TIMEOUT});
              }
              //Update old entry
              else {
                mprSelectorEntry->time = TIMEOUT;
              }
            }

            continue;
          }

          //Find identical entry
          auto twoHopEntry = std::find_if(m_twoHopSet.begin(), m_twoHopSet.end(),
            [addr, source](const auto& entry) {
              return (entry.main_addr == source) && (entry.twohop_addr == addr);
            });

          //Create new entry
          if(twoHopEntry == m_twoHopSet.end()) {
            m_twoHopSet.push_back({source, addr, TIMEOUT});
            updateMPRs = true;
          }
          //Update old entry
          else {
            twoHopEntry->time = TIMEOUT;
          }
        }
      }

      //Update MPR set if link/neighbor state changes detected
      if(updateMPRs) {
        updateMPRSet();
      }

    }
    break;

    case Message::Type::TC:
      //TODO
    break;

    case Message::Type::MID:
      //TODO
    break;

    case Message::Type::HNA:
      //TODO
    break;

    case Message::Type::User:
      m_dataHandler(m);
    break;
  }
}

void Router::forward(Message& m, DuplicateTuple* duplicate) {
  //Never forward Hello messages
  if(m.messageType() == Message::Type::Hello) {
    return;
  }

  if(duplicate && duplicate->D_retransmitted) {
    //Already retransmitted, do not forward
    return;
  }
  
  bool toRetransmit = false;

  //Retransmit condition
  if(isMPRSelector(m.packet()->sendInterface()) && m.ttl() > 0) {
    toRetransmit = true;
  }

  //Update duplicate tuple entry
  if(duplicate) {
    duplicate->D_time = TIMEOUT;
    duplicate->D_iface_list.push_back(m.packet()->recvInterface());
    duplicate->D_retransmitted = toRetransmit;
  }
  //Place new entry for message in duplicate tuple set
  else {
    m_duplicateSet.push_back({m.packet()->sendInterface(), m.seqNum(), toRetransmit,
      {m.packet()->recvInterface()}, TIMEOUT});
  }

  if(toRetransmit) {
    retransmit(m);
  }
}

DuplicateTuple* Router::duplicateExists(Address originator, uint8_t seqNum) {
  auto found = std::find_if(m_duplicateSet.begin(), m_duplicateSet.end(),
    [originator, seqNum](const DuplicateTuple& dt) {
      return (dt.D_addr == originator) && (dt.D_seq_num == seqNum);
    });

  if(found == m_duplicateSet.end()) {
    return nullptr;
  }
  else {
    return &(*found);
  }
}

bool Router::addressInList(const std::vector<Address>& iface_list, Address interface) const {
  auto found = std::find_if(iface_list.begin(), iface_list.end(), [interface](const Address& addr) {
    return addr == interface;
  });

  return found != iface_list.end();
}

bool Router::isMPRSelector(NodeID addr) const {
  auto found = std::find_if(m_mprSelectorSet.begin(), m_mprSelectorSet.end(),
    [addr](const MPRSelectorTuple& mst) {
      return mst.main_addr == addr;
    });

  return found != m_mprSelectorSet.end();
}

void Router::retransmit(Message& m) {
  //Decrement TTL
  m.ttl(m.ttl()-1);
  
  //Increment hop count
  m.hopCount(m.hopCount()+1);

  //Broadcast message copy on all interfaces
  send(BroadcastAddress, m);
}

void Router::send(NodeID addr, const Message& msg) {
  if(addr == BroadcastAddress) {
    //Send broadcast on all interfaces
    for(auto& iface : m_interfaces) {
      send(iface, addr, msg);
    }
  }
  else {
    //Look up interface for addr
    Interface* iface = m_neighborIfaceMap[addr];
    Address ifaceAddr = m_neighborAddressMap[addr];

    send(*iface, ifaceAddr, msg);
  }
}

void Router::send(Interface& iface, Address ifaceAddr, const Message& msg) {
  //Create vector representing packet containing message
  std::vector<uint8_t> packet(msg.messageSize() + 4);

  //Packet length
  util::packU16(packet.data(), packet.size());
  
  //Packet sequence number
  util::packU16(packet.data()+2, iface.seqNum);
  iface.seqNum++;

  //Insert message
  packet.insert(packet.begin()+4, msg.begin(), msg.end());

  //Send through the interface
  iface.sender(ifaceAddr, std::move(packet));
}

}
