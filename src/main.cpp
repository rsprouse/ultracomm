#include "StdAfx.h"

ofstream logfile;    // Log file.
SYSTEMTIME lt;

struct CreateLogfileError : public exception
{
  const char * what () const throw ()
    {
      return "Could not create logfile.";
    }
};
 
/*
    ultracomm -- main
*/

int main(int argc, char* argv[])
{

    int exit_status;

    const bool blocking = true;  // block until ulterius commands have succeeded
    const bool lazy_param_set = true;  // Do not set ultrasound params if they already have the correct value.
    int verbose;

    try {
        UltracommOptions uopt = UltracommOptions::UltracommOptions(argc, argv);
        verbose = uopt.opt["verbose"].as<int>();

        std::string outname = uopt.opt["output"].as<string>();
/* TODO: fix log handling for freeze-only */
        if (outname == "")
        {
            outname = "U:\\freeze";
        }
        std::string logname = outname + ".log.txt";
        logfile.open(logname, ios::out | ios::binary);
        if (logfile.fail())
        {
            throw CreateLogfileError();
        }
        GetSystemTime(&lt);
        logfile << "main: Connecting to ultracomm. Localtime: " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << "." << lt.wMilliseconds << "\n";
        logfile.flush();

        /* Temporary hack to swallow access violation errors at program end. */
        if (uopt.opt.count("error-hack"))
        {
            SetErrorMode(SetErrorMode(0) | SEM_NOGPFAULTERRORBOX | SEM_FAILCRITICALERRORS);
            GetSystemTime(&lt);
            logfile << "main: Using SetErrorMode to capture access violations: " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << "." << lt.wMilliseconds << "\n";
            logfile.flush();
        }

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
            uc.set_callback("data");
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
            uc.set_callback("no-op");
            uc.freeze(blocking);
            // When unset_data_to_acquire() is not used, we often get several frames
            // of repeat data in our ulterius callback.
            if (uopt.opt["acqmode"].as<string>() == "buffered") {
                // Get data from Ultrasonix and save to file.
                uc.save_data();
            }
        }
// TODO: determine if this delay is necessary or helpful.
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
        goto EXIT;
    }
    catch(const UltracommOptions::WantsToStopWithDelay& e) {
        e.what();   // Doesn't do much besides avoid unused variable warning.
        cout << "*** ultracomm finished. Press any key to exit the program. ***\n";
        _getch();  // Wait for user input.
        exit_status = EXIT_SUCCESS;
        goto EXIT;
    }
    catch(const po::required_option& e) { // e.g. missing --address
        cerr << "Missing required option: " << e.what() << "\n";
        exit_status = MISSING_REQUIRED_OPTION_ERROR;
        goto EXIT;
    }
    catch(const po::error& e) {  // e.g. an incomplete argument or command line syntax error
        cerr << "Error in program options: " << e.what() << "\n";
        exit_status = BAD_OPTION_ERROR;
        goto EXIT;
    }
    catch(const UltracommOptions::MissingOptionsFileError& e) {
        cerr << "Caught MissingOptionsFileError.\n";
        cerr << e.what() << "\n";
        exit_status = MISSING_OPTIONS_FILE_ERROR;
        goto EXIT;
    }
    catch(const UltracommOptions::UnimplementedFeatureError& e) {
        cerr << "Caught UnimplementedFeatureError.\n";
        cerr << e.what() << "\n";
        exit_status = UNIMPLEMENTED_FEATURE_ERROR;
        goto EXIT;
    }
    catch(const CreateLogfileError& e) {
        cerr << "Caught CreateLogfileError.\n";
        cerr << e.what() << "\n";
        exit_status = CREATE_LOGFILE_ERROR;
        goto EXIT;
    }
    catch(const Ultracomm::NoFramesError& e) {
        cerr << "Caught NoFramesError.\n";
        cerr << e.what() << "\n";
        exit_status = NO_FRAMES_ERROR;
        goto EXIT;
    }
    catch(const exception& e) {
        cerr << "Exception: " << e.what() << "\n";
        GetSystemTime(&lt);
        logfile << "main: Caught generic exception " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << "." << lt.wMilliseconds << "\n";
        logfile << e.what() << "\n";
        logfile.flush();
        exit_status = UNKNOWN_ERROR;
        goto EXIT;
    }
    catch(...) {
        cerr << "Unhandled exception of unknown type!\n";
        GetSystemTime(&lt);
        logfile << "main: Caught exception of unknown type " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << "." << lt.wMilliseconds << "\n";
        logfile.flush();
        exit_status = UNKNOWN_ERROR;
        goto EXIT;
    }

    EXIT:
    GetSystemTime(&lt);
    logfile << "main: Exiting with returncode " << exit_status << ". Localtime: " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << "." << lt.wMilliseconds << "\n";
    logfile.flush();
    return exit_status;
}

