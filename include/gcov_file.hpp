#ifndef GOODA_GCOV_FILE_HPP
#define GOODA_GCOV_FILE_HPP

#include <iostream>
#include <fstream>

#include "gcov_types.hpp"

#define GCOV_VERSION ((gcov_unsigned_t)0x3430372a)      /* 407* */
#define GCOV_DATA_MAGIC ((gcov_unsigned_t)0x67636461)   /* "gcda" */

//AFDO tag names
#define GCOV_TAG_AFDO_FILE_NAMES ((gcov_unsigned_t)0xaa000000)
#define GCOV_TAG_AFDO_FUNCTION ((gcov_unsigned_t)0xac000000)
#define GCOV_TAG_AFDO_MODULE_GROUPING ((gcov_unsigned_t)0xae000000)
#define GCOV_TAG_AFDO_WORKING_SET ((gcov_unsigned_t)0xaf000000)

namespace gooda {

class gcov_file {
    public:
        bool open(const std::string& file);

        void write_unsigned(gcov_unsigned_t value);
        void write_counter(gcov_type value);
        void write_string (const std::string& value);
        void write_header();
        void write_section_header(gcov_unsigned_t tag, unsigned int length);

    private:
        std::ofstream gcov_file;
};

}

#endif