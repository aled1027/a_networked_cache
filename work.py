
import socket
import sys
import json
import time
import string
import datetime
import time
import random
from threading import Thread, Event, Timer
import requests as req
from requests_futures.sessions import FuturesSession
from queue import Queue
import numpy as np
import subprocess

#global constants for connecting to remote server
HOST = '127.0.0.1'
TCP_PORT = '8080'
UDP_PORT = '8081'
TCP_BASE = 'http://' + HOST + ':' + TCP_PORT
data_filename = "workload_data.tsv"

#global costants for testing the cache at various "rates" 
RATES = [i for i in range(750, 2000, 100)]
SUSTAINED_FOR = 30

#global variables for keeping track of get requests and responses
sent_req_get = 0
recv_res_get = 0

sent_times_get = []
recv_times_get = []

tcp_responses = Queue()

#globals for cache set-up
TIMEOUT = .025 #25 ms; any request taking longer than this will be counted as "lost"
MAX_PAIRS_INIT = 250 #initializes the cache with #MAX_PAIRS_INIT items
WORKLOAD_CHOICE = ["GET", "DEL", "UP"] #types of work we can do: get, delete,upd ate
WORKLOAD_CHOICE_PROB = [.7, 0, .3] #rough probability of the type of work we're going to do
KEYS = []
VALUES = []
KEY_LEN = 8
VAL_LEN = 16

def get_time():
    return time.time()

def tcp_delete(sess, key):
    '''
    given a key, and a session send a PUT request. if no key is supplied, use default
    '''
    global tcp_responses
    try:
        get_string = TCP_BASE + '/' + key
        resp = sess.delete(get_string, timeout=TIMEOUT*200)
        tcp_responses.put_nowait(resp)

        return key
    except Exception as e:
        print("tcp_delete::uhoh: {}".format(e))

def tcp_update(sess, key='ab', value='abc'):
    '''
    given a key and a value, send a PUT request. if no key or value is supplied, use default
    '''
    global tcp_responses
    try:
        get_string = TCP_BASE + '/' + key + '/' + value
        resp = sess.put(get_string, timeout=TIMEOUT*200)
        tcp_responses.put_nowait(resp)

        # print("tcp:: post")
        return key, value
    except Exception as e:
        print("tcp_update::uhoh: {}".format(e))

def udp_get(sock):
    '''
    given a socket and  key, get the response for that key to the socket address
    '''
    global recv_res_get, recv_times_get
    try:
        recv_data = sock.recvfrom(1024)
        recv_times_get.append(get_time())
        recv_res_get += 1

        # print("got: {}".format(recv_data))
        reply = recv_data[0]
        addr = recv_data[1]
        ret_dict = json.loads(reply.decode("utf-8"))
        return ret_dict
    except OSError:
        print("udp_get:: we closed this socket")
        return None
    except:
        print("udp_get::Unexpected error:", sys.exc_info()[0])
        return None

def udp_send(sock, key):
    '''
    given a socket and  key, send a get request for that key to the socket address
    '''
    global sent_req_get, sent_times_get
    try:
        sock.sendto(key.encode('utf-8'), (HOST, int(UDP_PORT)))
        sent_times_get.append(get_time())
        sent_req_get += 1
        # print("udp::send")
    except OSError:
        print("udp::send we closed this socket")
        return None
    except:
        return None

def recv_workload(sock, stop_task):
    '''
    allows us to continuously grab udp datagram responses
    '''
    while not stop_task.is_set():
        udp_get(sock)

def mixed_workload(sock, sess, rate, stop_task):
    '''
    A mixed workload as defined by global WORKLOAD_CHOICE_PROP list
    '''
    print("\n********************************************")
    print("sending messages at a rate of ~{} per second".format(rate))

    rate_timedelta = 1.0 / rate
    print("rate_timedelta = {}".format(rate_timedelta))
    start_time = get_time()
    next_time = start_time + rate_timedelta
    while not stop_task.is_set():
        work_type = np.random.choice(WORKLOAD_CHOICE, 1, p=WORKLOAD_CHOICE_PROB)
        key = random.choice(KEYS)
        new_key, new_val = single_key_val_pair()
        cur_time = get_time()
        if cur_time < next_time:
            delay = next_time - cur_time
            time.sleep(delay)
        if work_type == "GET":
            udp_send(sock, key)
        elif work_type == "DEL":
            tcp_delete(sess, key)
        else:
            tcp_update(sess, new_key, new_val)

        next_time += rate_timedelta

