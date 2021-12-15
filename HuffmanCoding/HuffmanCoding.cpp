
#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <cstdint>
#include <fstream>
#include <exception>


class BitArray
{
public:
	BitArray() = default;

	BitArray(const std::string& str)
	{
		for (char c : str)
		{
			addByte(c);
		}
	}

	BitArray(const std::vector<std::uint16_t>& data, int numberOfBits)
	{
		for (std::uint16_t num : data)
		{
			addBits(num, numberOfBits);
		}
	}

	std::string getAsString() const
	{
		std::string str;
		str.reserve(bytes());

		for (size_t i = 0; i < bytes(); i++)
		{
			str += getByte(i * 8);
		}

		return str;
	}

	bool getBit(size_t bitPos) const
	{
		return m_data[bitPos];
	}

	std::vector<bool> getBit(size_t bitPos, size_t count) const
	{
		std::vector<bool> bits;

		for (size_t i = 0; i < count; i++)
		{
			bits.push_back(m_data[bitPos + i]);
		}

		return bits;
	}

	std::uint16_t getBits(size_t bitPos, int numberOfBits) const
	{
		std::uint16_t number{};

		numberOfBits = static_cast<int>(std::min(bits() - bitPos, static_cast<size_t>(numberOfBits)));
		numberOfBits = std::max(numberOfBits, 0);

		for (int i = 0; i < numberOfBits; i++)
		{
			size_t idx = bitPos + i;

			number |= (m_data[idx] << (numberOfBits - 1 - i));
		}

		return number;
	}

	std::vector<std::uint16_t> getBits(size_t bitPos, size_t count, int numberOfBits) const
	{
		std::vector<std::uint16_t> data;

		size_t availableCount = bits() > bitPos ? bits() - bitPos : 0;
		availableCount = availableCount / numberOfBits + (availableCount % numberOfBits == 0 ? 0 : 1);

		count = std::min(availableCount, count);
		data.reserve(count);

		for (int i = 0; i < count; i++)
		{
			size_t idx = bitPos + i * numberOfBits;
			data.push_back(getBits(idx, numberOfBits));
		}

		return data;
	}

	std::uint8_t getByte(size_t bitOffset) const
	{
		return static_cast<std::uint8_t>(getBits(bitOffset, 8));
	}

	std::vector<std::uint8_t> getByte(size_t bitOffset, size_t count) const
	{
		std::vector<std::uint8_t> bytes;
		bytes.reserve(count);

		for (size_t i = 0; i < count; i++)
		{
			bytes.push_back(getByte(bitOffset + i * 8));
		}

		return bytes;
	}

	std::uint16_t get2Byte(size_t bitOffset) const
	{
		return getBits(bitOffset, 16);
	}

	std::uint32_t get4Byte(size_t bitOffset) const
	{
		std::uint16_t a = get2Byte(bitOffset);
		std::uint16_t b = get2Byte(bitOffset + 16);
		return (static_cast<std::uint32_t>(a) << 16) | b;
	}

	std::uint64_t get8Byte(size_t bitOffset) const
	{
		std::uint32_t a = get4Byte(bitOffset);
		std::uint32_t b = get4Byte(bitOffset + 32);
		return (static_cast<std::uint64_t>(a) << 32) | b;
	}

	std::vector<std::uint8_t> getData() const
	{
		if (bytes() == 0)
			return {};

		size_t count = bytes() - 1;

		auto data = getByte(0, count);
		std::uint8_t number{};

		for (int i = 0; i < bits() - count * 8; i++)
		{
			size_t idx = count * 8 + i;
			number |= (m_data[idx] << (7 - i));
		}

		data.push_back(number);
		return data;
	}

	std::vector<std::uint16_t> getDataAsBits(int numberOfBits) const
	{
		return getBits(0, -1, numberOfBits);
	}

	void addBit(bool bit)
	{
		m_data.push_back(bit);
	}

	void addBit(const std::vector<bool>& bits)
	{
		m_data.insert(m_data.end(), bits.begin(), bits.end());
	}

	void addBits(std::uint16_t number, int numberOfBits)
	{
		for (int i = 0; i < numberOfBits; i++)
		{
			bool isSet = (number & (1 << (numberOfBits - 1 - i))) != 0;
			m_data.push_back(isSet);
		}
	}

