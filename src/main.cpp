#include "StdAfx.h"

int main(int argc, char* argv[])
{

    po::variables_map opt;  // Command line and options file options.

    try {
      get_cmdline_options(argc, argv, opt);
      Ultracomm uc = Ultracomm::Ultracomm(opt);
      uc.verify_uparams(opt);
      uc.freeze();
      cout << "Acquiring images. Press <Enter> to stop.\n";
      uc.unfreeze();
      cin.ignore();  // Wait until <Enter>.
      uc.freeze();
      cout << "From main(), going to save().\n";
      uc.save(opt["output"].as<string>());
      uc.disconnect();
    }
    catch(const OptionWantsToStop& e) {   // --help or --version
      cerr << e.what() << "\n";
      return 0;
    }
    catch(const po::required_option& e) {
      cerr << "Missing required option: " << e.what() << "\n";
      return MISSING_REQUIRED_OPTION_ERROR;
    }
    catch(const MissingOptionsFileError& e) {
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


