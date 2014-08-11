#include "StdAfx.h"

/*
  Command line and options file option handling for ultracomm.
*/

UltracommOptions::UltracommOptions(const int& argc, char* argv[])
    : int_imaging_params("Ultracomm integer parameters")
{
    //string appName = boost::filesystem::basename(argv[0]);

    // Command-line-only options.
    po::options_description cmdlineonly("Ultracomm command-line-only options");
    cmdlineonly.add_options()
        ("help,h", "print help message and stop")
        ("version", "print ultracomm version and stop")
        ("sdkversion", "print Ultrasonix SDK version used to compile ultracomm and stop")
        ("params,p", po::value<string>(), "parameter options file (see below)")
        ("verbose,v", po::value<int>()->default_value(0), "display informational messages")
    ;

    // Options allowed in options file or on command line.
    po::options_description global_opts("Ultracomm global options for command line or options file");
    global_opts.add_options()
        ("address,a", po::value<string>()->required(), "ultrasonix ip address")
        ("output,o", po::value<string>()->required(), "output filename")
        ("datatype", po::value<int>()->required(), "datatype")
        ("probe-id", po::value<int>(), "probe-id")
    ;

    // Options allowed in options file or on command line.
/*
    NOTE: this one is now in the initialization list
    po::options_description int_imaging_params("Ultracomm integer imaging parameters");
*/
    int_imaging_params.add_options()
        ("b-depth", po::value<int>(), "b-depth")
        ("trigger_out", po::value<int>(), "trigger out")
        ("trigger_out_2", po::value<int>(), "trigger out 2")
        ("sector", po::value<int>(), "sector")
        ("b-ldensity", po::value<int>(), "b-ldensity")
        ("b-focus_count", po::value<int>(), "b-focus_count")
        ("b-freq", po::value<int>(), "b-freq")
        ("b-sampl_freq", po::value<int>(), "b-sampl_freq")
        ("b-persistence", po::value<int>(), "b-persistence")
        ("soundvelocity", po::value<int>(), "soundvelocity")
        ("focus_depth", po::value<int>(), "focus_depth")
        ("image_flip", po::value<int>(), "image_flip")
        ("frame_rate", po::value<int>(), "frame_rate")
        ("max_frame", po::value<int>(), "max_frame")
    ;

    // Now combine into full set of command line options.
    po::options_description cmdline_options;
    cmdline_options.add(cmdlineonly).add(global_opts).add(int_imaging_params);

    // And a full set of options file options.
    po::options_description cmd_or_file;
    cmd_or_file.add(global_opts).add(int_imaging_params);

    // Read command line options into opt.
    po::store(po::parse_command_line(argc, argv, cmdline_options), opt);

    // This precedes po::notify() in case of error in parameters.
    if (opt.count("help")) {
        cout << cmdline_options << "\n";
        throw WantsToStop();
    }
    if (opt.count("version")) {
        cout << ULTRACOMM_VERSION;
        throw WantsToStop();
    }
    if (opt.count("sdkversion")) {
        cout << ULTERIUS_SDK_VERSION;
        throw WantsToStop();
    }

    // Add values from options file, if specified in command line.
    if (opt.count("params")) {
        if (opt["verbose"].as<int>() > 0) {
            cerr << "verbosity is " << opt["verbose"].as<int>() << ".\n";
            cerr << "Using params file " << opt["params"].as<string>() << ".\n";
        }

        ifstream ifs(opt["params"].as<string>().c_str());
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

    // Check that we are working within the limitations of the current 
    // program implementation.
    const string ext = ".bpr";
    if (! boost::algorithm::ends_with(opt["output"].as<string>(), ext))
    {
        cerr << "Only .bpr output is supported.\n";
        throw UnimplementedFeatureError();
    }
    if (opt["datatype"].as<int>() != 2)
    {
        cerr << "Only datatype 2 is supported.\n";
        throw UnimplementedFeatureError();
    }
}
