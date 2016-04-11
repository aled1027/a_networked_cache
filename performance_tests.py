import requests as req
import numpy as np
import socket

HOST = 'localhost'
TCP_PORT = '8080'
UDP_PORT = '8081'
TCP_BASE = 'http://' + HOST + ':' + TCP_PORT

def my_assert(r):
    if r.status_code != 200:
        print("request failed")

def tcp_get(key):
    print('getting')
    get_string = TCP_BASE + '/' + key
    resp = req.get(get_string)
    print(resp)

def udp_get(key):
    # acquired from: https://gist.github.com/silv3rm00n/5678933
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    except socket.error:
        print ('Failed to create socket')
        sys.exit()

    host = 'localhost';
    port = 8081;
    while(1) :
        try :
            msg = input('Enter message to send : ')
            msg = 'ab'

            #Set the whole string
            s.sendto(msg.encode('utf-8'), (host, port))

            # receive data from client (data, addr)
            d = s.recvfrom(1024)
            reply = d[0]
            addr = d[1]

            print ('Server reply : ' + reply.decode("utf-8"))

        # Some problem sending data ??
        except socket.error as e:
            print ('Error Code : ' + str(e[0]) + ' Message ' + e[1])
            sys.exit()

        # Ctrl + C
        except KeyboardInterrupt:
            break

    print('Program Complete')

def tcp_post():
    key_length = 2
    key = 'ab'
    value = 'abc'
    get_string = TCP_BASE + '/' + key + '/' + value
    resp = req.put(get_string)
    print(resp)

def udp_test():
    np.random.seed(0)
    TCP_BASE = 'http://localhost:9090'
    req_type = 'GET' if np.random.rand() < .8 else 'PUT'

def tcp_test():
    np.random.seed(0)
    req_type = 'GET' if np.random.rand() < .8 else 'PUT'

    if req_type == 'GET':
        key_length = 2
        key = 'a' * key_length
        tcp_get(key)

    elif req_type == 'PUT':
        print('putting')

if __name__ == '__main__':
    # tcp_test()
    tcp_post()
    udp_get('ab')
