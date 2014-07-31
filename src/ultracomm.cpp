#include "StdAfx.h"

#define BUFFERSIZE (2 * 1024 * 1024)
char gBuffer[BUFFERSIZE];
#define PROBE_ID 19   	// TODO: remove hard-coding of probe id

// Construct, connect to server, and set/verify parameters.
Ultracomm::Ultracomm(const UltracommOptions& myuopt)
    : uopt(myuopt),
      address(myuopt.opt["address"].as<string>()),
      datatype(myuopt.opt["datatype"].as<int>())
{
    connect();
    ult.setDataToAcquire(datatype);
    set_uparams();
    verify_uparams();
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

void Ultracomm::disconnect()
{
    if (ult.isConnected())
    {
        ult.disconnect();
    }
}

/*
  Put ultrasonix into freeze state and wait for confirmation.
*/
void Ultracomm::freeze()
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
void Ultracomm::unfreeze()
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

void Ultracomm::set_uparams()
{
    po::variables_map params = uopt.opt;

    if (params.count("b-depth"))
    {
        ult.setParamValue("b-depth", params["b-depth"].as<int>());
    }
    // TODO: automating this is a pain because of the space in the param name
    if (params.count("trigger_out"))
    {
        ult.setParamValue("trigger out", params["trigger_out"].as<int>());
    }
    if (params.count("trigger_out_2"))
    {
        ult.setParamValue("trigger out 2", params["trigger_out_2"].as<int>());
    }
}

void Ultracomm::verify_uparams()
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

void Ultracomm::save()
{
    po::variables_map params = uopt.opt;
    const string outbase = params["output"].as<string>();
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
	string outname = outbase;
    string ext = ".bpr";
    outname += ext;
    ofstream outfile (outname, ios::out | ios::binary);

/* **** begin header info **** */
	/*
	TODO: header information should be correct for our .bpr files but probably
	is not correct for other datatypes.
	*/
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
/* **** end header info **** */

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

