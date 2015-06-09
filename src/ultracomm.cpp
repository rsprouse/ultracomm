#include "StdAfx.h"

#define BUFFERSIZE (2 * 1024 * 1024)
char gBuffer[BUFFERSIZE];
int maxFrameIndex = 16000;  // Highest frame index in callback.
int frame_incr = 0;   // How much to increment frmnum in callback.
int lastFrame = 0;
int framesReceived = 0;
int callback_verbose = 0;

/*
   The Ultracomm object is not available in the callback, so we copy some of
   its attributes into file-local variables that the callback can access.
*/
ofstream* mystream;
int myframesize;
ofstream* myindexstream;
//ofstream mylog;
//FILE *mylog;

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
    callback_verbose = verbose;
    if (verbose) {
        cerr << "Setting data to acquire to datatype " << datatype << ".\n";
    }
//    ult.setDataToAcquire(datatype);
//    set_int_imaging_params();
//    check_int_imaging_params();
    const int compstat = uopt.opt["compression_status"].as<int>();
    if (verbose) {
        cerr << "Setting compression status to " << compstat << ".\n";
    }

    if (! ult.setCompressionStatus(compstat)) {
        cerr << "Failed to set compression status to " << compstat << ".\n";
        throw ParameterMismatchError();
    }
    if (! ult.getDataDescriptor((uData)datatype, desc))
    {
        throw DataDescriptorError();
    }
    if (! ult.isDataAvailable((uData)datatype))
    {
        throw DataError();
    }
    // TODO: framesize assumes desc.ss is always a multiple of 8, and that might not be safe.
    framesize = (desc.ss / 8) * desc.w * desc.h;
    myframesize = framesize;


    if (! uopt.opt.count("freeze-only"))
    {
        std::string outname = myuopt.opt["output"].as<string>();
        outfile.open(outname, ios::out | ios::binary);
        if (outfile.fail())
        {
            throw OutputError();
        }
        mystream = &outfile;
        std::string outindexname = outname + ".idx.txt";
        outindexfile.open(outindexname, ios::out | ios::binary);
        if (outindexfile.fail())
        {
            throw OutputError();
        }
        myindexstream = &outindexfile;
        std::string outlogname = outname + ".log.txt";
        //mylog.open(outlogname, ios::out | ios::binary);
        //if (mylog.fail())
        //{
        //    throw OutputError();
        //}
        if (acqmode == "continuous") {
            ult.setCallback(frame_callback);
            write_header(outfile, desc, 0);
        }
    }
}

/*
   Class destructor.
*/
Ultracomm::~Ultracomm()
{
/*
    mylog << "In destructor.\n";
    mylog.flush();
    mylog << "Flushing mystream.\n";
    mylog.flush();
    mystream->flush();
    //mylog << "Closing mystream.\n";
    //mylog.flush();
    //mystream->close();
    mylog << "Flushing myindexstream.\n";
    mylog.flush();
    myindexstream->flush();
    //mylog << "Closing myindexstream.\n";
    //mylog.flush();
    //myindexstream->close();
    mylog << "Flushing outfile.\n";
    mylog.flush();
    outfile.flush();
    mylog << "Closing outfile.\n";
    mylog.flush();
    outfile.close();
    mylog << "Flushing outindexfile.\n";
    mylog.flush();
    outindexfile.flush();
    mylog << "Closing outindexfile.\n";
    mylog.flush();
    outindexfile.close();

    mylog << "Closing log at end of destructor.\n";
    mylog.flush();
    mylog.close();
    //delete mylog;
*/
}

