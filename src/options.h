#pragma once
namespace po = boost::program_options;

using namespace std;

class UltracommOptions
{

public:
    po::variables_map opt;
    po::options_description int_imaging_params;

    // Throw this one if program should stop when an option is encountered,
    // e.g. --help.
    struct WantsToStop : public exception
    {
      const char * what () const throw ()
        {
          return "";
        }
    };
    
    // Throw this one if program should stop when an option is encountered,
    // e.g. --help, and then delay exit until user input.
    struct WantsToStopWithDelay : public exception
    {
      const char * what () const throw ()
        {
          return "";
        }
    };
    
    struct MissingOptionsFileError : public exception
    {
      const char * what () const throw ()
        {
          return "Could not find options file.";
        }
    };
    
    struct UnimplementedFeatureError : public exception
    {
      const char * what () const throw ()
        {
          return "Option requested a feature that has not been implemented.";
        }
    };

    struct MissingRequiredOptionError : public exception
    {
      const char * what () const throw ()
        {
          return "A required option has not been specified.";
        }
    };

    UltracommOptions(const int& argc, char* argv[]);
};
