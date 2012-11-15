#ifndef GOODA_GOODA_REPORT_HPP
#define GOODA_GOODA_REPORT_HPP

#include <string>
#include <vector>
#include <unordered_map>

#include <boost/algorithm/string.hpp>
#include <boost/range/iterator_range.hpp>

/*!
 * \file gooda_report.hpp
 * \brief Contains the data structures holding the Gooda report. 
 */

typedef std::string::const_iterator string_iter;
typedef boost::iterator_range<string_iter> string_view;
//typedef std::string string_view;

//Hotspot indices
#define HS_FUNCTION_NAME 2
#define HS_UNHALTED_CORE_CYCLES 7

//asm indices
#define ASM_FILE 3
#define ASM_DISASSEMBLY 6
#define ASM_UNHALTED_CORE_CYCLES 7

//src indices
#define SRC_LINE 0
#define SRC_UNHALTED_CORE_CYCLES 2

namespace gooda {

//TODO Better encapsulation of this class

/*!
 * \struct gooda_line
 * \brief A line of a Gooda Spreadsheets.
 */
class gooda_line {
    public:
        std::string get_string(std::size_t index) const;
        unsigned long get_counter(std::size_t index) const;
    
        std::string line;
        std::vector<string_view> contents;
};

/*!
 * \struct gooda_file
 * \brief The contents of a specific Gooda file. 
 */
class gooda_file {
    public:
        typedef std::vector<gooda_line>::iterator iterator;
        typedef std::vector<gooda_line>::const_iterator const_iterator;

        iterator begin();
        iterator end();

        const_iterator begin() const;
        const_iterator end() const;
        
        gooda_line& new_line();

        std::size_t size() const;
        
        gooda_line& line(std::size_t i);
        const gooda_line& line(std::size_t i) const;

    private:
        std::vector<gooda_line> lines;
};

/*!
 * \struct gooda_report 
 * \brief The contents of a whole Gooda report. 
 */
class gooda_report {
    public:
        std::size_t functions() const;
        std::size_t processes() const;

        gooda_line& new_hotspot_function();
        const gooda_line& hotspot_function(std::size_t i) const;

        gooda_line& new_process();
        const gooda_line& process(std::size_t i) const;
        
        gooda_file& src_file(std::size_t i);
        const gooda_file& src_file(std::size_t i) const;
        
        gooda_file& asm_file(std::size_t i);
        const gooda_file& asm_file(std::size_t i) const;

        bool has_src_file(std::size_t i) const;
        bool has_asm_file(std::size_t i) const;

    private:
        gooda_file hotspot_file;
        gooda_file process_file;
        
        std::unordered_map<std::size_t, gooda_file> src_files;
        std::unordered_map<std::size_t, gooda_file> asm_files;
};

}

#endif
