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
    const string address;          // IP address of Ultrasonix machine
    const string acqmode;          // acquisition mode ("continuous"|"buffered")
    const int datatype;            // type of data to acquire and save
    ofstream outfile;              // output ofstream
    ofstream outindexfile;         // output of indexes ofstream
    uDataDesc desc;                // data descriptor
    int framesize;           // size of frame image
    const int verbose;             // verbosity

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
    struct OutputError : public exception
    {
      const char * what () const throw ()
        {
          return "Failed to open output file.";
        }
    };


    Ultracomm(const UltracommOptions& uopt);
    ~Ultracomm();
    void connect();
    void disconnect();
    void set_data_to_acquire(const bool block);
    void unset_data_to_acquire(const bool block);
    void freeze(const bool block);
    void unfreeze(const bool block);
    void dump_params();
    void set_int_imaging_params(const bool lazy);
    void check_int_imaging_params();
    void save_data();
    void write_header(ofstream& outfile, const uDataDesc& desc, const int& num_frames);
    void write_numframes_in_header(ofstream& outfile, const int& num_frames);
    static bool frame_callback(void* data, int type, int sz, bool cine, int frmnum);

};

