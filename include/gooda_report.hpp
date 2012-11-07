#ifndef CONVERTER_GOODA_REPORT_HPP
#define CONVERTER_GOODA_REPORT_HPP

#include <string>
#include <vector>
#include <unordered_map>

#include <boost/algorithm/string.hpp>

typedef std::string::const_iterator string_iter;
typedef boost::iterator_range<string_iter> string_view;

//Hotspot indices
#define HS_FUNCTION_NAME 2
#define HS_UNHALTED_CORE_CYCLES 7

//asm indices
#define ASM_DISASSEMBLY 6
#define ASM_UNHALTED_CORE_CYCLES 7

//src indices
#define SRC_LINE 0
#define SRC_UNHALTED_CORE_CYCLES 2

namespace converter {

struct gooda_line {
    std::string line;
    std::vector<string_view> contents;

    std::string get_string(std::size_t index) const;
    unsigned long get_counter(std::size_t index) const;

};

struct gooda_file {
    typedef std::vector<gooda_line>::iterator iterator;
    typedef std::vector<gooda_line>::const_iterator const_iterator;

    std::vector<gooda_line> lines;

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;
};

class gooda_report {
    public:
        std::size_t functions() const;

        void add_hotspot_function(gooda_line&& line);
        const gooda_line& hotspot_function(std::size_t i) const;
        
        gooda_file& src_file(std::size_t i);
        const gooda_file& src_file(std::size_t i) const;
        
        gooda_file& asm_file(std::size_t i);
        const gooda_file& asm_file(std::size_t i) const;

        bool has_src_file(std::size_t i) const;
        bool has_asm_file(std::size_t i) const;

    private:
        gooda_file hotspot_file;
        
        std::unordered_map<std::size_t, gooda_file> src_files;
        std::unordered_map<std::size_t, gooda_file> asm_files;
};

}

#endif
