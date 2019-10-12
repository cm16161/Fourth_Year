'''This script will try to find a nonce with given parameters'''
import hashlib
import binascii

def find_nonce(difficulty_level, block="COMSM0010cloud"):
    """This function will continually generate nonces and see if they
    are golden"""
    for i in range(0, 2**32):
        sha_f = hashlib.sha256()
        block += str(i)
        block_bytes = bytes(block, 'ascii')
        sha_f.update(block_bytes)
        sha_f.update(sha_f.digest())
        result = sha_f.digest()
        bit_shift = 257-difficulty_level               
        if (int.from_bytes(result, byteorder='little', signed=False) >> bit_shift) == 0:
            print("Golden Nonce: "+str(i))
            return


def main():
    """Main Function"""
    find_nonce(22)

if __name__ == '__main__':
    main()


