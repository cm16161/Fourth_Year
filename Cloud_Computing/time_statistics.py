'''This script will try to find a nonce with given parameters'''
import hashlib
import time

n_iterations = 10000

def find_nonce(block="COMSM0010cloud"):
    """This function will continually generate nonces and see if they
    are golden"""
    for i in range(1, 101):
        block_to_use = block
        sha_f = hashlib.sha256()
        block_to_use += str(i)
        block_bytes = bytes(block_to_use, 'ascii')
        sha_f.update(block_bytes)
        sha_f.update(sha_f.digest())
        result = sha_f.digest()
        bit_shift = 256-32
        if (int.from_bytes(result, byteorder='little', signed=False) >> bit_shift) == 0:
            print("Golden Nonce: "+str(i))



def main():
    """Main Function"""
    global_time = 0
    for i in range(1,n_iterations+1):
        start_time = time.time()
        find_nonce()
        end_time = time.time()
        global_time += (end_time - start_time)
        #print(end_time - start_time)

    print(global_time/n_iterations)

if __name__ == '__main__':
    main()
