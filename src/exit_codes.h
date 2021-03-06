#pragma once

// Successful program exit.
#define EXIT_SUCCESS 0

//  One or more required command line parameters are missing.
#define MISSING_REQUIRED_OPTION_ERROR 1

// An options file was identified on the command line but can't be accessed.
#define MISSING_OPTIONS_FILE_ERROR 2

// Program requested a feature that has not been implemented.
#define UNIMPLEMENTED_FEATURE_ERROR 3

// Other problem with program options.
#define BAD_OPTION_ERROR 4

// Runtime error. Probably issue with logfile.
#define RUNTIME_ERROR 100

// Could not connect to the Ultrasonix server.
#define CONNECTION_ERROR 101

// Error in setting Ultrasonix imaging parameters.
#define PARAMETER_MISMATCH_ERROR 102

// Error in logfile creation.
#define CREATE_LOGFILE_ERROR 103

// No frames of ultrasound data were acquired.
#define NO_FRAMES_ERROR 104

// Unknown.
#define UNKNOWN_ERROR 255
