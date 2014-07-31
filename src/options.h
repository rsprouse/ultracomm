#pragma once
namespace po = boost::program_options;

using namespace std;

class UltracommOptions
{

public:
    po::variables_map opt;

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
    
    UltracommOptions(const int& argc, char* argv[]);
    //void get_program_options(const int& argc, char* argv[]);
};
