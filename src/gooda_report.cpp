//=======================================================================
// Copyright Baptiste Wicht 2012-2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

/*!
 * \file gooda_report.cpp
 * \brief Implementation of gooda_report. 
 */

#include "gooda_report.hpp"

std::size_t gooda::gooda_report::functions() const {
    return hotspot_file.size();
}

std::size_t gooda::gooda_report::processes() const {
    return process_file.size();
}
        
gooda::gooda_file& gooda::gooda_report::src_file(std::size_t i){
    return src_files[i];
}

gooda::gooda_file& gooda::gooda_report::asm_file(std::size_t i){
    return asm_files[i];
}

const gooda::gooda_file& gooda::gooda_report::asm_file(std::size_t i) const {
    return asm_files.at(i);
}

const gooda::gooda_file& gooda::gooda_report::src_file(std::size_t i) const {
    return src_files.at(i);
}

bool gooda::gooda_report::has_src_file(std::size_t i) const {
    return src_files.find(i) != src_files.end();
}

bool gooda::gooda_report::has_asm_file(std::size_t i) const {
    return asm_files.find(i) != asm_files.end();
}

gooda::gooda_line& gooda::gooda_report::new_process(){
    return process_file.new_line();
}

const gooda::gooda_line& gooda::gooda_report::process(std::size_t i) const {
    return process_file.line(i);
}

gooda::gooda_line& gooda::gooda_report::new_hotspot_function(){
    return hotspot_file.new_line();
}

const gooda::gooda_line& gooda::gooda_report::hotspot_function(std::size_t i) const {
    return hotspot_file.line(i);
}

gooda::gooda_file& gooda::gooda_report::get_hotspot_file(){
    return hotspot_file;
}

gooda::gooda_file& gooda::gooda_report::get_process_file(){
    return process_file;
}

const gooda::gooda_file& gooda::gooda_report::get_hotspot_file() const {
    return hotspot_file;
}

const gooda::gooda_file& gooda::gooda_report::get_process_file() const {
    return process_file;
}
