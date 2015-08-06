#include "StdAfx.h"

ofstream logfile;    // Log file.
SYSTEMTIME lt;

/*
    ultracomm -- main
*/

int main(int argc, char* argv[])
{

    int exit_status;

    /* Temporary hack to swallow access violation errors at program end. */
    SetErrorMode(SetErrorMode(0) | SEM_NOGPFAULTERRORBOX | SEM_FAILCRITICALERRORS);

    const bool blocking = true;  // block until ulterius commands have succeeded
    const bool lazy_param_set = true;  // Do not set ultrasound params if they already have the correct value.
    UltracommOptions uopt; 
    int verbose;

    try {
        // Get command line and config file options.
        uopt.loadargs(argc, argv);
        verbose = uopt.opt["verbose"].as<int>();

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
        cerr << "Caught required_option error.\n";
        cerr << "Missing required option: " << e.what() << "\n";
        exit_status = MISSING_REQUIRED_OPTION_ERROR;
    }
    catch(const UltracommOptions::MissingOptionsFileError& e) {
        cerr << "Caught MissingOptionsFileError.\n";
        cerr << e.what() << "\n";
        exit_status = MISSING_OPTIONS_FILE_ERROR;
    }
    catch(const UltracommOptions::UnimplementedFeatureError& e) {
        cerr << "Caught UnimplementedFeatureError.\n";
        cerr << e.what() << "\n";
        exit_status = UNIMPLEMENTED_FEATURE_ERROR;
    }
    catch(const std::runtime_error& e) {
        cerr << "Caught runtime_error. Probably could not open logfile.\n";
        cerr << e.what() << "\n";
        exit_status = RUNTIME_ERROR;
    }
    catch(const exception& e) {
        cerr << "Exception: " << e.what() << "\n";
        GetSystemTime(&lt);
        logfile << "main: Caught generic exception " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << "." << lt.wMilliseconds << "\n";
        logfile << e.what() << "\n";
        logfile.flush();
        exit_status = UNKNOWN_ERROR;
    }
    catch(...) {
        cerr << "Unhandled exception of unknown type!\n";
        GetSystemTime(&lt);
        logfile << "main: Caught exception of unknown type " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << "." << lt.wMilliseconds << "\n";
        logfile.flush();
        exit_status = UNKNOWN_ERROR;
    }

    try {
        Ultracomm uc = Ultracomm::Ultracomm(uopt, logfile);
//        Ultracomm uc = Ultracomm::Ultracomm(uopt);
        try {
            // Connect to Ultrasonix and set parameters.
            GetSystemTime(&lt);
            logfile << "main: Connecting to ultrasonix. Localtime: " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << "." << lt.wMilliseconds << "\n";
            logfile.flush();
            uc.connect();
        }
        catch(const Ultracomm::ConnectionError& e) {
            delete uc.ult;
            cerr << "(stderr) Caught ConnectionError.\n";
            cerr << e.what() << "\n";
            GetSystemTime(&lt);
            logfile << "main: Caught ConnectionError. Localtime: " << lt.wSecond << "." << lt.wMilliseconds << "\n";
            logfile << e.what() << "\n";
            logfile.flush();
            exit_status = CONNECTION_ERROR;
            goto EXIT;
        }
        try {
            GetSystemTime(&lt);
            logfile << "main: Setting parameters. Localtime: " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << "." << lt.wMilliseconds << "\n";
            logfile.flush();
            uc.set_int_imaging_params(lazy_param_set);
            uc.check_int_imaging_params();
        }
        catch(const Ultracomm::ParameterMismatchError& e) {
            delete uc.ult;
            cerr << "Caught ParameterMismatchError.\n";
            cerr << e.what() << "\n";
            GetSystemTime(&lt);
            logfile << "main: Caught ParameterMismtachError. Localtime: " << lt.wSecond << "." << lt.wMilliseconds << "\n";
            logfile << e.what() << "\n";
            logfile.flush();
            exit_status = PARAMETER_MISMATCH_ERROR;
            goto EXIT;
        }
        uc.freeze(blocking);
        
        if (uopt.opt.count("dump-params")) {
            uc.dump_params();
        }
        else if (! uopt.opt.count("freeze-only")) {
            // Start acquisition, wait for user interaction, then stop.
//            uc.unset_data_to_acquire(blocking);
            uc.set_data_to_acquire(blocking);
            //uc.freeze(blocking);
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
    catch(const exception& e) {
        cerr << "Exception: " << e.what() << "\n";
        GetSystemTime(&lt);
        logfile << "main: Caught generic exception " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << "." << lt.wMilliseconds << "\n";
        logfile << e.what() << "\n";
        logfile.flush();
        exit_status = UNKNOWN_ERROR;
    }
    catch(...) {
        cerr << "Unhandled exception of unknown type!\n";
        GetSystemTime(&lt);
        logfile << "main: Caught exception of unknown type " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << "." << lt.wMilliseconds << "\n";
        logfile.flush();
        exit_status = UNKNOWN_ERROR;
    }

    EXIT:
    GetSystemTime(&lt);
    logfile << "main: Exiting. Localtime: " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << "." << lt.wMilliseconds << "\n";
    logfile.flush();
    return exit_status;
}

/*
int exception_filter(unsigned int code, struct _EXCEPTION_POINTERS *ep) {
    cerr << "In exception_filter().\n";
    if (code == EXCEPTION_ACCESS_VIOLATION) {
        cerr << "WARNING: Ignoring access violation.\n";
        return EXCEPTION_EXECUTE_HANDLER;
        cin.ignore();
    }
    else {
        cerr << "WARNING: Unknown exception type.\n";
        return EXCEPTION_CONTINUE_SEARCH;
        cin.ignore();
    }
}

int mymain(int argc, char* argv[])
{
//    cerr << "At start of main().\n";
    GetSystemTime(&lt);
    logfile << "main: At start of main() " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << "." << lt.wMilliseconds << "\n";
    logfile.flush();
    __try
    {
//        cerr << "At start of main() __try.\n";
        GetSystemTime(&lt);
        logfile << "main: At start of main() __try " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << "." << lt.wMilliseconds << "\n";
        logfile.flush();
        int exit_status = mymain(argc, argv);
//        cerr << "At end of main() __try.\n";
        GetSystemTime(&lt);
        logfile << "main: At end of main() __try " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << "." << lt.wMilliseconds << "\n";
        logfile.flush();
        return exit_status;
    }
    __except(exception_filter(GetExceptionCode(), GetExceptionInformation()))
    {
//        cerr << "At end of main() __except.\n";
        GetSystemTime(&lt);
        logfile << "main: At end of main() __except " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << "." << lt.wMilliseconds << "\n";
        logfile.flush();
        return -1;
    }
}
*/
