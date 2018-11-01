import zmq
import sys

CLIENT_MAP = {
    'tcp://localhost:5555': '44444',
    'tcp://localhost:5556': '4445',
    'tcp://localhost:5557': '4446',
}

def connect_to(context, host, port):
    s = context.socket(zmq.DEALER)
    s.connect("tcp://{}:{}".format(host, port))
    return s


def main():
    endpoint = sys.argv[1] or 'tcp://localhost:5555'
    print(endpoint)
    context = zmq.Context()
    s = connect_to(context, 'localhost', CLIENT_MAP[endpoint])

    s.send(b"hi")

    msgs = s.recv_multipart()
    if msgs[0] == b'LEADER':
        print("FOUND THE LEADER {}".format(endpoint))
    else:
        print("leader is {}. Will establish connection.".format(msgs[1]))
        endpoint = msgs[1].decode()
        s = connect_to(context, 'localhost', CLIENT_MAP[endpoint])
        s.send(b"hi")
        msgs = s.recv_multipart()
        if msgs[0] == b'LEADER':
            print("FOUND THE LEADER {}".format(endpoint))
        else:
            print("THERE'S SOMETHING WRONG {}".format(msgs))


if __name__ == '__main__':
    main()