#include "Router.hpp"

#include "util.hpp"

namespace OLSR {

Router::Router(const std::vector<Interface>& interfaces, const DataHandler& handler)
  : m_interfaces(interfaces)
  , m_dataHandler{handler} {
}

void Router::process(Packet&& p) {
  for(auto& msg : p) {
    //Find duplicate tuple entry for message
    auto* duplicate = duplicateExists(p.originator(), msg.seqNum());

    //Processing condition
    if( (msg.ttl() > 0) && !duplicate ) {
      process(msg);


    }

    //Forwarding condition
    if(!duplicate || !addressInList(duplicate->D_iface_list), p.recvInterface()) {
      forward(msg, duplicate);
    }

    //Place in duplicate set
    //m_duplicateSet.emplace_back(p.originator(), msg.seqNum(), false, {p.recvInterface()}, 0);
  }
}

void Router::process(Message& m) {
  switch(msg.messageType()) {
    case Type::Hello:
      //TODO
    break;

    case Type::TC:
      //TODO
    break;

    case Type::MID:
      //TODO
    break;

    case Type::HNA:
      //TODO
    break;

    case Type::User:
      m_dataHandler(msg);
    break;
  }

}

void Router::forward(Mesage& m, DuplicateTuple* duplicate) {
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
    duplicate->D_time = currentTime + DUP_HOLD_TIME;
    duplicate->D_iface_list.push_back(m.packet()->recvInterface());
    duplicate->D_retransmitted = toRetransmit;
  }
  //Place new entry for message in duplicate tuple set
  else {
    m_duplicateSet.emplace_back(m.originator(), m.seqNum(), toRetransmit,
      {m.packet()->recvInterface()}, curTime + DUP_HOLD_TIME);
  }

  if(toRetransmit) {
    retransmit(m);
  }
}

void Router::retransmit(Message& m) {
  //Decrement TTL
  m.ttl(m.ttl()-1);
  
  //Increment hop count
  m.hopCount(m.hopCount()+1);

  //Broadcast message copy on all interfaces
  broadcast(m);
}

void Router::send(Interface& interface, const Message& msg) {
  //Create vector representing packet containing message
  std::vector<uint8_t> packet(msg.messageSize() + 4);

  //Packet length
  util::packU16(packet.data(), packet.size());
  
  //Packet sequence number
  util::packU16(packet.data()+2, interface.seqNum);
  interface.seqNum++;

  //Insert message
  packet.insert(packet.begin()+4, msg.begin(), msg.end());

  //Send packet
  interface.sender(std::move(packet));
}
