#include "StdAfx.h"

/*
    ultracomm -- main
*/

int main(int argc, char* argv[])
{

    int exit_status;
    const bool blocking = true;  // block until ulterius commands have succeeded
    const bool lazy_param_set = true;  // Do not set ultrasound params if they already have the correct value.
    ofstream logfile;    // Log file.
    SYSTEMTIME lt;

    try {
        // Get command line and config file options.
        UltracommOptions uopt = UltracommOptions::UltracommOptions(argc, argv);
        int verbose = uopt.opt["verbose"].as<int>();

        std::string outname = uopt.opt["output"].as<string>();
        std::string logname = outname + ".log.txt";
        logfile.open(logname, ios::out | ios::binary);
        if (logfile.fail())
        {
            throw std::runtime_error("Could not open logfile.");
        }
        GetSystemTime(&lt);
        logfile << "main: Connecting to ultracomm. Localtime: " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << "." << lt.wMilliseconds << "\n";
        logfile.flush();

        // Connect to Ultrasonix and set parameters.
        Ultracomm uc = Ultracomm::Ultracomm(uopt, logfile);
        GetSystemTime(&lt);
        logfile << "main: Setting parameters. Localtime: " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << "." << lt.wMilliseconds << "\n";
        logfile.flush();
        uc.set_int_imaging_params(lazy_param_set);
        uc.check_int_imaging_params();
        uc.freeze(blocking);
        
        if (uopt.opt.count("dump-params")) {
            uc.dump_params();
        }
        else if (! uopt.opt.count("freeze-only")) {
            // Start acquisition, wait for user interaction, then stop.
//            uc.unset_data_to_acquire(blocking);
            uc.set_data_to_acquire(blocking);
            uc.freeze(blocking);
            uc.unfreeze(blocking);
            if (! uopt.opt.count("named-pipe")) {
                cout << "*** Acquiring images. Press any key to stop. ***\n";
                _getch();  // Wait for user input.
            }
            else {
                if (verbose) {
                    cout << "*** Acquiring images. Waiting on named pipe. ***\n";
                }
                block_on_named_pipe();
                if (verbose) {
                    cout << "*** Read data from named pipe. ***\n";
                }
            }
            uc.freeze(blocking);
            // When unset_data_to_acquire() is not used, we often get several frames
            // of repeat data in our ulterius callback.
            uc.unset_data_to_acquire(blocking);
            if (uopt.opt["acqmode"].as<string>() == "buffered") {
                // Get data from Ultrasonix and save to file.
                uc.save_data();
            }
        }
        Sleep(200);  // allow time for callbacks to finish

        // We're done.
        if (uopt.opt.count("delay-exit")) {
            cout << "*** ultracomm finished. Press any key to exit the program. ***\n";
            _getch();  // Wait for user input.
        }
        uc.disconnect();
        GetSystemTime(&lt);
        logfile << "main: disconnected from ultracomm. Localtime: " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << "." << lt.wMilliseconds << "\n";
        logfile.flush();
        exit_status = EXIT_SUCCESS;
    }
    catch(const UltracommOptions::WantsToStop& e) {   // --help or --version
        e.what();   // Doesn't do much besides avoid unused variable warning.
        exit_status = EXIT_SUCCESS;
    }
    catch(const UltracommOptions::WantsToStopWithDelay& e) {
        e.what();   // Doesn't do much besides avoid unused variable warning.
        cout << "*** ultracomm finished. Press any key to exit the program. ***\n";
        _getch();  // Wait for user input.
        exit_status = EXIT_SUCCESS;
    }
    catch(const po::required_option& e) {
        cerr << "Missing required option: " << e.what() << "\n";
        exit_status = MISSING_REQUIRED_OPTION_ERROR;
    }
    catch(const UltracommOptions::MissingOptionsFileError& e) {
        cerr << e.what() << "\n";
        exit_status = MISSING_OPTIONS_FILE_ERROR;
    }
    catch(const UltracommOptions::UnimplementedFeatureError& e) {
        cerr << e.what() << "\n";
        exit_status = UNIMPLEMENTED_FEATURE_ERROR;
    }
    catch(const Ultracomm::ConnectionError& e) {
        cerr << e.what() << "\n";
        exit_status = CONNECTION_ERROR;
    }
    catch(const exception& e) {
        cerr << "Exception: " << e.what() << "\n";
        exit_status = UNKNOWN_ERROR;
    }
    catch(...) {
        cerr << "Unhandled exception of unknown type!\n";
        exit_status = UNKNOWN_ERROR;
    }

    GetSystemTime(&lt);
    logfile << "main: Exiting. Localtime: " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << "." << lt.wMilliseconds << "\n";
    logfile.flush();
    return exit_status;
}