	void addByte(std::uint8_t byte)
	{
		addBits(byte, 8);
	}

	void add2Byte(std::uint16_t number)
	{
		addBits(number, 16);
	}

	void add4Byte(std::uint32_t number)
	{
		std::uint16_t a = static_cast<std::uint16_t>(number >> 16);
		std::uint16_t b = static_cast<std::uint16_t>(number);

		add2Byte(a);
		add2Byte(b);
	}

	void add8Byte(std::uint64_t number)
	{
		std::uint32_t a = static_cast<std::uint32_t>(number >> 32);
		std::uint32_t b = static_cast<std::uint32_t>(number);

		add4Byte(a);
		add4Byte(b);
	}

	void addByte(const std::vector<std::uint8_t>& bytes)
	{
		addByte(bytes, 0, bytes.size());
	}

	void addByte(const std::vector<std::uint8_t>& bytes, std::uint64_t start, std::uint64_t count)
	{
		count = std::min(bytes.size() - start, count);
		count = std::max(count, 0llu);

		reserveBits(bits() + count * 8);
		for (size_t i = 0; i < count; i++)
		{
			addByte(bytes[i + start]);
		}
	}

	void extend(const BitArray& arr)
	{
		m_data.insert(m_data.end(), arr.m_data.begin(), arr.m_data.end());
	}

	size_t bits() const
	{
		return m_data.size();
	}

	size_t bytes() const
	{
		int allignmentByte = bits() % 8 == 0 ? 0 : 1;
		size_t bytes = bits() / 8 + allignmentByte;

		return bytes;
	}

	void reserveBits(size_t bits)
	{
		m_data.reserve(bits);
	}

	void reserveBytes(size_t bytes)
	{
		reserveBits(bytes * 8);
	}

private:
	std::vector<bool> m_data;
};

class HuffmanNode
{
public:
	HuffmanNode(std::uint16_t character, int frequency)
	{
		m_character = character;
		m_frequency = frequency;
		m_left = m_right = nullptr;
	}

	HuffmanNode(HuffmanNode* left, HuffmanNode* right)
	{
		m_character = 0;
		m_left = left;
		m_right = right;
		m_frequency = 0;

		if (left && right)
			m_frequency = left->m_frequency + right->m_frequency;
	}

	~HuffmanNode()
	{
		delete m_left;
		delete m_right;
	}

	bool operator>(const HuffmanNode& rhs) const { return m_frequency > rhs.m_frequency; }
	bool isLeaf() const { return m_left == nullptr; }

	std::uint16_t m_character;
	int m_frequency;

	HuffmanNode* m_left;
	HuffmanNode* m_right;
};

class HuffmanTree
{
public:
	HuffmanTree(std::uint8_t bitCount)
		: m_bitCount(bitCount), m_symbolCount(1 << bitCount), m_frequencies(m_symbolCount), m_codes(m_symbolCount)
	{
		if (bitCount < 1 || bitCount > 16)
			throw std::invalid_argument("Bit count has to be in [1, 16] range!");
	}

	~HuffmanTree()
	{
		cleanUp();
	}

	BitArray compress(const std::string& text)
	{
		if (text.size() * 8 % m_bitCount != 0)
			throw std::invalid_argument("Total number of bits has to be dividible by the set bit count");

		cleanUp();

		m_data = BitArray(text).getDataAsBits(m_bitCount);

		// find character frequencies
		for (std::uint16_t c : m_data)
		{
			m_frequencies[c]++;
		}

		// add all unique chars with their frequences to min heap
		for (int c = 0; c < m_symbolCount; c++)
		{
			if (m_frequencies[c] > 0)
			{
				HuffmanNode* node = new HuffmanNode(c, m_frequencies[c]);
				m_minHeap.push(node);
			}
		}

		// create a single root node from all nodes
		while (m_minHeap.size() > 1)
		{
			HuffmanNode* left = heapPop();
			HuffmanNode* right = heapPop();

			HuffmanNode* node = new HuffmanNode(left, right);
			heapPush(node);
		}

		findCodes(m_minHeap.top());
		BitArray compressedData = getCompressedData();

		size_t uncompressedBits = m_data.size() * m_bitCount;

		std::cout << "input symbols: " << m_data.size() << ", bits: " << uncompressedBits << std::endl;
		std::cout << "compressor bit count: " << static_cast<int>(m_bitCount) << std::endl;
		std::cout << "compressed bits: " << compressedSize() << ", compression ratio: " << static_cast<double>(uncompressedBits) / compressedSize() << std::endl;
		std::cout << "compressed bits with header: " << compressedData.bits() << ", compression ratio: " << static_cast<double>(uncompressedBits) / compressedData.bits() << std::endl;
		std::cout << std::endl;

		return compressedData;
	}

