
import utils
from bitarray import bitarray
from encoder import ASCII_TO_INT

INT_TO_ASCII: dict = {i: b for b, i in ASCII_TO_INT.items()}  # for decoding



class LZWDecoding:

    def __init__(self, encoded_path):
        """
        :param encoded_path: file path to compress
        """
        self.reverse_lzw_mapping = INT_TO_ASCII.copy()  # key = dictionary length
        self.rev_keys = len(INT_TO_ASCII)
        self.encoded_path = encoded_path

    def lzw_decompress(self, output_path):
        """
        decompress input_file
        :param output_path: file decompressed saved to filename
        """

        with open(self.encoded_path, 'rb') as file, open(output_path, 'wb') as output:
            buffer = bitarray()
            buffer.frombytes(file.read())
            data = buffer.to01()
            padding = int(data[:3], 2)
            data = data[3 + padding:]
            self.param = int(data[:8], 2)
            allkeys = data[8:]
            decoded_text = bytearray()
            previous = -1
            pos = 0
            keys = []
            while True:

                if self.rev_keys == (2 ** self.param-1) and self.param != 8:  # init dictionary
                    self.reverse_lzw_mapping = INT_TO_ASCII.copy()
                    self.rev_keys = len(INT_TO_ASCII)


                number = utils.number_of_bits(self.rev_keys + 1, 2**self.param)

                key = int(allkeys[pos:pos + number], 2)  # read as many bits as encoded dynamically
                keys.append(key)

                if previous == -1:
                    previous = key
                else:
                    if key != self.rev_keys:
                        word = self.reverse_lzw_mapping[previous] + self.reverse_lzw_mapping[key][0:1]
                    else:
                        word = self.reverse_lzw_mapping[previous] + self.reverse_lzw_mapping[previous][0:1]
                    self.reverse_lzw_mapping[self.rev_keys] = word
                    self.rev_keys += 1
                    previous = key
                decoded_text.extend(self.reverse_lzw_mapping[key])
                pos += number
                if pos == len(allkeys):
                    break

            output.write(decoded_text)  # write to file
            print("LZW Decompressed")
