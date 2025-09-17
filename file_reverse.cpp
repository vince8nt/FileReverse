// file_reverse.cpp
#include <iostream>
#include <fstream>
#include <array>
#include <algorithm>

// Fixed size buffer stored on the stack
// Better performance than std::string
std::array<char, 4096> buffer;

// Clear in_file's error flags, then read until newline, eof, or full buffer
// Return the file position cooresponding to the start of the buffer
inline size_t getline_pos(std::ifstream& in_file) {
    in_file.clear();
    size_t buffer_start = in_file.tellg();
    in_file.getline(buffer.data(), buffer.size());
    return buffer_start;
}

// Read until newline or eof
// Resulting buffer might only contain ending portion of the line (for lines longer than the buffer)
// Returns the file position cooresponding to the start of the buffer
size_t read_until_newline(std::ifstream& in_file) {
    size_t buffer_start = getline_pos(in_file);
    while (true) {
        switch (in_file.rdstate()) {
            case std::ios::badbit:
                std::cerr << "Error: failed to read from input file: " << std::endl;
                exit(1);
            case std::ios::failbit:
                buffer_start = getline_pos(in_file);
                break;
            case std::ios::eofbit:
                return buffer_start;
            default:
                return buffer_start;
        }
    }
}

// Reverse the (portion of) buffer and write to output file
void reverse_write_buffer(std::ofstream& out_file, const size_t size) {
    std::reverse(buffer.begin(), buffer.begin() + size);
    out_file.write(buffer.data(), size);
    if (out_file.fail()) {
        std::cerr << "Error: failed to write to output file: " << std::endl;
        exit(1);
    }
}

// Writes the reverse line to the output file
// in_file: the input file stream (positioned after reading the line)
// out_file: the output file stream
// line_start: the file position cooresponding to the start of the line
// buffer_start: the file position cooresponding to the start of the buffer
// Returns true if eof is reached
bool write_reverse_line(std::ifstream& in_file, std::ofstream& out_file) {
    const size_t line_start = in_file.tellg();
    size_t buffer_start = read_until_newline(in_file);
    const size_t next_line_start = in_file.tellg(); // could be garbage if eof is reached
    const bool eof = in_file.eof();
    
    // Empty line at the end of the file
    if (buffer_start == next_line_start) {
        return true;
    }

    // If eof is not reached, gcount() will count the newline (which wasn't read into the buffer)
    // This works for both Unix/Linux (newline is \n) and Windows (newline is \r\n)
    //// since \r\n is transormed into \n prior to gcount()'s calculation.
    reverse_write_buffer(out_file, in_file.gcount() - (eof ? 0 : 1));

    // Seek backwards to read from line longer than the buffer
    while (buffer_start > line_start) {
        size_t read_size = std::min(buffer_start - line_start, buffer.size());
        buffer_start -= read_size;
        in_file.clear();
        in_file.seekg(buffer_start);
        in_file.read(buffer.data(), read_size);
        if (in_file.bad()) {
            std::cerr << "Error: failed to read from input file: " << std::endl;
            exit(1);
        }
        reverse_write_buffer(out_file, read_size);
    }
    
    // Write newline and seek to initial in_file position
    if (!eof) {
        if (in_file.tellg() != next_line_start) {
            in_file.seekg(next_line_start);
        }
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

    // Read file line by line:
    //// For short lines, fstream's internal buffering will make this efficient
    //// For lines longer than the buffer, we read a 4096 byte chunk until we find the newline
    while (write_reverse_line(in_file, out_file) == false);
}