	std::string decompress(const BitArray& compressedBlock)
	{
		cleanUp();
		// recreate codes

		size_t lengthIdx = 0;
		HuffmanNode* node = new HuffmanNode(nullptr, nullptr);
		heapPush(node);

		for (int c = 0; c < m_symbolCount; c++)
		{
			size_t codeIdx = lengthIdx + 8;

			std::uint8_t codeSize = compressedBlock.getByte(lengthIdx);
			lengthIdx = codeIdx + codeSize;

			if (codeSize > 0)
			{
				m_codes[c] = compressedBlock.getBit(codeIdx, codeSize);

				node = m_minHeap.top();

				for (int i = 0; i < codeSize; i++)
				{
					if (m_codes[c][i] == 0)
					{
						if (!node->m_left)
							node->m_left = new HuffmanNode(nullptr, nullptr);

						node = node->m_left;
					}
					else
					{
						if (!node->m_right)
							node->m_right = new HuffmanNode(nullptr, nullptr);

						node = node->m_right;
					}
				}

				node->m_character = c;
			}


		}

		// decode
		std::uint64_t compressedDataLength = compressedBlock.get8Byte(lengthIdx);

		size_t dataIdx = lengthIdx + 8 * 8;
		auto data = compressedBlock.getBit(dataIdx, compressedDataLength);

		node = m_minHeap.top();

		for (bool cd : data)
		{
			if (cd == 0)
			{
				node = node->m_left;
			}
			else
			{
				node = node->m_right;
			}

			if (node->isLeaf())
			{
				m_data.push_back(node->m_character);
				node = m_minHeap.top();
			}
		}

		return BitArray(m_data, m_bitCount).getAsString();
	}

	void compressFullFile(const std::string& inputFile, const std::string& outputFile)
	{
		std::ifstream fin(inputFile, std::ios::binary | std::ios::in);
		size_t size = getInputFileStreamSize(fin);

		std::string str(size, '\0');
		fin.read(str.data(), size);

		BitArray compressedBlock = compress(str);
		std::vector<uint8_t> data = compressedBlock.getData();

		std::ofstream fout(outputFile, std::ios::binary | std::ios::out);
		fout.write(reinterpret_cast<char*>(data.data()), data.size());
	}

	void decompressFullFile(const std::string& inputFile, const std::string& outputFile)
	{
		std::ifstream fin(inputFile, std::ios::binary | std::ios::in);

		size_t size = getInputFileStreamSize(fin);

		BitArray compressedBlock;
		compressedBlock.reserveBytes(size);

		static constexpr int chunkSize = 64 * 1024;

		std::vector<uint8_t> tmpData(chunkSize);

		while (fin.good())
		{
			fin.read(reinterpret_cast<char*>(tmpData.data()), chunkSize);
			auto readSize = fin.gcount();

			compressedBlock.addByte(tmpData, 0, readSize);
		}

		std::string decompressedStr = decompress(compressedBlock);

		std::ofstream fout(outputFile, std::ios::binary | std::ios::out);
		fout.write(decompressedStr.data(), decompressedStr.size());
	}

	static size_t getInputFileStreamSize(std::ifstream& fin)
	{
		auto start = fin.tellg();
		fin.seekg(0, std::ios::end);
		auto size = fin.tellg() - start;
		fin.seekg(0, std::ios::beg);

		return static_cast<size_t>(size);
	}

private:
	void cleanUp()
	{
		while (!m_minHeap.empty())
		{
			HuffmanNode* node = heapPop();
			delete node;
		}

		m_data.clear();
		m_frequencies = std::vector<int>(m_symbolCount);
		m_codes = std::vector<std::vector<bool>>(m_symbolCount);
	}

