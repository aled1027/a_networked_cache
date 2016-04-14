

#std python libs imports
import socket
import json
import time
# import signal
import datetime
import time
from threading import Thread, Event

#installed libs imports
import requests as req
import numpy as np

#global constants for connecting to remote server
HOST = '127.0.0.1'
TCP_PORT = '8080'
UDP_PORT = '8081'
TCP_BASE = 'http://' + HOST + ':' + TCP_PORT

#global costants for testing the cache at various rates 
RATES = [5, 200, 300, 400, 500, 1000, 10000]
SUSTAINED_FOR = 30

#global variables for keeping track of requests and responses
sent_req = 0
recv_res = 0
sent_times = 0
recv_times = 0
elapsed = datetime.timedelta()


def my_assert(r):
    if r.status_code != 200:
        print("request failed")

def tcp_post(key='ab', value='abc'):
    '''
    given a key and a value, send a PUT request. if no key or value is supplied, use default
    '''
    global elapsed, sent_req, recv_res
    sent_req += 1
    get_string = TCP_BASE + '/' + key + '/' + value
    resp = req.put(get_string)
    recv_res += 1
    elapsed += resp.elapsed

    my_assert(resp)
    return key, value


def udp_get(s, key):
    '''
    given a socket and  key, get the response for that key to the socket address
    '''
    global recv_res, recv_times
    recv_data = s.recvfrom(1024)
    recv_times += time.monotonic()
    recv_res += 1

    reply = recv_data[0]
    addr = recv_data[1]
    ret_dict = json.loads(reply.decode("utf-8"))
    return ret_dict

def udp_send(s, key):
    '''
    given a socket and  key, send a get request for that key to the socket address
    '''
    global sent_req, sent_times
    s.sendto(key.encode('utf-8'), (HOST, int(UDP_PORT)))
    sent_times += time.monotonic()
    sent_req += 1

def get_workload(rate, stop_task):
    '''
    measures the mean response time for a workload composed of 100 percent get requests 
    sent at a rate of rate
    '''

    global sent_times, recv_times, sent_req, recv_res
    try: 
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        print("sending messages at a rate of {} per second".format(rate))
        key = "hello"
        value = "what do we have here"
        tcp_post(key, value)
        print("added key: <{}>  value:<{}> to cache".format(key, value))
        count = 0

        while not stop_task.is_set():
            sleep_time = 1.0/rate
            udp_send(s, key)
            got = udp_get(s, key)
            time.sleep(sleep_time)

    except socket.error as e:
        print ('Error Code : ' + str(e[0]) + ' Message ' + e[1])
        sys.exit()

def task_master():
    global sent_times, recv_times, sent_req, recv_res
    '''
    goal: determine the mean response times for a given workload at a variety of rates which is sustained for SUSTAINED_FOR seconds
    '''
    rate = RATES[0]
    task_stop=Event()
    task = Thread(target=get_workload, args=(rate, task_stop))
    task.start()
    
    time.sleep(SUSTAINED_FOR)
    task_stop.set()
    mean_time = (recv_times - sent_times)/(sent_req)
    print("\nget mean time for {} requests: {}. \n\tsent requests: {}, received responses: {}".format(sent_req, mean_time, sent_req, recv_res))
    recv_times = 0
    sent_times = 0
    sent_req = 0
    recv_res = 0

if __name__ == '__main__':
    task_master()












