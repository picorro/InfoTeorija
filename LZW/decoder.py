
import utils
from bitarray import bitarray
from encoder import ASCII_TO_INT

INT_TO_ASCII: dict = {i: b for b, i in ASCII_TO_INT.items()}  # for decoding

class LZWDecoding:

    def __init__(self, encoded_path):
        self.reverse_lzw_mapping = INT_TO_ASCII.copy()
        self.dictionaryLength = len(INT_TO_ASCII)
        self.encoded_path = encoded_path
    def lzw_decompress(self, output_path):
        with open(self.encoded_path, 'rb') as file, open(output_path, 'wb') as output:
            buffer = bitarray()
            decoded_text = bytearray()

            buffer.frombytes(file.read())
            data = buffer.to01()

            padding = int(data[:4], 2)
            data = data[4 + padding:]
            self.bitLength = int(data[:8], 2) # first byte contains bit length for encoding
            allkeys = data[8:]

            pos = self.bitLength
            previous = int(allkeys[0:0 + self.bitLength], 2) 
            decoded_text.extend(self.reverse_lzw_mapping[previous]) # first byte is always the same

            while pos != len(allkeys):
                if self.dictionaryLength == (2 ** self.bitLength-1):  # if dictionary is full
                    self.reverse_lzw_mapping = INT_TO_ASCII.copy() # reset dictionary to original
                    self.dictionaryLength = len(INT_TO_ASCII)


                key = int(allkeys[pos:pos + self.bitLength], 2)  # read from current position to defined bit length, starts at second word
                if key != self.dictionaryLength: # key is not a new one
                    word = self.reverse_lzw_mapping[previous] + self.reverse_lzw_mapping[key][0:1] # take previous word and add the first char of a new word to it
                else:
                    word = self.reverse_lzw_mapping[previous] + self.reverse_lzw_mapping[previous][0:1]

                self.reverse_lzw_mapping[self.dictionaryLength] = word
                self.dictionaryLength += 1
                previous = key

                decoded_text.extend(self.reverse_lzw_mapping[key])
                pos += self.bitLength

            output.write(decoded_text)
            print("LZW Decompressed")