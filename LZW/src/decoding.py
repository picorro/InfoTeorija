import os
from utils import readArray
from array import array
dictionary: dict = {i.to_bytes(1, 'big'): i for i in range(256)}
reverseDictionary = {}
keys = reverseDictionary.copy()

def decode(inputFile, bitCount):
    global dictionary
    outputFile = os.path.splitext(inputFile)[0] + '.lzwdecoded'
    output = open(outputFile, 'wb')
    bits = readArray(inputFile) # all bytes from the file
    word = b''
    bitCountInts = []

    while len(bits) > bitCount:
        bitCountInt = int(bits[0:bitCount], 2)
        bits = bits[bitCount:]
        bitCountInts.append(bitCountInt)

    items = []
    for idx, bitCountInt in enumerate(bitCountInts):
        #print(bitCountInt)
        if bitCountInts[idx] >= 256:
            for item in keys[bitCountInts[idx]]:
                items.append(item)
        else:
            items.append(bitCountInts[idx])

        if idx < len(bitCountInts) - 1:
            if bitCountInts[idx + 1] >= 256:
                for item in keys[bitCountInts[idx + 1]]:
                    items.append(item)
            else:
                items.append(bitCountInts[idx + 1])
        if len(items) > 1:
            if len(items) > 3:
                items = items[:-1]
                keys[len(keys) + 256] = items
            else:
                keys[len(keys) + 256] = items
        #print(items)

        if len(items) > 1:
            items = items[:-1]
        if bitCountInt < 256:
            items = items[0:1]

        for item in items:
            out = item.to_bytes(1, byteorder='big')
            output.write(out)

        items = []
    output.close()



if __name__ == "__main__":
    inputFile = 'input.lzw'
    decode(inputFile, 9)