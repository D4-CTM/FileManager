#include "Partition.hpp"

const void Disk_Partitioner::create(const std::string &partitionName, size_t &block_Cant, size_t &block_Size)
{
    std::ofstream writer(ROOT + "/" + partitionName, std::ios::binary);

    if (writer.is_open()) 
    {
        writer.write(reinterpret_cast<char *>(&block_Cant), sizeof(size_t));
        writer.write(reinterpret_cast<char *>(&block_Size), sizeof(size_t));

        bool free;
        unsigned char* bytes;
        for (int i = 0; i < block_Cant; i++) {
            bytes = new unsigned char[block_Size - 1];
            writer.write(reinterpret_cast<char *>(&free), sizeof(bool));
            writer.write(reinterpret_cast<char *>(&bytes), sizeof(unsigned char) * (block_Size - 1));
            if (bytes) {
                delete [] bytes;
                bytes = nullptr;
            }
        }

        writer.close();
    }

}

const bool Disk_Partitioner::write(const int &blockPos, const std::vector<unsigned char>& text)
{
    if (partitionName.empty()) {
        std::cout << AnsiCodes::RED << "ERROR: please selcet a partition first" << '\n';
        return false;
    }

    if (blockPos > block_Cant) {
        std::cout << AnsiCodes::RED << "ERROR: " << partitionName << " only has " << block_Cant << " blocks!" << '\n';
        return false;
    }

    if (block_Size < text.size()) {
        std::cout << AnsiCodes::RED << "ERROR: the blocks can only hold " << block_Size << " chars!" << '\n';
        return false;
    }

    std::fstream writer(ROOT + "/" + partitionName, std::ios::in | std::ios::binary | std::ios::out);
    if (!writer.is_open()) {
        std::cout << AnsiCodes::RED << "ERROR: the file could't been open!" << '\n';
        return false;
    }

    int writtingPos = initialBlockPosition + (block_Size * blockPos);
    writer.seekp(writtingPos, std::ios::beg);

    bool free = false;
    writer.write(reinterpret_cast<char *>(&free), sizeof(bool));
    unsigned char* data = new unsigned char[block_Size];
    std::copy(text.begin(), text.end(), data);
    writer.write(reinterpret_cast<char *>(&data), sizeof(unsigned char) * (block_Size - 1));

    writer.close();
    return true;
}



const void Disk_Partitioner::select(const std::string &_partitionName)
{
    std::ifstream reader(ROOT + "/" + _partitionName, std::ios::binary);

    if (reader.is_open()) {
        reader.read(reinterpret_cast<char *>(&block_Cant), sizeof(size_t));
        reader.read(reinterpret_cast<char *>(&block_Size), sizeof(size_t));
        partitionName = _partitionName;
        reader.close();
    }
}

const void Disk_Partitioner::info()
{
    std::cout << "|----------------------------------|" << '\n';
    std::cout << "| Partition name: " << partitionName << '\n';
    std::cout << "| Partition size: " << (2 * sizeof(size_t)) + (block_Cant * (block_Size)) << '\n';
    std::cout << "| Block quantity: " << block_Cant << '\n';
    std::cout << "| Blocks size: " << block_Size << '\n';
    std::cout << "|----------------------------------|" << '\n';
}
