#pragma once

#include "types.hpp"

namespace OLSR {
  class Router {
  public:
    Router(const std::vector<Interface>& interfaces, const DataHandler& handler);

    void process(Packet&& p);


  private:
    void process(Message& m);
    void forward(Message& m, DuplicateTuple*);
    void retransmit(Message& m);

    void send(Interface& interface, const Message& msg);

    DuplicateTuple* duplicateExists(Address originator, uint8_t seqNum);
    bool addressInList(const std::vector<Address>& iface_list, Address interface);

    
    void pruneDuplicateSet();


    std::vector<Interface> m_interfaces;
    DataHandler m_dataHandler;

    std::vector<DuplicateTuple> m_duplicateSet;

  };
}
