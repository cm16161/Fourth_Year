# Cloud Nonce Discovery (CND)

## Configuration

Edit the file generate_in_cloud.py between lines 17-21 with the appropriate
information to configure the system to spawn, connect to, and terminate VM's on
AWS.

**You must also have the keyfile (.pem file) in this directory.**

**NB. You must also set up your ~/.aws/credentials file**

## Usage

To use CND, execute:
`python3 generate_in_cloud.py -n [Number of VM's] -d [Difficulty level] -c [Confidence Level] -t [Timout] -l [Logfile]`
