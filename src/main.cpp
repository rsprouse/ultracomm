#include "StdAfx.h"

/*
    ultracomm -- main
*/

int main(int argc, char* argv[])
{

    int exit_status;
    try {
        // Get command line and config file options.
        UltracommOptions uopt = UltracommOptions::UltracommOptions(argc, argv);
        
        // Connect to Ultrasonix and set parameters.
        Ultracomm uc = Ultracomm::Ultracomm(uopt);
        uc.wait_for_freeze();
        
        // Start acquisition, wait for user interaction, then stop.
        uc.wait_for_unfreeze();
        cout << "*** Acquiring images. Press <Enter> to stop. ***\n";
        cin.ignore();  // Wait until <Enter>.
        uc.wait_for_freeze();
        
        if (uopt.opt["acqmode"].as<string>() == "buffered") {
            // Get data from Ultrasonix and save to file.
            uc.save_data();
        }

        // We're done.
        uc.disconnect();
        exit_status = EXIT_SUCCESS;
    }
    catch(const UltracommOptions::WantsToStop& e) {   // --help or --version
        e.what();   // Doesn't do much besides avoid unused variable warning.
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

    return exit_status;
}


