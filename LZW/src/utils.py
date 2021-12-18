def read(fileName):
    with open(fileName, "rb") as stream:
        data = stream.read()
        if data:
            yield data

def readArray(fileName):
    bits = ''
    with open(fileName, 'rb') as f:
        while 1:
            byte_s = f.read(1)
            if not byte_s:
                break
            byte = byte_s[0]
            text = format(byte, f'0{8}b')
            bits += text
    return bits