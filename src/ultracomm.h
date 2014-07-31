#pragma once

#include <exception>
#include <boost/program_options.hpp>
#include "options.h"

namespace po = boost::program_options;
using namespace std;


class Ultracomm
{
    ulterius ult;                  // interface to Ultrasonix dll
    const UltracommOptions& uopt;  // program options
    const int datatype;            // type of data to acquire and save
    const string address;          // IP address of Ultrasonix machine

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


    Ultracomm(const UltracommOptions& uopt);
    void connect();
    void disconnect();
    void freeze();
    void unfreeze();
    void set_uparams();
    void verify_uparams();
    void save();
};

