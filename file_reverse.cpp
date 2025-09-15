// file_reverse.cpp
#include <iostream>
#include <fstream>
#include <array>

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: file_reverse <input file path> <output file path>" << std::endl;
        exit(1);
    }

    std::ifstream in_file(argv[1], std::ifstream::in);
    if (!in_file.is_open()) {
        std::cerr << "Error: failed to open input file: " << argv[1] << std::endl;
        exit(1);
    }

    std::ifstream out_file(argv[2], std::ofstream::out);
    if (!out_file.is_open()) {
        std::cerr << "Error: failed to open output file: " << argv[2] << std::endl;
        exit(1);
    }

    // read file line by line:
        // for short lines, fstream's internal buffering will make this efficient
        // for super long lines, we repeatidly read a 4096 byte chunk until we find the newline
    std::array<char, 4097> buffer; // 4096 + null terminator
    while (true) {
        in_file.getline(buffer.data(), buffer.size());
        if (!in_file.good()) {
            if (in_file.fail()) {

            } else if (in_file.eof()) {
                
            } else {
                std::cerr << "Error: failed to open output file: " << argv[2] << std::endl;
                exit(1);
            }
            
        }
    }
    

}