def single_key_val_pair():
    global KEYS, VALUES
    key = ''.join(random.choice(string.ascii_letters) for i in range(KEY_LEN))
    value = ''.join(random.choice(string.ascii_letters) for i in range(VAL_LEN))
    KEYS.append(key)
    VALUES.append(value)
    return key, value


def generate_key_val(num=MAX_PAIRS_INIT):
    '''
    generates up to MAX_PAIRS_INIT key value pairs,
    '''
    global KEYS, VALUES
    num_pairs = 0
    while num_pairs < MAX_PAIRS_INIT:
        # lengths = [int(x) for x in np.random.normal(40, 10, 2)]
        key = ''.join(random.choice(string.ascii_letters) for i in range(KEY_LEN))
        value = ''.join(random.choice(string.ascii_letters) for i in range(VAL_LEN))
        yield key, value
        KEYS.append(key)
        VALUES.append(value)
        num_pairs += 1

def tcp_put(key, value):
    '''
    given a key and a value, send a PUT request. if no key or value is supplied, use default
    '''
    get_string = TCP_BASE + '/' + key + '/' + value
    resp = req.put(get_string)
    return key, value

def setup_cache():
    '''
    adds key,value pairs to the cache in anticipation of mixed_workload
    '''
    print("adding some key value pairs to the cache... slowly")
    for key, value in generate_key_val(MAX_PAIRS_INIT):
        tcp_put(key, value)
        time.sleep(.01)
        # print("adding", key, value)
    print("...done pre-populating the cache")

def shutdown_cache():
    print("done, shutting down cache....")
    print("********************************************\n")
    resp = req.post(TCP_BASE + '/shutdown')
    time.sleep(4) # give time for the server to setup

def analyze_data(rate, filename):
    """
    analyzes data and writes to file
    """
    global sent_req_get, recv_res_get, sent_times_get, recv_times_get, tcp_responses
    time.sleep(5)
    # Prepare data from requests sent via tcp
    puts = 0
    deletes = 0
    lost_put = 0
    lost_delete = 0
    put_tot_elapsed = 0
    del_tot_elapsed = 0

    now_time = get_time()
    next_time = now_time + 1
    print("ok, analyzing the data for reals...")

    while not tcp_responses.empty():
        if now_time > next_time:
            left = tcp_responses.qsize()
            print("...responses left to process: {}".format(left))
            next_time = now_time + 2
        now_time = get_time()
        try:
            fut_res = tcp_responses.get()
            resp = fut_res.result()
            elapsed = resp.elapsed.total_seconds()
            method = resp.request.method
            if method == 'DELETE':
                deletes += 1
                del_tot_elapsed += elapsed
                if elapsed > TIMEOUT:
                    lost_delete += 1
            elif method == 'PUT':
                puts += 1
                put_tot_elapsed += elapsed
                if elapsed > TIMEOUT:
                    lost_put += 1
            else:
                print("what? not a request method we care about")
        except ConnectionError as e:
            print("oops: {}".format(e))
        except req.exceptions.Timeout as e:
            print("oops Timeout error: {}".format(e))
            #there are no deletes, so count a timeout as a lost put request
            if WORKLOAD_CHOICE_PROB[1] == 0:
                lost_put += 1
                elapsed += TIMEOUT


    if puts:
        try:
            mean_put = put_tot_elapsed/puts
        except ZeroDivisionError:
            print("Ah! I guess we lost all the put packets!")
            mean_put = 0.0
    else:
        mean_put = 0.0

    if deletes:
        try:
            mean_delete = del_tot_elapsed/deletes
        except ZeroDivisionError:
            print("Ah! I guess we lost all the delete packets!")
            mean_delete = 0.0
    else:
        mean_delete = 0.0

    print("done with tcp analysis")
    # end tcp work

    #prepare data from requests sent via udp

    get_times = [(e - s) for (s, e) in list(zip(sent_times_get, recv_times_get))]
    lost_get = sent_req_get - recv_res_get

    try:
        mean_get = sum(get_times)/(sent_req_get - lost_get)
    except ZeroDivisionError:
        print("Ah! I guess we lost all the get packets!")
        mean_get = 0.0

    print("done with udp analysis")
    #end udp work

    #total requests sent, requests lost, mean time (rough weighted avg)
    sent_req = sent_req_get + puts + deletes 
    lost = lost_get + lost_put + lost_delete
    mean_time = (mean_get*(sent_req_get/sent_req) + mean_delete*(deletes/sent_req) + mean_put*(puts/sent_req))

    with open(filename, 'a') as f:
        f.write("{:6d},\t{:.6f},\t{:6d},\t{:6d},\t{:.6f},\t{:6d},\t{:6d},\t{:.6f},\t{:6d},\t{:6d},\t{:.6f},\t{:6d},\t{:6d},\t{:6d}\n".format(rate, mean_time, sent_req, sent_req-lost,\
                mean_get, sent_req_get, recv_res_get, mean_put, puts,\
                puts-lost_put, mean_delete, deletes, deletes-lost_delete, lost))
    print("\n\tTOT (sent): {:6d}\t TOT (recv): {:6d}\t mean TOT(s): {:.6f}\t lost: {}".format(sent_req, sent_req-lost, mean_time, lost))
    print("\tGET (sent): {:6d}\t GET (recv): {:6d}\t mean GET(s): {:.6f}\t lost: {}".format(sent_req_get, recv_res_get, mean_get, lost_get))
    print("\tPUT (sent): {:6d}\t PUT (recv): {:6d}\t mean PUT(s): {:.6f}\t lost: {}".format(puts, puts-lost_put, mean_put, lost_put))
    print("\tDEL (sent): {:6d}\t DEL (recv): {:6d}\t mean DEL(s): {:.6f}\t lost: {}".format(deletes, deletes-lost_delete, mean_delete, lost_delete))
    print("lost {} responses, sleeping for a few seconds".format(lost))

