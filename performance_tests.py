import requests as req
import numpy as np
import socket
import json

HOST = 'localhost'
TCP_PORT = '8080'
UDP_PORT = '8081'
TCP_BASE = 'http://' + HOST + ':' + TCP_PORT

def my_assert(r):
    if r.status_code != 200:
        print("request failed")

def tcp_get(key):
    get_string = TCP_BASE + '/' + key
    resp = req.get(get_string)
    return resp

def udp_get(key):
    # adapted from: https://gist.github.com/silv3rm00n/5678933
    try :
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.sendto(key.encode('utf-8'), (HOST, int(UDP_PORT)))
        recv_data = s.recvfrom(1024)
        reply = recv_data[0]
        addr = recv_data[1]
        ret_dict = json.loads(reply.decode("utf-8"))
        return ret_dict

    except socket.error as e:
        print ('Error Code : ' + str(e[0]) + ' Message ' + e[1])
        sys.exit()

def tcp_post():
    key_length = 2
    key = 'ab'
    value = 'abc'
    get_string = TCP_BASE + '/' + key + '/' + value
    resp = req.put(get_string)
    my_assert(resp)
    return key, value

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
    #tcp_post()
    tcp_post()
    a = udp_get('ab')
    print(a)

