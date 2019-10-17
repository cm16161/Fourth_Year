'''This script will try to find a nonce with given parameters'''
import hashlib
import threading
import argparse

def get_args():
    '''Get the commandline arguments passed into the program'''
    parser = argparse.ArgumentParser()
    parser.add_argument("-n", "--n_threads", help="Input the number of threads desired",
                        type=int, default="1")
    parser.add_argument("-d", "--difficulty", help="Input the difficulty value",
                        type=int, default="32")
    args = parser.parse_args()
    return args

def find_nonce(difficulty_level, start_val=0, end_val=2**32, block="COMSM0010cloud"):
    """This function will continually generate nonces and see if they
    are golden"""
    for i in range(start_val, end_val):
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
    args = get_args()
    threads = list()
    for _t in range(args.n_threads):
        start_val = (2**32 / 4)*_t
        end_val = (2**32 / 4)*(_t+1)
        _x = threading.Thread(target=find_nonce, args=(args.difficulty,
                                                       int(start_val), int(end_val),))
        threads.append(_x)
        _x.start()

    for _t in threads:
        _t.join()

if __name__ == '__main__':
    main()