def task_master():
    '''
    goal: determine the mean response times for a given workload at a variety of rates which is sustained for SUSTAINED_FOR seconds
    '''

    global sent_req_get, recv_res_get, sent_times_get, recv_times_get, KEYS, VALUES, tcp_responses

    #create file name, open file, write header 
    nowish = datetime.datetime.now().strftime("%Y-%m-%d_%H:%M")
    filename = "data/workload_data{}.csv".format(nowish)

    with open(filename, 'a') as f:
        f.write("\n#rate(req/sec),\t mean(sec),\t total_sent,\t total_recieved,\t get_mean(sec),\t \
                get_sent,\t get_received,\t put_mean(sec),\t put_sent,\t put_received,\t \
                delete_mean(sec),\t delete_sent,\t delete_received,\t lost\n")

    for i, rate in enumerate(RATES):
        print("********************************************")
        print("starting up server... ")
        make_process = subprocess.Popen("make run_server", shell=True, close_fds=True)
        time.sleep(3)

        #reset global variables
        tcp_responses = Queue()
        sent_req_get = 0
        recv_res_get = 0

        sent_times_get = []
        recv_times_get = []
        KEYS = []
        VALUES = []

        try:
            port_no = 8082 + i
            host_name = "127.0.0.1"
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            sock.bind((host_name, port_no))
            print("\nclient datagram socket bound to: {}:{}".format(host_name, port_no))

            sess = FuturesSession()
            setup_cache()

            task_stop=Event()
            task = Thread(target=mixed_workload, args=(sock, sess, rate, task_stop))
            task_getter = Thread(target=recv_workload, args=(sock, task_stop))
            task.start()
            task_getter.start()

            time.sleep(SUSTAINED_FOR)
            task_stop.set()
            sock.close()

            task.join()
            task_getter.join() # doesn't work on Alex's computer

            analyze_data(rate, filename)
            shutdown_cache()
        except OSError as e:
            print("could not complete at rate: {} exception: {}".format(rate, e))
        except:
            print("Unexpected error:", sys.exc_info()[0])
        finally:
            make_process.kill()

    print("done")
    sys.exit()





if __name__ == '__main__':
    task_master()