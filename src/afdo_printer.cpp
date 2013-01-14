//=======================================================================
// Copyright Baptiste Wicht 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

#include <iostream>
#include <sstream>

#include "afdo_printer.hpp"

std::string pretty_size(unsigned int size){
    std::stringstream stream;
    std::string unit = "B";

    if(size > 1024){
       size /= 1024; 
       unit = "KB";
    }
    
    if(size > 1024){
       size /= 1024; 
       unit = "MB";
    }

    stream << size << unit;
    return stream.str();
}

void gooda::dump_afdo(const afdo_data& data, boost::program_options::variables_map& vm){
    std::cout << "The AFDO data contains " << data.functions.size() << " hotspot functions" << std::endl;

    std::cout << "Strings" << std::endl;
    for(std::size_t i = 0; i < data.file_names.size(); ++i){
        std::cout << "   " << i << ":" << data.file_names[i] << std::endl;
    }

    auto functions = data.functions;

    std::sort(functions.begin(), functions.end(), [](const gooda::afdo_function& lhs, const gooda::afdo_function& rhs){return lhs.total_count > rhs.total_count; });

    std::cout << "Hotspot functions" << std::endl;
    for(auto& function : functions) {
        std::cout << function.name << " (" << function.file << "(" << data.get_file_index(function.file) << "))" 
            << " [" << function.total_count << ":" << function.entry_count << "]" << std::endl;

        auto stacks = function.stacks;

        std::sort(stacks.begin(), stacks.end(), [](const gooda::afdo_stack& lhs, const gooda::afdo_stack& rhs){return lhs.stack.front().line < rhs.stack.front().line; });

        for(auto& stack : stacks){
            std::cout << "   Stack of size " << stack.stack.size() 
                << ", with " << stack.num_inst << " dynamic instructions " 
                << "[count=" << stack.count;
            
            if(vm.count("cache-misses")){
                std::cout << ", cache-misses=" << stack.cache_misses;
            }
            
            std::cout << "]" << std::endl;

            for(auto& pos : stack.stack){
                std::cout << "      Instruction at " 
                    << pos.file << "(" << data.get_file_index(pos.file) << "):" << pos.line 
                    << ", func=" << pos.func << "(" << data.get_file_index(pos.func) << ")" 
                    << ", discr=" << pos.discr << std::endl;
            }
        }
    }

    if(!vm.count("nows")){
        std::cout << "Working Set" << std::endl;
        for(std::size_t i = 0; i < data.working_set.size(); ++i){
            std::cout << "   " << i << ": min= " << data.working_set[i].min_counter << " num= " << data.working_set[i].num_counter << std::endl;
        }
    }

    std::cout << "Length" << std::endl;
    std::cout << "   File Name Table: " << pretty_size(data.length_file_section * 4) << std::endl;
    std::cout << "   Function Table: " << pretty_size(data.length_function_section * 4) << std::endl;
    std::cout << "   Modules Table: " << pretty_size(data.length_modules_section * 4) << std::endl;
    std::cout << "   Working Set Table: " << pretty_size(data.length_working_set_section * 4) << std::endl;
}

void gooda::dump_afdo_light(const afdo_data& data, boost::program_options::variables_map& /*vm*/){
    std::cout << "The AFDO data contains " << data.functions.size() << " hotspot functions" << std::endl;

    auto functions = data.functions;
   
    std::sort(functions.begin(), functions.end(), [](const gooda::afdo_function& lhs, const gooda::afdo_function& rhs){return lhs.total_count > rhs.total_count; });

    for(auto& function : functions) {
        std::cout << "   " << function.name << " (" << function.file << ")" << " [" << function.total_count << ":" << function.entry_count << "]" << std::endl;
    }
}
