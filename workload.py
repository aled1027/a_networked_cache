

#std python libs imports
import socket
import json
import time
import string
import datetime
import time
import random
from threading import Thread, Event, Timer

#installed libs imports
import requests as req
import numpy as np

#global constants for connecting to remote server
HOST = '127.0.0.1' #@ifjorissen's ip address at home 
TCP_PORT = '8080'
UDP_PORT = '8081'
TCP_BASE = 'http://' + HOST + ':' + TCP_PORT

#global costants for testing the cache at various "rates" (really just sleep times) 
RATES = [5, 25, 50, 100, 250, 500, 750, 800, 850, 900, 950, 1000, 1050, 1100, 1200, 1500]
SUSTAINED_FOR = 30

#global variables for keeping track of requests and responses
sent_req_del = 0
recv_res_del = 0
sent_req_up = 0
recv_res_up = 0
sent_req_get = 0
recv_res_get = 0

sent_times_del = []
recv_times_del = []
sent_times_up = []
recv_times_up = []
sent_times_get = []
recv_times_get = []
elapsed = datetime.timedelta()


#globals for cache set-up
MAX_PAIRS = 100
WORKLOAD_CHOICE = ["GET", "DEL", "UP"] #types of work we can do: get, delete,upd ate
WORKLOAD_CHOICE_PROB = [.6, .3, .1] #rough probability of the type of work we're going to do
KEYS = []
VALUES = []

def my_assert(r):
    if r.status_code != 200:
        print("request failed")


def tcp_delete(key='ab', value='abc'):
    '''
    given a key and a value, send a PUT request. if no key or value is supplied, use default
    '''
    global elapsed, sent_req_del, recv_res_del, sent_times_del, recv_times_del
    sent_req_del += 1
    get_string = TCP_BASE + '/' + key
    sent_times_del.append(time.monotonic())
    resp = req.delete(get_string)
    recv_times_del.append(time.monotonic())
    recv_res_del += 1
    elapsed += resp.elapsed
    # print("tcp:: delete")

    my_assert(resp)
    return key, value

def tcp_post(key='ab', value='abc'):
    '''
    given a key and a value, send a PUT request. if no key or value is supplied, use default
    '''
    global elapsed, sent_req_up, recv_res_up, sent_times_up, recv_times_up
    sent_req_up += 1
    get_string = TCP_BASE + '/' + key + '/' + value
    sent_times_up.append(time.monotonic())
    resp = req.put(get_string)
    recv_times_up.append(time.monotonic())
    recv_res_up += 1
    elapsed += resp.elapsed
    # print("tcp:: post")

    my_assert(resp)
    return key, value


def udp_get(sock):
    '''
    given a socket and  key, get the response for that key to the socket address
    '''
    global recv_res_get, recv_times_get
    try:
        recv_data = sock.recvfrom(1024)
        recv_times_get.append(time.monotonic())
        recv_res_get += 1
        reply = recv_data[0]
        addr = recv_data[1]
        ret_dict = json.loads(reply.decode("utf-8"))
        return ret_dict
    except OSError: 
        print("we closed this socket")
        return None

def udp_send(sock, key):
    '''
    given a socket and  key, send a get request for that key to the socket address
    '''
    global sent_req_get, sent_times_get
    sock.sendto(key.encode('utf-8'), (HOST, int(UDP_PORT)))
    sent_times_get.append(time.monotonic())
    sent_req_get += 1
    # print("udp:: send")

def get_workload(sock, stop_task):
    '''
    allows us to continuously grab udp datagram responses
    '''
    while not stop_task.is_set():
        udp_get(sock)

def send_workload(sock, rate, stop_task):
    '''
    measures the mean response time for a workload composed of 100 percent get requests 
    sent at a rate of rate
    '''
    print("\n********************************************")
    print("sending messages at a rate of ~{} per second".format(rate))
    key = "hello" + str(rate) + "persec"
    value = "what do we have here"
    tcp_post(key, value)
    print("added key: <{}>  value:<{}> to cache".format(key, value))
    sleep_time = 1.0/rate
    while not stop_task.is_set():
        udp_send(sock, key)
        time.sleep(sleep_time)

def mixed_workload(sock, rate, stop_task):
    '''
    ~30percent DELETE, ~60percent GET, ~10percent update (post)
    '''
    print("\n********************************************")
    print("sending messages at a rate of ~{} per second".format(rate))
    sleep_time = 1.0/rate
    while not stop_task.is_set():
        work_type = np.random.choice(WORKLOAD_CHOICE, 1, p=WORKLOAD_CHOICE_PROB)
        key = random.choice(KEYS)
        val = random.choice(VALUES)

        if work_type == "GET":
            udp_send(sock, key)
        elif work_type == "DEL":
            tcp_delete(key)
        else:
            tcp_post(key, val)
        time.sleep(sleep_time)

def generate_key_val(num=MAX_PAIRS):
    '''
    generates up to MAX_PAIRS key value pairs, 
    '''
    global KEYS, VALUES
    num_pairs = 0
    while num_pairs < MAX_PAIRS:
        lengths = [int(x) for x in np.random.normal(40, 10, 2)]
        key = ''.join(random.choice(string.ascii_letters) for i in range(lengths[0]))
        value = ''.join(random.choice(string.ascii_letters) for i in range(lengths[1]))
        print("key: {}, value: {}".format(key, value))
        yield key, value
        KEYS.append(key)
        VALUES.append(value)
        num_pairs += 1

