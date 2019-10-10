'''This script will try to find a nonce with given parameters'''
import hashlib

def find_nonce(difficulty_level):
    """This function will continually generate nonces and see if they
    are golden"""
    for i in range(0, 2**32):
        sha_f = hashlib.sha256()
        num = bytes(i)
        sha_f.update(num)
        sha_f.update(sha_f.digest())
        result = sha_f.digest()
        bit_shift = 257-difficulty_level
        if (int.from_bytes(result, byteorder='little', signed=False) >> bit_shift) == 0:
            print("Golden Nonce: "+str(i))


def main():
    """Main Function"""
    find_nonce(20)

if __name__ == '__main__':
    main()
