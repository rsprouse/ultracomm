#pragma once

#include <exception>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
using namespace std;


class Ultracomm
{
public:
    struct ConnectionError : public exception
    {
      const char * what () const throw ()
        {
          return "Could not connect to Ultrasonix.";
        }
    };
    struct DataDescriptorError : public exception
    {
      const char * what () const throw ()
        {
          return "Error retrieving data descriptor.";
        }
    };
    struct DataError : public exception
    {
      const char * what () const throw ()
        {
          return "No data available.";
        }
    };
    struct ParameterMismatchError : public exception
    {
      const char * what () const throw ()
        {
          return "Parameter value does not match machine.";
        }
    };

    ulterius ult;
    int datatype;

    Ultracomm();
    Ultracomm(const string& address);
    Ultracomm(const po::variables_map& params);
    void connect(const string& addr);
    void disconnect();
    void freeze();
    void unfreeze();
    int set_uparams(const po::variables_map& params);
    int verify_uparams(const po::variables_map& params);
    int save(const string& outbase);
};

