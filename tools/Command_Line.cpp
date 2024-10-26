#include "Command_Line.hpp"

Command_Line::Command_Line()
: partitioner(new Disk_Partitioner), run(true)
{
    if (!std::filesystem::exists(partitioner->getRoot())) {
        std::filesystem::create_directory(partitioner->getRoot());
    }
}

const void Command_Line::doCommand(const std::string &methods)
{
    if (methods.empty()) return ;
    const auto lines = split(methods);
    
    if (lines[0].empty()) throw std::runtime_error("ERROR: please input first the command!");

    if (lines[0] == KILL) {
    
        run = false;
    
    } else if (lines[0] == CLEAR) {
    
        clearScreen();
    
    } else if (lines[0] == LIST_ELEMENTS) {
    
        listElements();
    
    } else if (lines[0] == HELP_ME) {
    
        helpMe();

    } else if (lines[0] == CREATE_PARTITION) {
        
        createPartition(lines);
    
    } else if (lines[0] == SELECT_PARTITION) {
    
        if (lines.size() > 1 && !lines[1].empty()) {
    
            selectPartition(lines[1]);
    
        } else std::cout << AnsiCodes::RED << "ERROR: Please input something the partition\'s name!\n" << AnsiCodes::DEFAULT << "For reference you could use the \'--help\' command" << '\n';
    
    } else if (lines[0] == PARTITION_INFO) {
    
        if (!partitioner->getPartitionName().empty()) {
            std::cout << AnsiCodes::GREEN;

            partitioner->info();

            std::cout << AnsiCodes::DEFAULT;
        } else std::cout << AnsiCodes::RED << "ERROR: Please select a partition first!\n" << AnsiCodes::DEFAULT << "For reference you could use the \'--help\' command" << '\n';
    
    } else if (lines[0] == WRITE_INFO) {

        writeInformation(lines, methods);

    } else if (lines[0] == READ_INFO) {

    }

}

const bool isNumeric(const std::string& str) {
    int i;
    for (auto& c : str) {
        for (i = 0; i < 10; i++) {
            if (c == ('0' + i)) {
                break;
            }
            if (i == 9) return false;
        }
    }
    return true;
}

const void Command_Line::createPartition(const std::vector<std::string>& partitionInfo) {
    const int size = partitionInfo.size();
    if (size <= 1) {
        std::cout << AnsiCodes::RED << "ERROR: please input the partition name!" << '\n';
        return ;
    }
    std::string fileName = partitionInfo[1];

    if (size <= 2) {
        std::cout << AnsiCodes::RED << "ERROR: please digit the size of each block!" << '\n';
        return ;
    }

    if (!isNumeric(partitionInfo[2])) {
        std::cout << AnsiCodes::RED << "ERROR: the input for the block size must be a possitive integer!" << '\n';
        return ;
    }
    size_t blocks_size = std::stoi(partitionInfo[2]);

    if (blocks_size == 0) {
        std::cout << AnsiCodes::RED << "ERROR: the input for the block size must be a greater than 0!" << '\n';
        return ;
    }

    if (size <= 3) {
        std::cout << AnsiCodes::RED << "ERROR: please digit the quantity of blocks on the partition!" << '\n';
        return ;
    }

    if (!isNumeric(partitionInfo[3])) {
        std::cout << AnsiCodes::RED << "ERROR: the input for the blocks quantity must be a possitive integer!" << '\n';
        return ;
    }
    size_t block_cant = std::stoi(partitionInfo[3]);

    if (block_cant == 0) {
        std::cout << AnsiCodes::RED << "ERROR: the input for the blocks quantity must be a greater than 0!" << '\n';
        return ;
    }

    partitioner->create(fileName, block_cant, blocks_size);
}

