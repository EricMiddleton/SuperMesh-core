#pragma once

#include <vector>
#include <functional>
#include <cstdint>

//#include "Message.hpp"

namespace OLSR {
  class Message;

  using Address=uint32_t;
  using NodeID = uint32_t;

  using DataHandler = std::function<void(const Message&)>;
  using DataSender = std::function<void(Address, std::vector<uint8_t>&&)>;
  using TimerFn = std::function<void(void)>;

  using TimeValue = uint32_t;

  struct Interface {
    Address addr;
    DataSender sender;

    uint16_t seqNum;
  };

  //Struct representing a one-hop neighbor link
  //Used by external link manager to inform router about existing links
  struct Link {
    NodeID nodeAddr;
    Address ifaceAddr;
  };

  struct DuplicateTuple {
    //Originator address of the message
    //Originator address is the main address of the node that sent the message
    NodeID D_addr;

    //Message sequence number
    uint8_t D_seq_num;

    //Boolean, indicating whether the message has already been retransmitted
    bool D_retransmitted;

    //List of addresses of the interfaces on which the message has been received
    std::vector<Address> D_iface_list;

    //Time at which this tuple expires and _must_ be removed
    uint8_t D_time;
  };

  struct IfaceAssociationTuple {
    Address iface_addr;           //Interface address of node
    Address NodeID;            //Main address of node
    uint8_t time;                 //Time at which this record expires and must be removed
  };

  struct LinkTuple {
    Address local_iface_addr;     //Interface address of local node
    Address neighbor_iface_addr;  //Interface address of neighbor node
    uint8_t SYM_time;             //Time until link is considered symmetric
    uint8_t ASYM_time;            //Time until which the neighbor interface is considered heard
    uint8_t time;                 //Time this record expires and must be removed
  };

  struct NeighborTuple {
    NodeID main_addr;             //Main address of node
    uint8_t status;               //Specifies if node is NOT_SYM or SYM
    uint8_t willingness;          //Integer [0, 7], specifies the node's willingness to route
  };

  struct TwoHopTuple {
    NodeID main_addr;             //Main address of neighbor
    Address twohop_addr;          //Main address of 2-hop neighbor with
                                  //direct link to neighbor_main_addr
    uint8_t time;                 //Time at which this record expires and must be removed
  };

  struct MPRSelectorTuple {
    NodeID main_addr;             //Main address of node that selected this as MPR
    uint8_t time;                 //Time this record expires and must be removed
  };

  struct TopologyTuple {
    NodeID dest_addr;             //Main address of node that is one hop from last_addr
    NodeID last_addr;             //Main address of node that is one hop from dest_addr
                                  //Typically, this is an MPR of dest_addr
    uint16_t seq;                 //Sequence number
    uint8_t time;                 //Time this record expires and must be removed
  };

  struct LinkInfo {
    uint8_t linkCode;
    
    std::vector<Address> neighborInterfaces;
  };

  const Address BroadcastAddress = 0xFFFFFFFF;
}
