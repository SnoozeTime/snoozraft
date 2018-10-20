# Distributed K->V store using RAFT

Some experimentations with zeromq, peer-to-peer systems and consensus protocol.

### Todo

Not exhaustive list of things I want to implement. Will be updated from time to time.

- [ ] Better logging
- [ ] Frame protocol
- [ ] Finish configuration with validation
- [X] Create wrapper for zmq::poll (ZmqLoop)
- [X] Add timer to ZmqLoop
- [ ] Complete ZmqLoop (remove objects)
- [X] Send ping to all other peers
- [X] Add peer reaper: when timeout, should check if some peers are not alive and KILL them.
- [ ] Begin RAFT
