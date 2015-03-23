#pragma once

#include <exception>
#include <boost/program_options.hpp>
#include "options.h"

namespace po = boost::program_options;

class Ultracomm
{
    ulterius ult;                  // interface to Ultrasonix dll
    const UltracommOptions& uopt;  // program options
    const std::string address;     // IP address of Ultrasonix machine
    const std::string acqmode;     // acquisition mode ("continuous"|"buffered")
    const int datatype;            // type of data to acquire and save
    std::ofstream outfile;         // output ofstream
    std::ofstream outindexfile;    // output of indexes ofstream
    uDataDesc desc;                // data descriptor
    int framesize;           // size of frame image
    const int verbose;             // verbosity

public:
    struct ConnectionError : public std::exception
    {
      const char * what () const throw ()
        {
          return "Could not connect to Ultrasonix.";
        }
    };
    struct DataDescriptorError : public std::exception
    {
      const char * what () const throw ()
        {
          return "Error retrieving data descriptor.";
        }
    };
    struct DataError : public std::exception
    {
      const char * what () const throw ()
        {
          return "No data available.";
        }
    };
    struct ParameterMismatchError : public std::exception
    {
      const char * what () const throw ()
        {
          return "Parameter value does not match machine.";
        }
    };


    Ultracomm(const UltracommOptions& uopt);
    void connect();
    void disconnect();
    void wait_for_freeze();
    void wait_for_unfreeze();
    void dump_params();
    void set_int_imaging_params();
    void check_int_imaging_params();
    void save_data();
    void write_header(std::ofstream& outfile, const uDataDesc& desc, const int& num_frames);
    void write_numframes_in_header(std::ofstream& outfile, const int& num_frames);
    static bool frame_callback(void* data, int type, int sz, bool cine, int frmnum);

};

