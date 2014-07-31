#include "StdAfx.h"

/*
  Command line and options file option handling for ultracomm.
*/

UltracommOptions::UltracommOptions(const int& argc, char* argv[])
    : int_params("Ultracomm integer parameters")
{
    //string appName = boost::filesystem::basename(argv[0]);

    // Command-line-only options.
    po::options_description cmdlineonly("Ultracomm command-line-only options");
    cmdlineonly.add_options()
        ("help,h", "print help message and stop")
        ("version", "print ultracomm version and stop")
        ("optfile,o", po::value<string>(), "parameter options file (see below)")
        ("verbose,v", "display informational messages")
    ;

    // Options allowed in options file or on command line.
    po::options_description global_opts("Ultracomm global options for command line or options file");
    global_opts.add_options()
        ("address,a", po::value<string>()->required(), "ultrasonix ip address")
        ("output,O", po::value<string>()->required(), "base output name (including path)")
    ;

    // Options allowed in options file or on command line.
/*
    NOTE: this one is now in the initialization list
    po::options_description int_params("Ultracomm integer parameters");
*/
    int_params.add_options()
        ("b-depth", po::value<int>(), "b-depth")
        ("datatype", po::value<int>()->required(), "datatype")
        ("trigger_out", po::value<int>(), "trigger out")
        ("trigger_out_2", po::value<int>(), "trigger out 2")
    ;

    // Now combine into full set of command line options.
    po::options_description cmdline_options;
    cmdline_options.add(cmdlineonly).add(global_opts).add(int_params);

    // And a full set of options file options.
    po::options_description cmd_or_file;
    cmd_or_file.add(global_opts).add(int_params);

    // Read command line options into opt.
    po::store(po::parse_command_line(argc, argv, cmdline_options), opt);

    // This precedes po::notify() in case of error in parameters.
    if (opt.count("help")) {
        cout << cmdline_options << "\n";
        throw WantsToStop();
    }
    if (opt.count("version")) {
        cout << "Version info not implemented.\n";
        throw WantsToStop();
    }

    // Add values from options file, if specified in command line.
    if (opt.count("optfile")) {
        if (opt.count("verbose")) {
            cout << "verbosity is " << opt.count("verbose") << ".\n";
            cout << "Using options file " << opt["optfile"].as<string>() << ".\n";
        }

        ifstream ifs(opt["optfile"].as<string>().c_str());
        if (!ifs)
        {
            throw MissingOptionsFileError();
        }
        else
        {
            po::store(parse_config_file(ifs, cmd_or_file), opt);
        }
    }

    // Will throw exception if error in parameters.
    po::notify(opt);
}

// Return true if name is among set of param names accepted by ulterius setParamValue(name, int).
bool UltracommOptions::has_int_param(const string& name) const
{
    try
    {
        int_params.find(name, false, false, false);
        return true;
    }
    catch(...)
    {
        return false;
    }
}


