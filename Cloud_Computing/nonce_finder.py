'''This script will try to find a nonce with given parameters'''
import hashlib
import threading
import argparse

STOP_THREADS = False

def get_args():
    '''Get the commandline arguments passed into the program'''
    parser = argparse.ArgumentParser()
    parser.add_argument("-n", "--n_threads", help="Input the number of threads desired",
                        type=int, default="1")
    parser.add_argument("-d", "--difficulty", help="Input the difficulty value",
                        type=int, default="1")
    parser.add_argument("--start",type=int, default="0", help="Input the start value to search")
    parser.add_argument("--step",type=int, default="1", help="Input the step size to iterate through")
    parser.add_argument("--end", type=int, default=(2**32)+1, help="Input the end value to search up to")
    args = parser.parse_args()
    return args

def find_nonce(difficulty_level, start_val=0, step=1, end_val=2**32+1, block="COMSM0010cloud"):
    """This function will continually generate nonces and see if they
    are golden"""
    while True:
        for i in range(start_val, end_val, step):
            global STOP_THREADS
            sha_f = hashlib.sha256()
            block += str(i)
            block_bytes = bytes(block, 'ascii')
            sha_f.update(block_bytes)
            sha_f.update(sha_f.digest())
            result = sha_f.digest()
            bit_shift = 256-difficulty_level
            if STOP_THREADS:
                return
            if (int.from_bytes(result, byteorder='little', signed=False) >> bit_shift) == 0:
                print("Golden Nonce: "+str(i))
                STOP_THREADS = True
                return



def main():
    """Main Function"""
    args = get_args()
    start_val = args.start
    step_size = args.step
    difficulty= args.difficulty
    end_val = args.end
    find_nonce(difficulty, start_val, step_size, end_val)

if __name__ == '__main__':
    main()
