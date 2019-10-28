import sys
import boto3
from botocore.exceptions import ClientError

ec2 = boto3.client('ec2')
instances = boto3.resource('ec2').instances.filter(
    Filters=[{'Name': 'instance-state-name', 'Values': ['running', 'terminated']}])

for instance in instances:
    ec2.terminate_instances(InstanceIds = [instance.id], DryRun = False)

