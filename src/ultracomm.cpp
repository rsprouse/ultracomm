#include "StdAfx.h"

#define BUFFERSIZE (2 * 1024 * 1024)
char gBuffer[BUFFERSIZE];
int lastFrame = -1;


/*
    Construct Ultracomm object, connect to server,
    and set/check parameters.
*/
Ultracomm::Ultracomm(const UltracommOptions& myuopt)
    : uopt(myuopt),
      address(myuopt.opt["address"].as<string>()),
      acqmode(myuopt.opt["acqmode"].as<string>()),
      datatype(myuopt.opt["datatype"].as<int>()),
      verbose(myuopt.opt["verbose"].as<int>())
{
    connect();
    if (verbose) {
        cerr << "Setting data to acquire to datatype " << datatype << ".\n";
    }
    ult.setDataToAcquire(datatype);
    set_int_imaging_params();
    check_int_imaging_params();
    const int compstat = uopt.opt["compression_status"].as<int>();
    if (verbose) {
        cerr << "Setting compression status to " << compstat << ".\n";
    }
    if (acqmode == "continuous") {
        ult.setCallback(frame_callback);
    }
    if (! ult.setCompressionStatus(compstat)) {
        cerr << "Failed to set compression status to " << compstat << ".\n";
        throw ParameterMismatchError();
    }
}

/*
   Connect to the Ultrasonix.
*/
void Ultracomm::connect()
{
    if (verbose) {
        cerr << "Connecting to ultrasonix at address " << address << ".\n";
    }
    if (!ult.connect(address.c_str()))
    {
        throw ConnectionError();
    }
/*
TODO: throw different errors depending on problem. Note that FD_CONNECT error
is when server address is bad (e.g. 123), and SOCKET_ERROR is when we couldn't connect
(address good but not in research mode). Should also test when address is good
but ultrasound machine not running):

C:\build\ultracomm\bin\Debug>ultracomm --address 123 --output asdf
monitorConnection(): FD_CONNECT had ERROR
Could not connect to Ultrasonix.

C:\build\ultracomm\bin\Debug>ultracomm --address 192.168.1.200 --output asdf
monitorConnection(): WSAEnumNetworkEvents() ret SOCKET_ERROR (10093)
CmonitorConnection(): CONNECTION_COMM lost

ould not connect to Ultrasonix.
*/
}

/*
    Disconnect from Ultrasonix.
*/
void Ultracomm::disconnect()
{
    cout << "Disconnecting.\n";
    if (acqmode == "continuous") {
        printf("Last frame was %d.\n", lastFrame);
    }
    if (ult.isConnected())
    {
        if (verbose) {
            cerr << "Disconnecting from Ultrasonix.\n";
        }
        ult.disconnect();
    }
    else
    {
        if (verbose) {
            cerr << "Already disconnected from Ultrasonix.\n";
        }
    }
}

/*
  Put Ultrasonix into freeze state and wait for confirmation.
*/
void Ultracomm::wait_for_freeze()
{
    // 1 = FROZEN; 0 = IMAGING
    if (ult.getFreezeState() != 1)
    {
        if (verbose) {
            cerr << "Freezing Ultrasonix.\n";
        }
        ult.toggleFreeze();
    }
    else
    {
        if (verbose) {
            cerr << "Ultrasonix already frozen.\n";
        }
    }
    // Wait for server to acknowledge it has frozen.
    // TODO: this would be safer with a timeout.
    while (ult.getFreezeState() != 1)
    {
        if (verbose) {
            cerr << "Waiting for confirmation that Ultrasonix has frozen.\n";
        }
    }
    /*
       FIXME: Occasionally we get the message

           sendAndWait(): WaitForSingleObject() ret WAIT_TIMEOUT 

       when retrieving cine data. When --verbose is turned on this message
       usually appears in save_data() after the call to getCineData() and
       before write(). According to the discussion at
       http://research.ultrasonix.com/viewtopic.php?f=5&t=1100&p=4245&hilit=delay#p4245
       adding a delay after toggleFreeze() can prevent this condition, so
       we add it here.

       Possibly this delay is not necessary when used with newer versions of
       the Ultrasonix library.
    */
    if (uopt.opt["ms_delay_after_freeze"].as<int>() > 0)
    {
        Sleep(uopt.opt["ms_delay_after_freeze"].as<int>());
    }
}

/*
  Put ultrasonix into imaging state.
*/
void Ultracomm::wait_for_unfreeze()
{
    // 1 = FROZEN; 0 = IMAGING
    if (ult.getFreezeState() != 0)
    {
        if (verbose) {
            cerr << "Unfreezing Ultrasonix.\n";
        }
        ult.toggleFreeze();
    }
    else
    {
        if (verbose) {
            cerr << "Ultrasonix already imaging.\n";
        }
    }
    // Wait for server to acknowledge it has switched to imaging.
    // TODO: this would be safer with a timeout.
    while (ult.getFreezeState() != 0)
    {
        if (verbose) {
            cerr << "Waiting for confirmation that Ultrasonix is imaging.\n";
        }
    }
}

/*
    Set all integer-type Ultrasonix imaging parameters, as specified on the
    command line or in the parameter file.
*/
void Ultracomm::set_int_imaging_params()
{
    po::variables_map params = uopt.opt;
    po::options_description iopts = uopt.int_imaging_params;
    for ( auto iter = iopts.options().begin(); iter != iopts.options().end(); ++iter)
    {
        /*
           Ultracomm program options do not contain spaces. Some of the
           parameters used by the ulterius setParamValue() call do contain
           spaces, and none appear to use underscore. The convention is that
           ultracomm program options containing underscore correspond to
           ulterius parameters that have the same name, but with the
           underscores replaced with spaces (' ').

           optname: the name as used in ultracomm program options (underscores)
           ultname: the name as used by ulterius setParamValue() (spaces)
        */
        string optname = (*iter)->long_name();
        string ultname = boost::replace_all_copy(optname, "_", " ");
        if (params.count(optname)) {
            int val = params[optname].as<int>();
            if (verbose) {
                cerr << "Setting '" << ultname << "' to value " << val << ".\n";
            }
            ult.setParamValue(ultname.c_str(), val);
        }
    }
}

