import os
import utils
from bitarray import bitarray

ASCII_TO_INT: dict = {i.to_bytes(1, 'big'): i for i in range(256)}  # for encoding




class LZWEncoding:
    def __init__(self, path, param):
        """
        :param path: file path to compress
        :param param: max dictionary size
        """
        self.path = path
        self.keys = ASCII_TO_INT.copy()  # key = bytes
        self.n_keys = len(ASCII_TO_INT)  # length of dictionary
        self.param = param

    def lzw_compress(self):
        """
        compress the file located in self.path using lzw.
        :return: output_path. saving the compressed data into filename + ".lzw"
        """
        filename, file_extension = os.path.splitext(self.path)
        output_path = filename + ".lzw"
        data = utils.yield_from_file(self.path)
        dec = bitarray()
        word = b''

        with open(output_path, 'wb') as output:
            dec.extend(bin(self.param)[2:].zfill(8))
            for chunk in data:
                for byte in chunk:

                    new_word = word + byte.to_bytes(1, byteorder='big')
                    if self.n_keys == (2**self.param-1) and self.param != 8:
                        self.keys = ASCII_TO_INT.copy()  # key = bytes
                        self.n_keys = len(ASCII_TO_INT)  # length of dictionary

                    if new_word in self.keys:
                        word = new_word
                    else:

                        number_of_bits = utils.number_of_bits(self.n_keys, 2**self.param)
                        dec.extend(bin(self.keys[word])[2:].zfill(number_of_bits))
                        self.keys[new_word] = self.n_keys
                        self.n_keys += 1
                        word = byte.to_bytes(1, byteorder='big')


            if word in self.keys:  # for last string
                number_of_bits = utils.number_of_bits(self.n_keys, 2**self.param)
                dec.extend(bin(self.keys[word])[2:].zfill(number_of_bits))

            p = 8 - (len(dec)+3)%8
            padding = f'{p:08b}'[-3:] + p*'0'
            temp = bitarray(padding)
            temp.extend(dec)
            temp.tofile(output)

        print("LZW Compressed")
        return output_path
