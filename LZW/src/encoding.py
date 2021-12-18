import os
from utils import read
from array import array

# 1 Initialize the dictionary to contain all strings of length one.
dictionary: dict = {i.to_bytes(1, 'big'): i for i in range(256)}
keys = dictionary.copy() # we need to make a copy because dictionary itself is not iterable

def encode(inputFile, bitCount):
    outputFile = os.path.splitext(inputFile)[0] + '.lzw'
    output = open(outputFile, 'wb')
    data = read(inputFile) # all bytes from the file

    word = b''
    skip = 0
    bits = ''

    for chunk in data:
        for idx, byte in enumerate(chunk):
            if skip > 0:
                skip -= 1
                continue
            word += byte.to_bytes(1, byteorder='big')
            tIdx = idx
            while word in keys:
                tIdx += 1
                if tIdx == len(chunk):
                    break
                word += chunk[tIdx].to_bytes(1, byteorder='big')

            temp = word
            if len(word) > 1:
                keys[word] = len(keys)
                skip = len(word) - 2 # we always need to start from the last symbol of a new entry to dictionary
                temp = word[:-1]
                print(keys)
            

            text = format(keys[temp], f'0{bitCount}b')
            bits += text
            word = b''
            while len(bits) >= 8:
                byte = bits[0:8]
                bits = bits[8:]
                binarray = array('B')
                binarray.append(int(byte, 2))
                output.write(binarray)

    if len(bits) > 0:
        while len(bits) < 8:
            bits += '0'
        byte = bits[0:8]
        bits = bits[8:]
        binarray = array('B')
        binarray.append(int(byte, 2))
        output.write(binarray)
    output.close()

if __name__ == "__main__":
    inputFile = 'input.txt'
    encode(inputFile, 9)