/*
    Verify that integer-type Ultrasonix imaging parameters have value as
    specified by user.
*/
void Ultracomm::check_int_imaging_params()
{
    po::variables_map params = uopt.opt;
    po::options_description iopts = uopt.int_imaging_params;
    for ( auto iter = iopts.options().begin(); iter != iopts.options().end(); ++iter)
    {
        /*
           See comments in set_int_imaging_params() for mangling of
           parameter names.
        */
        string optname = (*iter)->long_name();
        string ultname = boost::replace_all_copy(optname, "_", " ");
        if (params.count(optname)) {
            int expected, got;
            expected = params[optname].as<int>();
            if (verbose) {
                cerr << "Getting value of '" << ultname << "'. Expecting " << expected << ".\n";
            }
            ult.getParamValue(ultname.c_str(), got);
            if (got != expected) {
                cerr << "Parameter '" << ultname << "' expected " << expected << " and got " << got << ".\n";
                throw ParameterMismatchError();
            }
        }
    }
}

/*
    Get data from Ultrasonix and save to file.
*/
void Ultracomm::save_data()
{
    po::variables_map params = uopt.opt;
    const string outname = params["output"].as<string>();
    int num_frames = ult.getCineDataCount((uData)datatype);
    uDataDesc desc;
    if (! ult.getDataDescriptor((uData)datatype, desc))
    {
        throw DataDescriptorError();
    }
    if (! ult.isDataAvailable((uData)datatype))
    {
        throw DataError();
    }
    ofstream outfile (outname, ios::out | ios::binary);
    write_header(outfile, desc, num_frames);

    // TODO: figure out why buffer and sz makes program crash
    //const int sz = 2 * 1024 * 1024;
    //char buffer[BUFFERSIZE];  // TODO: determine appropriate sizes on the fly
//    int num_frames = ult.getCineDataCount((uData)datatype);

    // TODO: framesize assumes desc.ss is always a multiple of 8, and that might not be safe.
    int framesize = (desc.ss / 8) * desc.w * desc.h;
    for (int idx = 0; idx < num_frames; idx++)
    {
        if (verbose) {
            cerr << "Getting cine data for frame " << idx << ".\n";
        }
        ult.getCineData((uData)datatype, idx, false, (char**)&gBuffer, BUFFERSIZE);
        outfile.write(gBuffer, framesize);
        if (verbose) {
            cerr << "Wrote cine data for frame " << idx << ".\n";
        }
    }
    outfile.close();
}

/*
    TODO: header information is probably correct for .bpr files but possibly
    not for other datatypes.
*/
void Ultracomm::write_header(ofstream& outfile, const uDataDesc& desc, const int& num_frames)
{
    if (verbose) {
        cerr << "Writing header.\n";
    }
    // Integer fields that we can write directly. Luckily these all are
    // consecutive fields in the header.
    const int isize = sizeof(__int32);
    static const int fields[] = {
        datatype,
        num_frames,
        desc.w,
        desc.h,
        desc.ss,
        desc.roi.ulx,
        desc.roi.uly,
        desc.roi.urx,
        desc.roi.ury,
        desc.roi.brx,
        desc.roi.bry,
        desc.roi.blx,
        desc.roi.bly,
        uopt.opt["probe-id"].as<int>()
    };
    for (int i = 0; i < sizeof(fields) / sizeof(fields[0]); ++i) {
        outfile.write(reinterpret_cast<const char *>(&(__int32)fields[i]), isize);
    }

    // Integer fields that we have to query from Ultrasonix. These are also
    // consecutive in the header.
    // FIXME: Determine if these are the correct params to put in the header.
    static const string queries[] = {
        "b-freq",
        "vec-freq",
        "frame rate",
        "b-ldensity"
    };
    for (int i = 0; i < sizeof(queries) / sizeof(queries[0]); ++i) {
        int val;
        ult.getParamValue(queries[i].c_str(), val);
        outfile.write(reinterpret_cast<const char *>(&(__int32)val), isize);
    }
    // FIXME: Figure out how to determine the value of the 'extra' field.
    // It will probably be added to queries[], but I don't know the param name.
    // For now we'll hard code it with value 0.
    int extra = 0;
    outfile.write(reinterpret_cast<const char *>(&(__int32)extra), isize);
}

/*
    Callback.
*/
bool Ultracomm::frame_callback(void* data, int type, int sz, bool cine, int frmnum)
{
/*
    if (verbose) {
        cerr << "In callback.\n";
    }

    if (!data || !sz)
    {
// TODO: proper error
        printf("Error: no actual frame data received\n");
        return false;
    }

    if (BUFFERSIZE < sz)
    {
// TODO: proper error
        printf("Error: frame too large for current buffer\n");
        return false;
    }
*/

    //printf("[Rx] type:(%d) size:(%d) cine:(%d) gBuffer:(%d) frame:(%d)\n", type, sz, cine, &gBuffer, frmnum);
    if (frmnum != lastFrame + 1) {
        printf("Skipped frame(s) %d - %d.\n", lastFrame + 1, frmnum - 1);
    }
    lastFrame = frmnum;
    // make sure we dont do an operation that takes longer than the acquisition frame rate
    memcpy(gBuffer, data, sz);

    return true;
}
