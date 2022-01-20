import os
import utils
from bitarray import bitarray

ASCII_TO_INT: dict = {i.to_bytes(1, 'big'): i for i in range(256)}  # starting dictionary contains all ascii characters, words are keys, and numbers are values

class LZWEncoding:
    def __init__(self, path, bitLength): # param^2 is max dictionary size
        self.path = path
        self.keys = ASCII_TO_INT.copy()  # key = byte
        self.dictionaryLength = len(ASCII_TO_INT)  # length of dictionary, starts at 256
        self.bitLength = bitLength

    def lzw_compress(self):
        filename, file_extension = os.path.splitext(self.path) # splits file name from extension
        output_path = filename + ".lzw" # adds lzw extension to encoded file
        data = utils.yield_from_file(self.path) # extracts file contents
        bits = bitarray()
        word = b'' # b for binary

        with open(output_path, 'wb') as output:
            bits.extend(bin(self.bitLength)[2:].zfill(8)) # add encoding bit amount to file
            for chunk in data:
                for byte in chunk:
                    new_word = word + byte.to_bytes(1, byteorder='big') # adds one more letter to current word

                    if self.dictionaryLength == (2**self.bitLength-1) and self.bitLength != 8: # if dictionary is full
                        self.keys = ASCII_TO_INT.copy() # reset the dictionary
                        self.dictionaryLength = len(ASCII_TO_INT)

                    if new_word in self.keys: # if word is currently in the dictionary
                        word = new_word
                    else: # word is not in the dictionary
                        bits.extend(bin(self.keys[word])[2:].zfill(self.bitLength))
                        self.keys[new_word] = self.dictionaryLength # add word to the dictionary
                        self.dictionaryLength += 1
                        word = byte.to_bytes(1, byteorder='big') # converts word into one byte


            if word in self.keys:  # for last word
                bits.extend(bin(self.keys[word])[2:].zfill(self.bitLength))

            p = 8 - (len(bits) + 3) % 8
            padding = f'{p:08b}'[-3:] + p*'0'
            temp = bitarray(padding)
            temp.extend(bits)
            temp.tofile(output)
        return output_path
