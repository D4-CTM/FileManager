#ifndef __Disk_Partitioner_HPP__
#define __Disk_Partitioner_HPP__
#include "TextColor.h"
#include <iostream>
#include <fstream>
#include <vector>

class Disk_Partitioner {
private:
    const int initialBlockPosition = 2 * (sizeof(size_t));
    //Folder that includes each partition made by the user
    const std::string ROOT = "./Partitions";
    std::string partitionName;
    size_t block_Cant;
    size_t block_Size;
public:
    Disk_Partitioner() 
    : partitionName(""), block_Cant(0), block_Size(0)
    {}

    const void create(const std::string& partitionName, size_t& block_Cant, size_t& block_Size);
    const bool write(const int& blockPos, const std::vector<unsigned char>& bytes);
    const void select(const std::string& partitionName);
    const bool read(int& blockPos);
    const void info();

    const inline std::string getPartitionPath() { return partitionName = "" ? "" : ROOT + "/" + partitionName; }
    const inline std::string getPartitionName() { return partitionName; }
    const inline std::string getRoot() { return ROOT; }
    constexpr inline size_t getBlockCant() { return block_Cant; }
    constexpr inline size_t getBlockSize() { return block_Size; }
    const inline void clearSuperblock() {
        partitionName = "";
        block_Cant = 0;
        block_Size = 0;
    }

    ~Disk_Partitioner()
    {}
};

#endif  //__Disk_Partitioner_HPP__