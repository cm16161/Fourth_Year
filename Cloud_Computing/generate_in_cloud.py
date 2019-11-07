import sys
import boto3
import paramiko
import time
import os
import kill_instances
import argparse
import threading
import multithreading


STOP_THREADS = False

def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("-n", "--n_threads", help="Input the number of threads desired",
                        type=int, default="1")
    parser.add_argument("-d", "--difficulty", help="Input the difficulty value",
                        type=int, default="32")
    args = parser.parse_args()
    return args

# instances = boto3.resource('ec2').instances.filter(
#     Filters=[{'Name': 'instance-state-name', 'Values': ['running',
#                                                         'terminated']}])
def send_to_cloud(difficulty=1, start_val=0, step=1):
    while True:
        global STOP_THREADS
        ec2 = boto3.resource('ec2', region_name='us-east-1')
        response = ec2.create_instances(InstanceInitiatedShutdownBehavior='terminate',
                                        ImageId='ami-0b69ea66ff7391e80',
                                        MinCount=1,
                                        MaxCount=1,
                                        InstanceType='t2.micro',
                                        KeyName="Cloud_Computing",
                                        SecurityGroupIds=["sg-086edc690398dd098"]
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
    

            key = paramiko.RSAKey.from_private_key_file("Cloud_Computing.pem")
            ssh = paramiko.SSHClient()

            ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
            ssh.connect(hostname=ip, username='ec2-user', pkey=key)
            sftp = ssh.open_sftp()

            sftp.put("nonce_finder.py", "/home/ec2-user/nonce_finder.py")
            
            command_to_run = "sudo yum install python3 -y > /dev/null && python3 nonce_finder.py -d " + str(difficulty) + " --start " + str(start_val) + " --step " + str(step)
            while True:
                stdin, stdout, stderr = ssh.exec_command(command_to_run)
                if STOP_THREADS:
                    sftp.close()
                    ssh.close()
                    return
                print(stdout.readlines())
                STOP_THREADS= True
                sftp.close()
                ssh.close()
                return


def main():
    """Main Function"""
    args = get_args()
    global STOP_THREADS
    threads = list()
    for _t in range(args.n_threads):
        start_val = _t
        step = args.n_threads
        _x = multiprocessing.Process(target=send_to_cloud,
                              args=(args.difficulty, int(start_val), int(step)))
        # _x.setDaemon(True)
        threads.append(_x)
        _x.start()
    while not STOP_THREADS:
        pass
    
    for _t in threads:
        _t.join()

    kill_instances.kill()

if __name__ == '__main__':
    main()