def tcp_post_setup(key='ab', value='abc'):
    '''
    given a key and a value, send a PUT request. if no key or value is supplied, use default
    '''
    get_string = TCP_BASE + '/' + key + '/' + value
    resp = req.put(get_string)
    my_assert(resp)
    return key, value

def setup_cache():
    '''
    adds key,value pairs to the cache in anticipation of mixed_workload
    '''
    print("\n********************************************")
    print("adding some key value pairs to the cache... slowly")
    for key, value in generate_key_val(MAX_PAIRS):
        tcp_post_setup(key, value)
        time.sleep(.1)

    print("...done pre-populating the cache")

def task_master():
    '''
    goal: determine the mean response times for a given workload at a variety of rates which is sustained for SUSTAINED_FOR seconds
    '''

    global sent_req_del, recv_res_del, sent_req_up, recv_res_up,\
        sent_req_get, recv_res_get, sent_times_del, recv_times_del,\
        sent_times_up, recv_times_up, sent_times_get, recv_times_get, elapsed

    with open("workload_data.dat", 'a') as f:
        f.write("#rate(req/sec)\t mean(sec)\t sent\t recieved\t lost\n")
    for i, rate in enumerate(RATES):
        try:
            port_no = 8082 + i 
            host_name = "127.0.0.1"
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            sock.bind((host_name, port_no))
            print("client datagram socket bound to: {}:{}".format(host_name, port_no)) 

            task_stop=Event()
            task = Thread(target=mixed_workload, args=(sock, rate, task_stop))
            task_getter = Thread(target=get_workload, args=(sock, task_stop))
            task.start()
            task_getter.start()
            
            time.sleep(SUSTAINED_FOR)
            task_stop.set()
            sock.close()
            task.join()
            task_getter.join()

            #now, do something moderately useful with this data
            sent_times = sent_times_get + sent_times_up + sent_times_del
            recv_times = recv_times_get + recv_times_up + recv_times_del
            sent_req = sent_req_get + sent_req_up + sent_req_del
            recv_res = recv_res_get + recv_res_up + recv_res_del

            get_times = [(e - s) for (s, e) in list(zip(sent_times_get, recv_times_get))]
            up_times = [(e - s) for (s, e) in list(zip(sent_times_up, recv_times_up))]
            del_times = [(e - s) for (s, e) in list(zip(sent_times_del, recv_times_del))]
            times = [(e - s) for (s, e) in list(zip(sent_times, recv_times))]

            lost = sent_req-recv_res
            lost_get = sent_req_get - recv_res_get    
            lost_up = sent_req_up - recv_res_up       
            lost_del = sent_req_del - recv_res_del

            try:
                mean_time = sum(times)/(sent_req - lost)
            except ZeroDivisionError:
                print("Ah! I guess we lost all the packets!")
                mean_time = 0.0


            try:
                mean_get = sum(get_times)/(sent_req_get - lost_get)
            except ZeroDivisionError:
                print("Ah! I guess we lost all the get packets!")
                mean_get = 0.0

            try:
                mean_up = sum(up_times)/(sent_req_up - lost_up)
            except ZeroDivisionError:
                print("Ah! I guess we lost all the update packets!")
                mean_up = 0.0


            try:
                mean_del = sum(del_times)/(sent_req_del - lost_del)
            except ZeroDivisionError:
                print("Ah! I guess we lost all the delete packets!")
                mean_del = 0.0
            
            with open("workload_data.dat", 'a') as f:
                f.write("{}\t{}\t{}\t{}\t{}\n".format(rate, mean_time, sent_req, recv_res, lost))
            print("\n\tTOT (sent): {}\t TOT (recv): {}\t mean TOT: {}\t lost: {}".format(sent_req, recv_res, mean_time, lost))
            print("\tGET (sent): {}\t GET (recv): {}\t mean GET: {}\t lost: {}".format(sent_req_get, recv_res_get, mean_get, lost_get))
            print("\tUPD (sent): {}\t UPD (recv): {}\t mean UPD: {}\t lost: {}".format(sent_req_up, recv_res_up, mean_up, lost_up))
            print("\tDEL (sent): {}\t DEL (recv): {}\t mean DEL: {}\t lost: {}".format(sent_req_del, recv_res_del, mean_del, lost_del))
            sent_req_del = 0
            recv_res_del = 0
            sent_req_up = 0
            recv_res_up = 0
            sent_req_get = 0
            recv_res_get = 0

            sent_times_del = []
            recv_times_del = []
            sent_times_up = []
            recv_times_up = []
            sent_times_get = []
            recv_times_get = []
            elapsed = datetime.timedelta()

            #sleep for some time so the server can catch up
            if lost > 10:
                sleep_time = lost/2
                print("lost {} responses, sleeping for {} seconds".format(lost, sleep_time))
            else:
                sleep_time = 5
                print("lost {} responses, sleeping for {} seconds".format(lost, sleep_time))
            time.sleep(sleep_time)

        except socket.error as e:
            print("Error Code: {} Message: {}".format(e[0], e[1]))
            sys.exit()
    print("done")

if __name__ == '__main__':
    setup_cache()
    task_master()












