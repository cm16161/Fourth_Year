import sys
import boto3
from botocore.exceptions import ClientError


def kill():
    ec2 = boto3.client('ec2')
    instances = boto3.resource('ec2').instances.filter(
    Filters=[{'Name': 'instance-state-name', 'Values': ['running']}])
    for instance in instances:
        print("Terminating: "+instance.id)
        ec2.terminate_instances(InstanceIds = [instance.id], DryRun = False)

if __name__ == '__main__':
    kill()
