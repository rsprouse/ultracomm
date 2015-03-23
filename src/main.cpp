#include "StdAfx.h"

// Port for listening socket.
#define DEFAULT_PORT "50047"

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
        
        if (uopt.opt.count("dump-params")) {
            uc.dump_params();
        }
        // Start acquisition, wait for interaction, then stop.
        else if (! uopt.opt.count("freeze-only")) {
            uc.wait_for_unfreeze();
            if (! uopt.opt.count("socket")) {
                std::cout << "*** Acquiring images. Press any key to stop. ***\n";
                _getch();  // Wait for user input.
            }
            // Block until an outside process connects to and disconnects from
            // Listener.
            else {
                try {
                    Listener listener = Listener::Listener(DEFAULT_PORT);
                    listener.do_listen();
                    listener.do_block();
                    listener.do_shutdown();
                }
                catch(const Listener::SocketBlockError& e) {
                    std::cerr << "Error while blocking on socket: ";
                    std::cerr << e.what() << "\n";
                    exit_status = SOCKET_BLOCK_ERROR;
                }
                catch(const std::exception& e) {
                    std::cerr << "Error in creating socket or accepting connection: ";
                    std::cerr << e.what() << "\n";
                    exit_status = SOCKET_INIT_ERROR;
                }
                catch(...) {
                    std::cerr << "Unhandled exception of unknown type!\n";
                    exit_status = UNKNOWN_ERROR;
                }
            }
            uc.wait_for_freeze();
            
            if (uopt.opt["acqmode"].as<std::string>() == "buffered") {
                // Get data from Ultrasonix and save to file.
                uc.save_data();
            }
        }

        // We're done.
        if (uopt.opt.count("delay-exit")) {
            std::cout << "*** ultracomm finished. Press any key to exit the program. ***\n";
            _getch();  // Wait for user input.
        }
        uc.disconnect();
        exit_status = EXIT_SUCCESS;
    }
    catch(const UltracommOptions::WantsToStop& e) {   // --help or --version
        e.what();   // Doesn't do much besides avoid unused variable warning.
        exit_status = EXIT_SUCCESS;
    }
    catch(const UltracommOptions::WantsToStopWithDelay& e) {
        e.what();   // Doesn't do much besides avoid unused variable warning.
        std::cout << "*** ultracomm finished. Press any key to exit the program. ***\n";
        _getch();  // Wait for user input.
        exit_status = EXIT_SUCCESS;
    }
    catch(const po::required_option& e) {
        std::cerr << "Missing required option: " << e.what() << "\n";
        exit_status = MISSING_REQUIRED_OPTION_ERROR;
    }
    catch(const UltracommOptions::MissingOptionsFileError& e) {
        std::cerr << e.what() << "\n";
        exit_status = MISSING_OPTIONS_FILE_ERROR;
    }
    catch(const UltracommOptions::UnimplementedFeatureError& e) {
        std::cerr << e.what() << "\n";
        exit_status = UNIMPLEMENTED_FEATURE_ERROR;
    }
    catch(const Ultracomm::ConnectionError& e) {
        std::cerr << e.what() << "\n";
        exit_status = CONNECTION_ERROR;
    }
    catch(const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        exit_status = UNKNOWN_ERROR;
    }
    catch(...) {
        std::cerr << "Unhandled exception of unknown type!\n";
        exit_status = UNKNOWN_ERROR;
    }

    return exit_status;
}


