import sys
import boto3
import paramiko
import time
import os

ec2 = boto3.resource('ec2', region_name='us-east-1')
client = boto3.client('ec2', region_name = 'us-east-1')
instances = boto3.resource('ec2').instances.filter(
    Filters=[{'Name': 'instance-state-name', 'Values': ['running',
                                                        'terminated']}])
# res = client.run_instances(InstanceInitiatedShutdownBehavior='terminate',
#                                 ImageId='ami-0b69ea66ff7391e80',
#                                 MinCount=1,
#                                 MaxCount=1,
#                                 InstanceType='t2.micro',
#                                 KeyName="Cloud_Computing",
#                                  SecurityGroupIds=["sg-086edc690398dd098"],
#                            UserData='''#!/bin/bash
#                            sudo yum install python3 -y''')

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
# print(i.public_dns_name)

# instance_id = res['Instances'][0]['InstanceId']
# while True:
#     statuses = client.describe_instance_status(InstanceIds=[instance_id])
#     status = statuses['InstanceStatuses']

#     if len(status) > 0:
#         status = status[0]
#         if status['InstanceStatus']['Status'] == 'ok' \
#                 and status['SystemStatus']['Status'] == 'ok':
#             break
#         print ('.')
#         time.sleep(5)
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

command_to_run = "sudo yum install python3 -y && python3 nonce_finder.py -d " + str(1)

stdin, stdout, stderr = ssh.exec_command(command_to_run)

print(stdout.readlines()[-1])

sftp.close()
ssh.close()

os.system("python3 kill_instances.py")
