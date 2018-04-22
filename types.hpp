#pragma once

#include <functional>

#include "Message.hpp"

namespace OLSR {
  using Address=uint32_t;

  using DataHandler = std::function<void(Message)>;
  using DataSender = std::function<void(std::vector<uint8_t>&&)>;

  struct Interface {
    Address addr;
    DataSender sender;

    uint16_t seqNum;
  };

  struct DuplicateTuple {
    //Originator address of the message
    //Originator address is the main address of the node that sent the message
    Address D_addr;

    //Message sequence number
    uint8_t D_seq_num;

    //Boolean, indicating whether the message has already been retransmitted
    bool D_retransmitted;

    //List of addresses of the interfaces on which the message has been received
    std::vector<Address> D_iface_list;

    //Time at which this tuple expires and _must_ be removed
    uint8_t D_time;
  };

  struct LinkInfo {
    uint8_t linkCode;
    
    std::vector<Address> neighborInterfaces;
  };
}
