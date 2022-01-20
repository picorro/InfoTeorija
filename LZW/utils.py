import math

chunk_size = 1024


def number_of_bits(k, max_size):
    return int(math.log2(max_size + k))


def yield_bytes_from_stream(stream):
    """Returns a chunk of bytes from a stream"""
    while True:
        chunk = stream.read(chunk_size)
        if chunk:
            yield chunk
        else:
            break


def yield_from_file(path_to_file):
    """Returns a chunk of bytes from a file"""
    with open(path_to_file, "rb") as stream:
        while True:
            data = stream.read(chunk_size)
            if data:
                yield data
            else:
                break
