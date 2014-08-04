#pragma once
namespace po = boost::program_options;

using namespace std;

class UltracommOptions
{

public:
    po::variables_map opt;
    po::options_description int_params;

    // Throw this one if program should stop when an option is encountered,
    // e.g. --help.
    struct WantsToStop : public exception
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

    UltracommOptions(const int& argc, char* argv[]);
    bool has_int_param(const string& name) const;
};
