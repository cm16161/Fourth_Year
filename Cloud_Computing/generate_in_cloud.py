import sys
import boto3
import paramiko
import time
import os
import kill_instances
import argparse
import threading
import multiprocessing
import time
import math
import logging


# Change the following lines to configure 

REGION = "us-east-1" # Region to deploy in
IMAGE_ID = "ami-00dc79254d0461090" # Image ID to launch
INSTANCE = "t2.micro" # Type of AWS instance to launch 
KEYNAME = "" # Name of the AWS key file (NB. DO NOT INCLUDE .pem)
SECURITY_GROUP_ID = "" # ID of the security group to assign a VM 

# End of Configurations

STOP_THREADS = False
TIME_FOR_ALL = 21524
FINISHED = 0


def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("-n", "--n_threads", help="Input the number of Virtual Machines to use",
                        type=int, default="1")
    parser.add_argument("-d", "--difficulty", help="Input the difficulty value",
                        type=int, default="32")
    parser.add_argument("-c", "--confidence", help="Input a confidence value such that the program will complete successfully with that confidence percentage", type=float, default="100")
    parser.add_argument("-t", "--timeout", type=int, help="Input a time-out value in seconds such that the whole program will terminate in that given time", default="86400")
    parser.add_argument("-l", "--log", action='store_true',help="Set this to output a logfile",default=False)
    args = parser.parse_args()
    return args

def send_to_cloud(difficulty=1, start_val=0, step=1, end=2**32+1):
    global STOP_THREADS
    global FINISHED
    ec2 = boto3.resource('ec2', region_name=REGION)
    response = ec2.create_instances(InstanceInitiatedShutdownBehavior='terminate',
                                    ImageId=IMAGE_ID,
                                    MinCount=1,
                                    MaxCount=1,
                                    InstanceType=INSTANCE,
                                    KeyName=KEYNAME,
                                    SecurityGroupIds=[SECURITY_GROUP_ID]
    )

    i = response[0]
    i.wait_until_running()
    i.load()
    time.sleep(40)

    instances = ec2.instances.filter(
        Filters=[{'Name': 'instance-state-name', 'Values': ['running']}])

    ip = i.public_dns_name
    print(ip)
    for instance in instances:
        print("Waiting for connection...")
        while instance.public_ip_address is None:
            pass

        key_file = KEYNAME+".pem"
        key = paramiko.RSAKey.from_private_key_file(key_file)
        ssh = paramiko.SSHClient()

        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh.connect(hostname=ip, username='ec2-user', pkey=key)
        sftp = ssh.open_sftp()
        print("Executing on: ec2-user@"+ip)
        sftp.put("nonce_finder.py", "/home/ec2-user/nonce_finder.py")

        command_to_run = "sudo yum install python3 -y > /dev/null && python3 nonce_finder.py -d " + str(difficulty) + " --start " + str(start_val) + " --step " + str(step) + " --end " + str(end)

        stdin, stdout, stderr = ssh.exec_command(command_to_run)
        if STOP_THREADS:
            sftp.close()
            ssh.close()
            return
        print(stdout.readlines())
        if stdout.readlines() == "[]":
            FINISHED+=1
            while True:
                pass

        STOP_THREADS= True
        sftp.close()
        ssh.close()
        return


def main():
    """Main Function"""
    args = get_args()
    start = time.time()
    if args.log:
        logging.basicConfig(filename="generate_in_cloud.log", level=logging.INFO)
        log_string = "Start time is: " + str(start)
        logging.info(log_string)
    global STOP_THREADS
    global FINISHED
    processes = list()
    n_processes = args.n_threads
    confidence = args.confidence/100
    if args.log:
        log_string = "Confidence is set to: " +str(args.confidence)
        logging.info(log_string)
    end = (confidence * 2**32) + 1
    if args.log:
        log_string = "End value to search up to is set to: " + str(end)
        logging.info(log_string)
    duration = args.timeout
    if args.log:
        log_string = "Maximum Program duration is set to: " + str(duration) + "s"
        logging.info(log_string)
    n_processes = math.ceil((confidence * TIME_FOR_ALL) / duration)
    if duration == 86400 and confidence == 1:
        n_processes = args.n_threads

    if args.log:
        log_string = "Spawning: " + str(n_processes) + " VMs"
        logging.info(log_string)
    for _t in range(args.n_threads):
        start_val = _t
        step = args.n_threads
        _x = multiprocessing.Process(target=send_to_cloud, daemon=True,
                                     args=(args.difficulty, int(start_val), int(step), int(end)))
        processes.append(_x)
        _x.start()
    wait = True
    while wait:
        if time.time() > start + duration:
            if args.log:
                log_string = "Timeout Met"
                logging.info(log_string)
            wait = False
        for _p in processes:
            if _p.is_alive():
                pass
            else:
                if args.log:
                    log_string = "Process Finished"
                    logging.info(log_string)
                wait = False
        if FINISHED == n_processes:
            if args.log:
                log_string = "All Processes Finished"
                logging.info(log_string)
            wait = False

    
    for _p in processes:
        if args.log:
            log_string = "Terminating Process"
            logging.info(log_string)    
        _p.terminate()

    if args.log:
        log_string = "Terminating VMs"
        logging.info(log_string)
    kill_instances.kill()
    

if __name__ == '__main__':
    main()