const void Command_Line::selectPartition(const std::string &fileName)
{
    if (fileName == "..") {
        partitioner->clearSuperblock();
        return ;
    }

    if (!partitioner->getPartitionName().empty()) {
        //change added to make sense of the 'close()' function asked.
        std::cout << AnsiCodes::RED << "ERROR: please close the current partition before attempting to change into a new one" << '\n';
        return ;
    }

    if (!std::filesystem::exists(partitioner->getRoot() + "/" + partitioner->getPartitionName())) {
        std::cout << AnsiCodes::RED << "ERROR: the partition doesn't exists!\n" << AnsiCodes::DEFAULT << "For reference you can use the command \'--help\'" << '\n';
        return ;
    }
    partitioner->select(fileName);
}

const void Command_Line::listElements()
{
    for (const auto& element : std::filesystem::directory_iterator(partitioner->getRoot())) {
        if (element.is_regular_file()) {
            std::cout << AnsiCodes::BLUE << std::filesystem::path(element).filename().string() << AnsiCodes::DEFAULT << '\t';
        }
    }
    std::cout << '\n';
}

std::vector<unsigned char> getInnerString(std::string str) {
    int initialPos = 0;
    int finalPos = 0;

    for (int i = 0; i < str.length(); i++) {

        if (str[i] == '\"') {

            if (initialPos == 0) {
                initialPos = i + 1;
            } else {
                finalPos = i;
            }

        }

    }

    if (finalPos <= initialPos) return {' '};

    std::vector<unsigned char> text;

    for (int i = initialPos; i < finalPos; i++) {
        text.push_back(str[i]);
    }

    return text;
}

const void Command_Line::writeInformation(const std::vector<std::string>& partitionInfo, const std::string& originalCommand)
{
    const int size = partitionInfo.size();

    if (size <= 1) {
        std::cout << AnsiCodes::RED << "ERROR: please input the block in which you'll like to write something!" << '\n';
        return;
    }

    if (!isNumeric(partitionInfo[1])) {
        std::cout << AnsiCodes::RED << "ERROR: please digit a numeric value for the position to which you'll write." << '\n';
        return;
    }
    const int writtingPos = std::stoi(partitionInfo[1]);

    if (size <= 2) {
        std::cout << AnsiCodes::RED << "ERROR: please input the data you'll store on this block!" << '\n';
        return;
    }
    const auto text = getInnerString(originalCommand);   

    if (text.size() == 1) {
        std::cout << AnsiCodes::RED << "ERROR: please type the data you need on the correct format!" << '\n';
        return;
    }

    partitioner->write(writtingPos, text);
}

const std::vector<std::string> Command_Line::split(const std::string& command)
{
    std::vector<std::string> lines;
    int lastPoint = 0;
    for (int i = 0; i < command.length(); i++) {
        if (command[i] == ' ') {
            lines.push_back(command.substr(lastPoint, i - lastPoint));
            lastPoint = i + 1;
        }
    }
    lines.push_back(command.substr(lastPoint, command.length() - lastPoint));
    return lines;
}

const void Command_Line::helpMe()
{
    std::cout << AnsiCodes::BLUE;
    std::cout << HELP_ME << ": display a list of all commands" << '\n';
    std::cout << "Commonly use commands" << '\n';
    std::cout << '\t' << LIST_ELEMENTS << ": display's a list of all partitions" << '\n';
    std::cout << '\t' << KILL << ": exit's this program" << '\n';
    std::cout << '\t' << CLEAR << ": remove's every element on the screen, for a cleaner terminal." << '\n';
    std::cout << "\nPartition tools:" << '\n';
    std::cout << '\t' << CREATE_PARTITION << " <partition name> <blocks size> <block quantity>: creates a partition with the specified name, size of each block and how many blocks to create" << '\n';
    std::cout << '\t' << SELECT_PARTITION << " <partition name> | <..>: selects the partition with the specified name." << '\n';
    std::cout << "\t\t\t\t  " << "| go back to the root file \'~\' closing the current partition." << '\n';
    std::cout << '\t' << PARTITION_INFO << ": displays some basic information about the selected partition" << '\n'; 
    std::cout << '\t' << WRITE_INFO << " <block> <data>: writed the data to the specified block, wrap the data between some \"\"" << '\n';
//    std::cout << "\t" << '\n';
    std::cout << AnsiCodes::DEFAULT << '\n';
}
