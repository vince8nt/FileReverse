// file_reverse.cpp
#include <iostream>
#include <fstream>
#include <array>
#include <algorithm>

std::array<char, 4096> buffer;

// read until newline
// resulting buffer might only contain ending portion of the line (for super long lines)
bool read_until_newline(std::ifstream& in_file) {
    in_file.getline(buffer.data(), buffer.size());
    while (!in_file.good()) {
        if (in_file.bad()) {
            std::cerr << "Error: failed to read from input file: " << std::endl;
            exit(1);
        }
        if (in_file.eof()) {
            return true;
        }
        in_file.getline(buffer.data(), buffer.size());
    }
    return false;
}

// write reverse line to output file
// for super long lines, we will need to repeatidly seek backwards + read from input file
void write_reverse_line(std::ifstream& in_file, const size_t line_start, std::ofstream& out_file) {
    const size_t line_end = in_file.tellg();
    do {
        // reverse the buffer and write to output file
        std::reverse(buffer.begin(), buffer.begin() + in_file.gcount());
        out_file.write(buffer.data(), in_file.gcount());
        if (out_file.fail()) {
            std::cerr << "Error: failed to write to output file: " << std::endl;
            exit(1);
        }
    } while ([&in_file, &out_file, line_start]() {
            // read backwards if needed
            size_t buffer_start = in_file.tellg() - in_file.gcount();
            size_t remaining = buffer_start - line_start;
            if (remaining > 0) {
                size_t read_size = std::min(remaining, buffer.size());
                in_file.seekg(buffer_start - read_size);
                in_file.read(buffer.data(), read_size);
                if (in_file.bad()) {
                    std::cerr << "Error: failed to read from input file: " << std::endl;
                    exit(1);
                }
                return true;
            }
            return false;
        }());
    // write newline
    out_file.write("\n", 1);
    if (out_file.fail()) {
        std::cerr << "Error: failed to write to output file: " << std::endl;
        exit(1);
    }
    // seek back to end of line (for the case where we needed to read backwards)
    if (in_file.tellg() != line_end) {
        in_file.seekg(line_end);
    }
}

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
    std::ofstream out_file(argv[2], std::ofstream::out);
    if (!out_file.is_open()) {
        std::cerr << "Error: failed to open output file: " << argv[2] << std::endl;
        exit(1);
    }

    // read file line by line:
    //// for short lines, fstream's internal buffering will make this efficient
    //// for super long lines, we repeatidly read a 4096 byte chunk until we find the newline
    bool eof = false;
    while (!eof) {
        size_t line_start = in_file.tellg();
        eof = read_until_newline(in_file);
        write_reverse_line(in_file, line_start, out_file);
    }
    
}
