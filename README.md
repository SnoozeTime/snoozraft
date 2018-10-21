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

- [ ] Better logging
- [X] Frame protocol (ZmqMessage)
- [ ] Finish configuration with validation
- [ ] Add dockerfile
- [ ] Create EnvConfig to read from environment variables
- [X] Create wrapper for zmq::poll (ZmqLoop)
- [X] Add timer to ZmqLoop
- [ ] Complete ZmqLoop (remove objects)
- [X] Send ping to all other peers
- [X] Add peer reaper: when timeout, should check if some peers are not alive and KILL them.
- [ ] Begin RAFT
