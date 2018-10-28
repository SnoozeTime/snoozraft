# Distributed K->V store using RAFT

Some experimentations with zeromq, peer-to-peer systems and consensus protocol.

## How does it work

### Peer model
Follow pattern described in ZMQ book. Each peer has a ROUTER socket to receive
messages and a DEALER socket to send message to each other peer.

## Network membership

There are bootstrap/anchor nodes that are known beforehand. Other nodes will
connect to the network by sending a join request to them.

## Todo

Not exhaustive list of things I want to implement. Will be updated from time to time.

- [X] Better logging
- [X] Frame protocol (ZmqMessage)
- [ ] Finish configuration with validation
- [X] Add dockerfile
- [X] Create EnvConfig to read from environment variables
- [X] Create wrapper for zmq::poll (ZmqLoop)
- [X] Add timer to ZmqLoop
- [X] Complete ZmqLoop (remove objects)
- [X] Send ping to all other peers
- [X] Add peer reaper: when timeout, should check if some peers are not alive and KILL them.
- [X] Begin RAFT
- [X] Use msgpack for message content (first frame is address, second is message ID, third is binary content)
- [X] Simple election + timeout in raft
- [X] Generation script -> Add from in message handler
- [X] Generation script -> create enum from messages
- [X] Generation script -> Create msg_handler class
- [ ] Raft: Create Raft state (persistent)
- [ ] Raft: Create Raft state (in memory)
- [ ] Raft: Create small cluster client to get leader ID
    - on server side, need another thread to wait for client request. Threads
      will communicate using zmq sockets so need another loop
    - on client side, just connect with zmq (python for fun?)
    
- [ ] VM + State machine
