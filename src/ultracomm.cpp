#include "StdAfx.h"

#define BUFFERSIZE (2 * 1024 * 1024)
char gBuffer[BUFFERSIZE];
#define PROBE_ID 19   	// TODO: remove hard-coding of probe id


/*
    Construct Ultracomm object, connect to server,
    and set/check parameters.
*/
Ultracomm::Ultracomm(const UltracommOptions& myuopt)
    : uopt(myuopt),
      address(myuopt.opt["address"].as<string>()),
      datatype(myuopt.opt["datatype"].as<int>())
{
    connect();
    ult.setDataToAcquire(datatype);
    set_int_imaging_params();
    check_int_imaging_params();
}

/*
   Connect to the Ultrasonix.
*/
void Ultracomm::connect()
{
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
    if (ult.isConnected())
    {
        ult.disconnect();
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
        ult.toggleFreeze();
    }
    // Wait for server to acknowledge it has frozen.
    // TODO: this would be safer with a timeout.
    while (ult.getFreezeState() != 1) {}
    if (ult.getFreezeState() != 1)
    {
// TODO: throw correct error
        throw ConnectionError();
    }
	ult.setCompressionStatus(1);
}

/*
  Put ultrasonix into imaging state.
*/
void Ultracomm::wait_for_unfreeze()
{
    // 1 = FROZEN; 0 = IMAGING
    if (ult.getFreezeState() != 0)
    {
        ult.toggleFreeze();
    }
    // Wait for server to acknowledge it has switched to imaging.
    // TODO: this would be safer with a timeout.
    while (ult.getFreezeState() != 0) {}
    if (ult.getFreezeState() != 0)
    {
// TODO: throw correct error
        throw ConnectionError();
    }
}

/*
    Set all integer-type Ultrasonix imaging parameters, as specified on the
    command line or in the parameter file.
*/
void Ultracomm::set_int_imaging_params()
{
    po::variables_map params = uopt.opt;
    po::options_description iopts = uopt.int_params;
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
        if (params.count(optname)) {
            int val = params[optname].as<int>();
            string ultname = boost::replace_all_copy(optname, "_", " ");
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
    if (params.count("b-depth"))
    {
        int val;
        ult.getParamValue("b-depth", val);
        if (val != params["b-depth"].as<int>())
        {
            throw ParameterMismatchError();
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
        ult.getCineData((uData)datatype, idx, false, (char**)&gBuffer, BUFFERSIZE);
        outfile.write(gBuffer, framesize);
    }
    outfile.close();
}

/*
	TODO: header information is probably correct for .bpr files but probably
	not for other datatypes.
*/
void Ultracomm::write_header(ofstream& outfile, const uDataDesc& desc, const int& num_frames)
{
	int isize = sizeof(__int32);
	outfile.write(reinterpret_cast<const char *>(&(__int32)datatype), isize);
	outfile.write(reinterpret_cast<const char *>(&(__int32)num_frames), isize);
	outfile.write(reinterpret_cast<const char *>(&(__int32)desc.w), isize);
	outfile.write(reinterpret_cast<const char *>(&(__int32)desc.h), isize);
	outfile.write(reinterpret_cast<const char *>(&(__int32)desc.ss), isize);
	outfile.write(reinterpret_cast<const char *>(&(__int32)desc.roi.ulx), isize);
	outfile.write(reinterpret_cast<const char *>(&(__int32)desc.roi.uly), isize);
	outfile.write(reinterpret_cast<const char *>(&(__int32)desc.roi.urx), isize);
	outfile.write(reinterpret_cast<const char *>(&(__int32)desc.roi.ury), isize);
	outfile.write(reinterpret_cast<const char *>(&(__int32)desc.roi.brx), isize);
	outfile.write(reinterpret_cast<const char *>(&(__int32)desc.roi.bry), isize);
	outfile.write(reinterpret_cast<const char *>(&(__int32)desc.roi.blx), isize);
	outfile.write(reinterpret_cast<const char *>(&(__int32)desc.roi.bly), isize);
	int probe_id = PROBE_ID;
	outfile.write(reinterpret_cast<const char *>(&(__int32)probe_id), isize);
    int txf;
    ult.getParamValue("b-freq", txf);
	outfile.write(reinterpret_cast<const char *>(&(__int32)txf), isize);
    int sf;
    ult.getParamValue("vec-freq", sf);
		// TODO: this gives 4000000 instead of 8000000
	sf = 8000000;
	outfile.write(reinterpret_cast<const char *>(&(__int32)sf), isize);
	int dr;
	ult.getParamValue("frame rate", dr);
	outfile.write(reinterpret_cast<const char *>(&(__int32)dr), isize);
	int ld;
	ult.getParamValue("b-ldensity", ld);
	outfile.write(reinterpret_cast<const char *>(&(__int32)ld), isize);
	int extra;
	//ult.getParamValue("color-ensemble", extra);
	extra = 0;   // TODO: figure out what goes here
	outfile.write(reinterpret_cast<const char *>(&(__int32)extra), isize);
}
