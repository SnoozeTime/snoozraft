import zmq
import sys


def connect_to(context, host, port):
    s = context.socket(zmq.DEALER)
    s.connect("tcp://{}:{}".format(host, port))
    return s


def main():
    endpoint = sys.argv[1]
    port = sys.argv[2]
    print(endpoint)
    context = zmq.Context()
    s = connect_to(context, endpoint, port)

    s.send(b"hi")

    msgs = s.recv_multipart()
    if msgs[0] == b'LEADER':
        print("FOUND THE LEADER {}".format(endpoint))
    else:
        print("leader is {}. Will establish connection.".format(msgs[1]))
        endpoint = msgs[1].decode()
        s = connect_to(context,
                       endpoint.split(':')[1][2:],
                       endpoint.split(':')[-1])
        s.send(b"hi")
        msgs = s.recv_multipart()
        if msgs[0] == b'LEADER':
            print("FOUND THE LEADER {}".format(endpoint))
        else:
           print("THERE'S SOMETHING WRONG {}".format(msgs))


if __name__ == '__main__':
    main()
