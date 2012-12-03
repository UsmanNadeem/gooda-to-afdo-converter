#include <iostream>
#include <chrono>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include "utils.hpp"
#include "gooda_reader.hpp"
#include "converter.hpp"
#include "afdo_generator.hpp"
#include "afdo_printer.hpp"
#include "logger.hpp"
    
namespace po = boost::program_options;

namespace {

//Chrono typedefs
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;

void process(const std::string& directory, po::variables_map& vm){
    Clock::time_point t0 = Clock::now();

    //Read the Gooda Spreadsheets
    auto report = gooda::read_spreadsheets(directory);

    gooda::afdo_data data;

    //Convert the Gooda report to AFDO
    gooda::read_report(report, data, vm);

    //Execute the specified action
    if(vm.count("dump")){
        gooda::dump_afdo_light(data);
    } else if(vm.count("full-dump")){
        gooda::dump_afdo(data);
    } else {
        gooda::generate_afdo(data, vm["output"].as<std::string>(), vm);
    }
    
    Clock::time_point t1 = Clock::now();
    milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);

    log::emit<log::Debug>() << "Conversion took " << ms.count() << "ms" << log::endl;
}

} //end of anonymous namespace

int main(int argc, char **argv){
    po::options_description description("converter [options] spreadsheets_directory");

    po::variables_map vm;
    po::parsed_options parsed_options(&description);
    
    try {
        description.add_options()
            ("help,h", "Display this help message")
            
            ("afdo", "Generate an AFDO profile file (default if --profile is not selected)")
            ("dump", "Dump the AFDO on standard output")
            ("full-dump", "Dump the complete AFDO information on standard output")
            ("output,o", po::value<std::string>()->default_value("fbdata.afdo"), "The name of the generated AFDO file")
            ("cache-misses", "Indicate that the cache misses information must be filled in the AFDO file")

            ("log", po::value<int>()->default_value(0), "Define the logging verbosity (0: No logging, 1: warnings, 2:debug)")

            ("profile,p", "Profile the given application")
            ("gooda", po::value<std::string>(), "Set the path to the Gooda installation. If not filled, use $GOODA_DIR or the current directory")
            ("lbr", "Performs precise profile with LBR")

            ("input-file", po::value<std::vector<std::string>>(), "Directory containing the spreadsheets");

        po::positional_options_description p;
        p.add("input-file", -1);

        parsed_options = po::command_line_parser(argc, argv).options(description).positional(p).allow_unregistered().run();
        po::store(parsed_options, vm);

        if(vm.count("help")){
            std::cout << description;
            return 0;
        }
    } catch (std::exception& e ) {
        log::emit<log::Error>() << e.what() << log::endl;
        return 1;
    }

    log::set_level(vm["log"].as<int>());

    //Profiling mode
    if(vm.count("profile")){
        int processor_model = -1;

        if(vm.count("model")){
            processor_model = vm["model"].as<int>();
        } else {
            processor_model = gooda::processor_model();
            
            if(processor_model == -1){
                log::emit<log::Error>() << "Cannot find your processor model. Please provide it with the --model option. " << log::endl;
                return -1;
            }
        }

        std::string script;

        if(processor_model == 0x2A || processor_model == 0x2D){
            log::emit<log::Debug>() << "Detected processor as \"Sandy Bridge\"" << log::endl;
            script = "run_record_cyc_snb.sh";
        } else if(processor_model == 0x3A){
            log::emit<log::Debug>() << "Detected processor as \"Ivy Bridge\"" << log::endl;
            script = "run_record_cyc_ivb.sh";
        } else if(processor_model == 0x25 || processor_model == 0x2C || processor_model == 0x2F){
            log::emit<log::Debug>() << "Detected processor as \"Westmere\"" << log::endl;
            script = "run_record_cyc_wsm_ep.sh";
        } else {
            std::cerr << "Sorry, your processor is not supported by Gooda" << std::endl;
            std::cerr << "Only Westmere, Sandy Bridge and Ivy Bridge are currently supported" << std::endl;
            return -1;
        }

        //There is only one script for LBR
        if(vm.count("lbr")){
            script = "gooda_bb_exec.sh";
        }

        //Try to find the Gooda directory
        std::string gooda_dir;
        const char* gooda_dir_val = ::getenv("GOODA_DIR");
        if(!gooda_dir_val){
            gooda_dir = "";
        } else {
            gooda_dir = gooda_dir_val;
        }

        std::string profile_command;
        if(vm.count("gooda")){
            profile_command = "sudo bash " + vm["gooda"].as<std::string>() + "/scripts/" + script + " ";
        } else if(gooda_dir.empty()){
            profile_command = "sudo bash scripts/" + script + " ";
        } else {
            profile_command = "sudo bash " + gooda_dir + "/scripts/" + script + " ";
        }

        auto further_options = po::collect_unrecognized(parsed_options.options, po::include_positional);

        //Append the application
        for(auto& option : further_options){
            profile_command += option + " ";
        }

        log::emit<log::Debug>() << "Profile the given application (perf needs to be run in root)" << log::endl;
        gooda::exec_command(profile_command);

        std::string gooda_command;
        if(vm.count("gooda")){
            gooda_command = "sudo " + vm["gooda"].as<std::string>() + "/gooda";
        } else if(gooda_dir.empty()){
            gooda_command = "sudo ./gooda";
        } else {
            gooda_command = "sudo " + gooda_dir + "/gooda";
        }

        log::emit<log::Debug>() << "Run Gooda (Gooda needs to be run in root)" << log::endl;
        gooda::exec_command(gooda_command);

        //If no option is specified, just as as a wrapper of Gooda
        if(vm.count("afdo")){
            //Process the spreadsheets to generate AFDO
            process("spreadsheets", vm); 
        }

        return 0;
    }
    
    //No further options are allowed if not in profile mode
    auto further_options = po::collect_unrecognized(parsed_options.options, po::exclude_positional);
    if(!further_options.empty()){
        std::cerr << "Error: Unrecognized options " << further_options[0];

        for(std::size_t i = 1; i < further_options.size(); ++i){
            std::cerr << ", " << further_options[i];
        }

        std::cerr << std::endl;

        return 1;
    }

    try {
        //Must be done after the test for help/profile to handle required arguments
        po::notify(vm);
    } catch (std::exception& e ) {
        log::emit<log::Error>() << e.what() << log::endl;
        return 1;
    }

    if(!vm.count("input-file")){
        log::emit<log::Error>() << "Error: No spreadsheets directory provided" << log::endl;

        return 1;
    }

    auto input_files = vm["input-file"].as<std::vector<std::string>>();

    //Test that there is a least one file
    if(input_files.empty()){
        log::emit<log::Error>() << "No spreadsheets directory provided" << log::endl;

        return 1;
    }

    //Verify that only one directory is provided
    if(input_files.size() > 1){
        log::emit<log::Error>() << "Only one directory can be analyzed at a time" << log::endl;

        return 1;
    }

    std::string directory = input_files[0];

    //The file must exists
    if(!gooda::exists(directory)){
        log::emit<log::Error>() << "\"" << directory << "\" does not exists" << log::endl;
        return 1;
    }

    //The file must be a directory
    if(!gooda::is_directory(directory)){
        log::emit<log::Error>() << "\"" << directory << "\" is not a directory" << log::endl;
        return 1;
    }

    process(directory, vm);

    return 0;
}
