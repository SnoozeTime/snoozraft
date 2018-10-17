# Distributed K->V store using RAFT

Some experimentations with zeromq, peer-to-peer systems and consensus protocol.

### Todo

Not exhaustive list of things I want to implement. Will be updated from time to time.

- [ ] Finish configuration with validation
- [ ] Create wrapper for zmq::poll (ZmqLoop)
- [ ] Add timer to ZmqLoop
- [ ] Send ping to all other peers
- [ ] Add peer reaper: when timeout, should check if some peers are not alive and KILL them.
- [ ] Begin RAFT
