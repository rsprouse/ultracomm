#include "StdAfx.h"

int main(int argc, char* argv[])
{

    //string appName = boost::filesystem::basename(argv[0]);

    try {
        // Get command line and config file options.
        UltracommOptions uopt = UltracommOptions::UltracommOptions(argc, argv);
        
        // Connect to Ultrasonix and set parameters.
        Ultracomm uc = Ultracomm::Ultracomm(uopt);
        uc.freeze();
        
        // Start acquisition, wait for user interaction, then stop.
        cout << "Acquiring images. Press <Enter> to stop.\n";
        uc.unfreeze();
        cin.ignore();  // Wait until <Enter>.
        uc.freeze();
        
        // Get data from Ultrasonix and save locally.
        uc.save();

        // We're done.
        uc.disconnect();
    }
    catch(const UltracommOptions::WantsToStop& e) {   // --help or --version
        cerr << e.what() << "\n";
        return 0;
    }
    catch(const po::required_option& e) {
        cerr << "Missing required option: " << e.what() << "\n";
        return MISSING_REQUIRED_OPTION_ERROR;
    }
    catch(const UltracommOptions::MissingOptionsFileError& e) {
        cerr << e.what() << "\n";
        return MISSING_OPTIONS_FILE_ERROR;
    }
    catch(const Ultracomm::ConnectionError& e) {
        cerr << e.what() << "\n";
        return CONNECTION_ERROR;
    }
    catch(const exception& e) {
        cerr << "Exception: " << e.what() << "\n";
        return UNKNOWN_ERROR;
    }
    catch(...) {
        cerr << "Unhandled exception of unknown type!\n";
        return UNKNOWN_ERROR;
    }

    return 0;
}


