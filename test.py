import socket
import sys
import json
import time
import string
import datetime
import time
import random
from threading import Thread, Event, Timer
import requests
#from requests_futures.sessions import FuturesSession

HOST = '127.0.0.1'
TCP_PORT = '8080'
UDP_PORT = '8081'
TCP_BASE = 'http://' + HOST + ':' + TCP_PORT

def get_time():
    return time.time()

def shutdown_cache(host, tcp_port):
    print("shutting down server")
    tcp_base = 'http://' + host + ':' + tcp_port
    resp = requests.post(tcp_base + '/shutdown')
    time.sleep(1) # give time for the server to setup

def setup_cache(num_pairs):
    print("setting up cache")
    key_size = 8
    val_size = 16
    keys = []
    values = []
    for i in range(num_pairs):
        key = ''.join(random.choice(string.ascii_letters) for i in range(key_size))
        value = ''.join(random.choice(string.ascii_letters) for i in range(val_size))
        print("setting", key, value)
        tcp_put(key, value)
        keys.append(key)
        values.append(value)
        time.sleep(.01)
    return keys, values

def tcp_put(key, value):
    '''
    given a key and a value, send a PUT request. if no key or value is supplied, use default
    '''
    get_string = TCP_BASE + '/' + key + '/' + value
    resp = requests.put(get_string)
    return key, value


def tcp_get(host, tcp_port, key):
    tcp_base = 'http://' + host + ':' + tcp_port
    get_string = tcp_base + '/' + key
    requests.get(get_string)

def udp_get(key, sock):
    '''
    given a socket and  key, send a get request for that key to the socket address
    '''
    try:
        print("getting", key)
        sock.sendto(key.encode("utf-8"), (HOST, int(UDP_PORT)))
    except:
        print(sys.exc_info()[0])

def send_requests(rate, duration, keys, values, sock):
    tcp_port = '8080'
    udp_port = 8081
    host = "127.0.0.1"


    rate_timedelta = 1. / rate
    print("rate_timedelta = {}".format(rate_timedelta))

    end_time = get_time() + duration
    i = 0
    while get_time() < end_time:
        i+=1
        #tcp_get(host, tcp_port, key)
        key = random.choice(keys)
        udp_get(key, sock)
        recv_val = udp_recv(sock)
        print(key, recv_val)

        #delay = get_time() - goal_end_time
        #print(delay)
        #if delay > 0.0001:
        #    time.sleep(delay)

    shutdown_cache(host, tcp_port)

    print("sent " , i)

def udp_recv(sock):
    '''
    given a socket and  key, get the response for that key to the socket address
    '''
    global recv_res_get, recv_times_get
    try:
        recv_data = sock.recvfrom(1024)
        print(recv_data)
        return recv_data[0]
    except:
        #print("udp_recv::Unexpected error:", sys.exc_info()[0])
        return None

def recv_responses(keys, values, duration, sock):
    end_time = get_time() + duration
    while get_time() < end_time:
        udp_recv(sock)

def go():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind((HOST, int(UDP_PORT)))

    duration = 5

    keys, values = setup_cache(1)
    #recv_thread = Thread(target=recv_responses, args=(keys, values, duration, sock))
    #recv_thread.start()

    print(keys)
    print(values)
    udp_get(keys[0], sock)
    recv_val = udp_recv(sock)
    print(recv_val)

    #send_requests(1000, duration, keys, values, sock)

    sock.close()

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind((HOST, int(UDP_PORT)))

key = "alex"
value = "ledger"
tcp_put(key, value)

sock.sendto(key.encode("utf-8"), (HOST, int(UDP_PORT)))
recv_data = sock.recvfrom(1024)

print(recv_data)
