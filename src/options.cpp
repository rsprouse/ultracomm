#include "StdAfx.h"

/*
  Command line and options file option handling for ultracomm.
*/

/*
  Get option values from command line and options file.
*/
void get_cmdline_options(const int& argc, char* argv[], po::variables_map& vm)
{
    //string appName = boost::filesystem::basename(argv[0]);

    // Command-line-only options.
    po::options_description cmdonly("Ultracomm command-line-only options");
    cmdonly.add_options()
        ("help,h", "print help message and stop")
        ("version", "print ultracomm version and stop")
        ("optfile,o", po::value<string>(), "parameter options file (see below)")
    ;

    // Options allowed in options file or on command line.
    po::options_description cmd_or_file("Ultracomm options for command line or options file");
    cmd_or_file.add_options()
        ("address,a", po::value<string>()->required(), "ultrasonix ip address")
        ("output,O", po::value<string>()->required(), "base output name (including path)")
        ("b-depth", po::value<int>(), "b-depth")
        ("datatype", po::value<int>(), "datatype")
        ("trigger_out", po::value<int>(), "trigger out")
        ("trigger_out_2", po::value<int>(), "trigger out 2")
        ("verbose,v", "display informational messages")
    ;

    // Now combine into full set of command line options.
    po::options_description cmdline_options;
    cmdline_options.add(cmdonly).add(cmd_or_file);

    // Read command line options into vm.
    po::store(po::parse_command_line(argc, argv, cmdline_options), vm);

    // This precedes po::notify() in case of error in parameters.
    if (vm.count("help")) {
        cout << cmdline_options << "\n";
        throw OptionWantsToStop();
    }
    if (vm.count("version")) {
        cout << "Version info not implemented.\n";
        throw OptionWantsToStop();
    }

    // Add cmd_or_file file options.
    if (vm.count("optfile")) {
        if (vm.count("verbose")) {
            cout << "verbosity is " << vm.count("verbose") << ".\n";
            cout << "Using options file " << vm["optfile"].as<string>() << ".\n";
        }

        ifstream ifs(vm["optfile"].as<string>().c_str());
        if (!ifs)
        {
            throw MissingOptionsFileError();
        }
        else
        {
            po::store(parse_config_file(ifs, cmd_or_file), vm);
        }
    }

    // Will throw exception if error in parameters.
    po::notify(vm);
}
