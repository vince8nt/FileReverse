// file_reverse.cpp
#include <iostream>
#include <fstream>
#include <array>
#include <algorithm>

// fixed size buffer stored on the stack
// better performance than std::string
std::array<char, 4096> buffer;

// read until newline or eof
// resulting buffer might only contain ending portion of the line (for lines longer than the buffer)
// returns the file position cooresponding to the start of the buffer
size_t read_until_newline(std::ifstream& in_file) {
    size_t buffer_start = in_file.tellg();
    in_file.getline(buffer.data(), buffer.size());
    while (true) {
        if (in_file.bad()) {
            std::cerr << "Error: failed to read from input file: " << std::endl;
            exit(1);
        } else if (in_file.fail()) {
            buffer_start = in_file.tellg();
            in_file.getline(buffer.data(), buffer.size());
        } else {
            return buffer_start;
        }
    }
}

// reverse the (portion of) buffer and write to output file
void reverse_write_buffer(std::ofstream& out_file, const size_t size) {
    std::reverse(buffer.begin(), buffer.begin() + size);
    out_file.write(buffer.data(), size);
    if (out_file.fail()) {
        std::cerr << "Error: failed to write to output file: " << std::endl;
        exit(1);
    }
}

// writes the reverse line to the output file
// in_file: the input file stream (positioned after reading the line)
// out_file: the output file stream
// line_start: the file position cooresponding to the start of the line
// buffer_start: the file position cooresponding to the start of the buffer
// returns true if eof is reached
bool write_reverse_line(std::ifstream& in_file, std::ofstream& out_file) {
    const size_t line_start = in_file.tellg();
    size_t buffer_start = read_until_newline(in_file);
    const size_t next_buffer_start = in_file.tellg();

    // if eof is not reached, gcount() will include the newline (which wasn't read into the buffer)
    // this works for both Unix/Linux (newline is \n) and Windows (newline is \r\n)
    bool eof = in_file.eof();
    reverse_write_buffer(out_file, in_file.gcount() - (eof ? 0 : 1));

    // seek backwards to read from line longer than the buffer
    while (buffer_start > line_start) {
        size_t read_size = std::min(buffer_start - line_start, buffer.size());
        buffer_start -= read_size;
        in_file.seekg(buffer_start);
        in_file.read(buffer.data(), read_size);
        if (in_file.bad()) {
            std::cerr << "Error: failed to read from input file: " << std::endl;
            exit(1);
        }
        reverse_write_buffer(out_file, read_size);
    }
    
    if (!eof) {
        // seek to end of line (initial read_file position)
        if (in_file.tellg() != next_buffer_start) {
            in_file.seekg(next_buffer_start);
        }

        // write newline
        out_file.write("\n", 1);
        if (out_file.fail()) {
            std::cerr << "Error: failed to write to output file: " << std::endl;
            exit(1);
        }
    }
    
    return eof;
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
    //// for lines longer than the buffer, we repeatidly read a 4096 byte chunk until we find the newline
    while (write_reverse_line(in_file, out_file) == false);
}