/*
   Connect to the Ultrasonix.
*/
void Ultracomm::connect()
{
    //mylog << "Connecting to ultrasonix.\n";
    //mylog.flush();
    if (verbose) {
        cerr << "Connecting to ultrasonix at address " << address << ".\n";
    }
    if (!ult.connect(address.c_str()))
    {
        throw ConnectionError();
    }
    // Stop streaming, if necessary.
    //if (ult.getStreamStatus()) {
    //    ult.stopStream();
    //}
/*
    int imgmode = ult.getActiveImagingMode();
    cout << "Imaging mode is " << imgmode << ".\n";
    bool im = ult.getInjectMode();
    cout << "Inject mode is " << im << ".\n";
    bool ss = ult.getStreamStatus();
    cout << "Stream status is " << ss << ".\n";
*/
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
    //mylog << "Disconnecting from ultrasonix.\n";
    //mylog.flush();
    if (! uopt.opt.count("freeze-only"))
    {
        write_numframes_in_header(outfile, framesReceived);
        //mylog << "Closing outfile.\n";
        //mylog.flush();
        outfile.close();
        //mylog << "Closing outindexfile.\n";
        //mylog.flush();
        outindexfile.close();
        //printf("Last frame was %d.\n", lastFrame);
        int expected = lastFrame + frame_incr + 1;
        double pct = 100.0 * framesReceived / expected;
        if (framesReceived > 0 )
        {
            printf("Acquired %d of %d frames (%0.4f percent).\n", framesReceived, expected, pct);
        }
        else
        {
            printf("No frames acquired.\n");
        }
    }
    if (ult.isConnected())
    {
        //mylog << "Disconnecting from ultrasonix.\n";
        //mylog.flush();
        if (verbose) {
            cerr << "Disconnecting from Ultrasonix.\n";
        }
        ult.disconnect();
        //mylog << "Disconnected from ultrasonix.\n";
        //mylog.flush();
    }
    else
    {
        //mylog << "Already disconnected from ultrasonix.\n";
        //mylog.flush();
        if (verbose) {
            cerr << "Already disconnected from Ultrasonix.\n";
        }
    }
    //mylog << "Disconnected.\n";
    //mylog.flush();
}

/*
  Set data to acquire from Ultrasonix to configuration value.
*/
void Ultracomm::set_data_to_acquire(const bool block)
{
    //mylog << "Setting data to acquire.\n";
    //mylog.flush();
    ult.setDataToAcquire(datatype);
    if (block)
    {
        while (ult.getDataToAcquire() != datatype)
        {
            //if (verbose) {
                cerr << "Waiting for confirmation that data acquisition has been set.\n";
           // }
        }
    }
}

/*
  Set data to acquire from Ultrasonix to 0.
*/
void Ultracomm::unset_data_to_acquire(const bool block)
{
    //mylog << "Unsetting data to acquire.\n";
    //mylog.flush();
    ult.setDataToAcquire(0);
    if (block)
    {
        while (ult.getDataToAcquire() != 0)
        {
            if (verbose) {
                cerr << "Waiting for confirmation that data acquisition has been unset.\n";
            }
        }
    }
}