	HuffmanNode* heapPop()
	{
		HuffmanNode* node = m_minHeap.top();
		m_minHeap.pop();

		return node;
	}

	void heapPush(HuffmanNode* node)
	{
		m_minHeap.push(node);
	}

	void findCodes(const HuffmanNode* root)
	{
		std::vector<bool> arr;
		findCodesRec(root, arr);
	}

	void findCodesRec(const HuffmanNode* node, const std::vector<bool>& code)
	{
		if (!node)
			return;

		if (node && node->isLeaf())
		{
			m_codes[node->m_character] = code;
		}
		else
		{
			std::vector<bool> code1 = code;
			code1.push_back(0);
			findCodesRec(node->m_left, code1);

			std::vector<bool> code2 = code;
			code2.push_back(1);
			findCodesRec(node->m_right, code2);
		}
	}

	// in bits
	std::uint64_t compressedSize() const
	{
		std::uint64_t size = 0;
		for (int c = 0; c < m_symbolCount; c++)
		{
			if (m_frequencies[c] > 0)
			{
				size += m_frequencies[c] * m_codes[c].size();
			}
		}

		return size;
	}

	// codes always 0-255?
	BitArray getCodeBlock() const
	{
		BitArray codeBlock;

		for (int c = 0; c < m_symbolCount; c++)
		{
			std::vector<bool> code = m_codes[c];
			std::uint8_t length = static_cast<std::uint8_t>(code.size());
			codeBlock.addByte(length);

			if (length > 0)
				codeBlock.addBit(code);
		}

		return codeBlock;
	}

	BitArray getCompressedBlock() const
	{
		BitArray compressedBlock;

		std::uint64_t size = compressedSize();
		compressedBlock.add8Byte(size);

		for (std::uint16_t c : m_data)
		{
			compressedBlock.addBit(m_codes[c]);
		}

		return compressedBlock;
	}

	BitArray getCompressedData() const
	{
		BitArray result;

		result.extend(getCodeBlock());
		result.extend(getCompressedBlock());

		return result;
	}

private:
	struct HuffmanCompare
	{
		bool operator()(const HuffmanNode* lhs, const HuffmanNode* rhs) const { return *lhs > *rhs; };
	};

	std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, HuffmanCompare> m_minHeap;

	std::uint8_t m_bitCount;
	int m_symbolCount;
	std::vector<int> m_frequencies;
	std::vector<std::vector<bool>> m_codes;
	std::vector<std::uint16_t> m_data;
};

bool compareFiles(const std::string& file1, const std::string& file2)
{
	std::ifstream fin1(file1, std::ios::binary | std::ios::in);
	std::ifstream fin2(file1, std::ios::binary | std::ios::in);

	size_t size = HuffmanTree::getInputFileStreamSize(fin1);

	if (size != HuffmanTree::getInputFileStreamSize(fin2))
		return false;

	std::vector<char> chunk1(size);
	std::vector<char> chunk2(size);

	fin1.read(chunk1.data(), size);
	fin2.read(chunk2.data(), size);

	return std::memcmp(chunk1.data(), chunk2.data(), size) == 0;
}

void compressDecompress(const std::string& file1, const std::string& file2, int bits)
{
	HuffmanTree tree(bits);
	HuffmanTree tree2(bits);

	try
	{
		tree.compressFullFile(file1, file2);
		tree2.decompressFullFile(file2, "tmp");

		/*if (!compareFiles(file1, "tmp"))
		{
			std::cout << "    decompression error!" << std::endl;
		}*/
	}
	catch (const std::invalid_argument&) {}
}

int main()
{
	//std::string uncompressed = "lorem_ipsum.txt";
	//std::string uncompressed = "lorem_ipsum_long.txt";
	//std::string uncompressed = "white_image.bmp";
	std::string uncompressed = "test.txt";

	std::string compressed = "test.hfmn";

	for (int i = 1; i <= 16; i++)
	{
		compressDecompress(uncompressed, compressed + std::to_string(i), i);
	}
}

