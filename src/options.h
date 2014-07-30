#pragma once
namespace po = boost::program_options;

using namespace std;

struct OptionWantsToStop : public exception
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

void get_program_options(const int& argc, char* argv[], po::variables_map& vm);
