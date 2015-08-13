#pragma once

#include <exception>
#include <boost/program_options.hpp>
#include "options.h"

namespace po = boost::program_options;
using namespace std;


class Ultracomm
{
    ulterius* ult;                  // interface to Ultrasonix dll
    const UltracommOptions& uopt;  // program options
    const string address;          // IP address of Ultrasonix machine
    const string acqmode;          // acquisition mode ("continuous"|"buffered")
    const int datatype;            // type of data to acquire and save
    uDataDesc desc;                // data descriptor
    int framesize;           // size of frame image
    const int verbose;             // verbosity
    ofstream& logfile;              // log file

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
    struct NoFramesError : public exception
    {
      const char * what () const throw ()
        {
          return "No frames were acquired.";
        }
    };


    Ultracomm(const UltracommOptions& myuopt, ofstream& mylogfile);
    //Ultracomm(const UltracommOptions& myuopt);
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
    void write_header(const uDataDesc& desc, const int& num_frames);
    void write_numframes_in_header(const int& num_frames);
    static bool frame_callback(void* data, int type, int sz, bool cine, int frmnum);
    static bool frame_callback_noop(void* data, int type, int sz, bool cine, int frmnum);
    static bool frame_callback_ignore_data(void* data, int type, int sz, bool cine, int frmnum);
    static bool param_callback_noop(void* paramID, int ptX, int ptY);


};

