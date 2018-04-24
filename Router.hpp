#pragma once

#include <memory>
#include <vector>
#include <map>

#include "types.hpp"
#include "Packet.hpp"
#include "Message.hpp"
#include "Hello.hpp"
#include "ITimer.hpp"

namespace OLSR {
  class Router {
  public:
    Router(NodeID myAddr, const std::vector<Interface>& interfaces, const DataHandler& handler,
      std::unique_ptr<ITimer>&& timer);

    void process(Packet&& p);

    void update();

    //void updateNeighbors(const std::vector<Link>& neighbors);


  private:
    const int UPDATE_PERIOD = 1000;
    const int TIMEOUT = 5;

    //Main update loop (called from timer)
    //void update();

    void process(Message& m);
    void forward(Message& m, DuplicateTuple*);
    void retransmit(Message& m);

    void sendHello();
    void sendTC();


    //Address addr: node name address
    void send(NodeID dest, const Message& msg);
    void send(Interface& iface, Address ifaceAddr, const Message& msg);

    DuplicateTuple* duplicateExists(Address originator, uint8_t seqNum);
    bool addressInList(const std::vector<Address>& iface_list, Address interface) const;
    bool isMPRSelector(NodeID addr) const;

    //For all tuples, decrement time and remove timed out entries
    void pruneSets();
    void updateMPRSet();

    NodeID m_myAddr;
    std::vector<Interface> m_interfaces;
    DataHandler m_dataHandler;

    //Maps one-hop neighbor to local interface
    std::map<NodeID, Interface*> m_neighborIfaceMap;

    //Maps one-hop neighbor to local address on interface
    std::map<NodeID, Address> m_neighborAddressMap;
    
    //OLSR Information Repositories (RFC3626 section 4)
    //Note: Anything holding a time value is being treated differently in this implementation
    //than according to RFC. I treat time as a counter that is decremented to zero before removal
    std::vector<IfaceAssociationTuple> m_ifaceAssociationSet;
    std::vector<LinkTuple> m_linkSet;
    std::vector<NeighborTuple> m_neighborSet;
    std::vector<TwoHopTuple> m_twoHopSet;
    std::map<NodeID, bool> m_mprSet;
    std::vector<MPRSelectorTuple> m_mprSelectorSet;
    std::vector<TopologyTuple> m_topologySet;

    std::vector<DuplicateTuple> m_duplicateSet;

    std::unique_ptr<ITimer> m_timer;

  };
}
