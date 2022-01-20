from ast import parse
import os
import sys
import time
import argparse

from decoder import LZWDecoding
from encoder import LZWEncoding


parser = argparse.ArgumentParser()
parser.add_argument('--path', help='path of the file used', required=True)
parser.add_argument('--bits', help='bits used to encode one word' , required=True)
args = parser.parse_args()

path = args.path
bits = int(args.bits)

if __name__ == "__main__":
    if bits < 8 or bits > 15:
        sys.exit('Library size is set incorrectly, bits should be between 8 and 15')

    filename, file_extension = os.path.splitext(path)

    # encoding
    start_time = time.time()
    encoding = LZWEncoding(path, bits)
    encoded_path = encoding.lzw_compress()
    print(f'Compressed to {encoded_path}')
    print(f'Before compresssion: {os.path.getsize(path)}')
    print(f'After compresssion:  {os.path.getsize(encoded_path)}')
    print(f"Compression time: {time.time() - start_time:.3f} seconds, compression ratio: {os.path.getsize(path) / os.path.getsize(encoded_path):.3f}\n")

    #decoding
    start_time = time.time()
    encoded_file = f'{filename}.lzw'
    decoded_file = f'{filename}.lzwdecoded'
    decoding = LZWDecoding(encoded_file)
    decoding.lzw_decompress(decoded_file)
    print(f'Decompressed to {decoded_file}')
    print(f'Before decompression:     {os.path.getsize(encoded_file)}')
    print(f'After Decompresssion:  {os.path.getsize(decoded_file)}')
    print(f"Decompression time: {time.time() - start_time:.3f} seconds")