/*
  Put Ultrasonix into freeze state and wait for confirmation.
*/
void Ultracomm::freeze(const bool block)
{
    //mylog << "Freezing.\n";
    //mylog.flush();
    // 1 = FROZEN; 0 = IMAGING
    if (ult.getFreezeState() != 1)
    {
        //mylog << "Freezing ultrasonix.\n";
        //mylog.flush();
        if (verbose) {
            cerr << "Freezing Ultrasonix.\n";
        }
        ult.toggleFreeze();
    }
    else
    {
        //mylog << "Ultrasonix already frozen.\n";
        //mylog.flush();
        if (verbose) {
            cerr << "Ultrasonix already frozen.\n";
        }
    }
    // Wait for server to acknowledge it has frozen.
    // TODO: this would be safer with a timeout.
    if (block)
    {
        while (ult.getFreezeState() != 1)
        {
            //mylog << "Waiting for confirmation that ultrasonix has frozen.\n";
            //mylog.flush();
//            if (verbose) {
                cerr << "Waiting for confirmation that Ultrasonix has frozen.\n";
//                cout << "cout: Waiting for confirmation that Ultrasonix has frozen.\n";
//            }
            //Sleep(100);
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
    if (
           uopt.opt["ms_delay_after_freeze"].as<int>() > 0 &&
           !uopt.opt.count("freeze-only")
    )
    {
        Sleep(uopt.opt["ms_delay_after_freeze"].as<int>());
    }
}

/*
  Put ultrasonix into imaging state.
*/
void Ultracomm::unfreeze(const bool block)
{
    //mylog << "Unfreezing.\n";
    //mylog.flush();
    // 1 = FROZEN; 0 = IMAGING
    if (ult.getFreezeState() != 0)
    {
        //mylog << "Unfreezing ultrasonix.\n";
        //mylog.flush();
        if (verbose) {
            cerr << "Unfreezing Ultrasonix.\n";
        }
        ult.toggleFreeze();
    }
    else
    {
        //mylog << "Ultrasonix already imaging.\n";
        //mylog.flush();
        if (verbose) {
            cerr << "Ultrasonix already imaging.\n";
        }
    }
    // Wait for server to acknowledge it has switched to imaging.
    // TODO: this would be safer with a timeout.
    if (block)
    {
        while (ult.getFreezeState() != 0)
        {
            //mylog << "Waiting for confirmation that Ultrasonix is imaging.\n";
            //mylog.flush();
//            if (verbose) {
                cerr << "Waiting for confirmation that Ultrasonix is imaging.\n";
//                cout << "cout: Waiting for confirmation that Ultrasonix is imaging.\n";
//            }
            //Sleep(100);
        }
    }
}

/*
    Print all ultrasonix parameters.
*/
void Ultracomm::dump_params()
{
    int i = 0;
    int val;
    uParam param;
    cout << "index\tparam.id\tvalue\tparam.name\tparam.source\tparam.type\n";
    while (ult.getParam(i++, param))
    {
        // TODO: Learn about different param types and how to get their values.
        // This probably doesn't work properly for non-integer param values.
        // Have not been able to get getParamValue() to return false either.
        if (ult.getParamValue(param.id, val))
        {
            cout << i << "\t" << param.id << "\t" << val << "\t" << param.name << "\t" << param.source << "\t" << param.type << "\t" << param.unit << "\n";
        }
        else
        {
            cerr << i << "\t'" << param.id << "'\t" << param.name << "\tFAILED\n";
        }
    }
}

/*
    Set all integer-type Ultrasonix imaging parameters, as specified on the
    command line or in the parameter file.

    If the lazy parameter is true, set each imaging parameter only when
    necessary, i.e. if the ultrasonix already has the correct value,
    do not set it.
*/
void Ultracomm::set_int_imaging_params(const bool lazy)
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
            int val, got;
            val = params[optname].as<int>();
            ult.getParamValue(ultname.c_str(), got);
            if (!lazy || got != val) {
                if (verbose) {
                    cerr << "Setting '" << ultname << "' to value " << val << ".\n";
                }
                ult.setParamValue(ultname.c_str(), val);
            }
            else {
                if (verbose) {
                    cerr << "Did not set '" << ultname << "' to value " << val << ".\n";
                }
            }
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
            ult.getParamValue(ultname.c_str(), got);
            if (verbose) {
                cerr << "Got value of '" << ultname << "'. Expected " << expected << " and got " << got << ".\n";
            }
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
    //mylog << "Saving data.\n";
    //mylog.flush();
    int num_frames = ult.getCineDataCount((uData)datatype);
    write_header(outfile, desc, num_frames);

    // TODO: figure out why buffer and sz makes program crash
    //const int sz = 2 * 1024 * 1024;
    //char buffer[BUFFERSIZE];  // TODO: determine appropriate sizes on the fly
//    int num_frames = ult.getCineDataCount((uData)datatype);

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
    //mylog << "Writing header.\n";
    //mylog.flush();
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
    TODO: kind of a hack.
*/
void Ultracomm::write_numframes_in_header(ofstream& outfile, const int& num_frames)
{
    //mylog << "Writing numframes in header.\n";
    //mylog.flush();
    const int isize = sizeof(__int32);
    outfile.seekp(isize);
    outfile.write(reinterpret_cast<const char *>(&(__int32)num_frames), isize);
    //mylog << "Wrote numframes in header.\n";
    //mylog.flush();
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
//    printf("%d\n", frmnum);
    // Check to see whether the frmnum index has wrapped past its maximum value.
    if (frmnum < lastFrame) {
        frame_incr += maxFrameIndex + 1;
    }
/*
    if (frmnum != lastFrame + 1) {
        printf("Skipped frame(s) %d - %d.\n", lastFrame + 1, frmnum - 1);
    }
*/
    lastFrame = frmnum;
    framesReceived++;
    if (callback_verbose)
    {
        cerr <<  lastFrame + frame_incr << " (frmnum: " << frmnum << ")\n";
    }

    // make sure we dont do an operation that takes longer than the acquisition frame rate
    //memcpy(gBuffer, data, sz);
    std::string frmint = std::to_string(long double(frmnum+frame_incr)) + "\n";
    myindexstream->write(frmint.c_str(), frmint.size());
    mystream->write((const char*)data, sz);

    return true;
}
