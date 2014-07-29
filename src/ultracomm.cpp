#include "StdAfx.h"

#define BUFFERSIZE (2 * 1024 * 1024)
char gBuffer[BUFFERSIZE];

Ultracomm::Ultracomm()
{
}

// Construct and connect to address.
Ultracomm::Ultracomm(const string& address)
{
    connect(address);
    freeze();
}

// Construct, connect to address, and set parameters.
Ultracomm::Ultracomm(const po::variables_map& params)
{
    connect(params["address"].as<string>());
    freeze();
    set_uparams(params);
}

/*
   Connect to the Ultrasonix.

   Inputs:
     addr: IP address of Ultrasonix machine.
*/
void Ultracomm::connect(const string& addr)
{
    if (!ult.connect(addr.c_str()))
    {
        throw ConnectionError();
    }
/*
TODO: throw different errors depending on problem. Note that FD_CONNECT error
is when address is bad (123), and SOCKET_ERROR is when we couldn't connect
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

int Ultracomm::set_uparams(const po::variables_map& params)
{
    if (params.count("datatype"))
    {
        datatype = params["datatype"].as<int>();
        ult.setDataToAcquire(datatype);
    }
    if (params.count("b-depth"))
    {
        ult.setParamValue("b-depth", params["b-depth"].as<int>());
    }
    return 1;
}

int Ultracomm::verify_uparams(const po::variables_map& params)
{
    if (params.count("b-depth"))
    {
        int val;
        ult.getParamValue("b-depth", val);
        if (val != params["b-depth"].as<int>())
        {
            throw ParameterMismatchError();
        }
    }
    return 1;
}

int Ultracomm::save(const string& outbase)
{
cerr << "Intending to save.\n";
    uDataDesc desc;
    if (! ult.getDataDescriptor((uData)datatype, desc))
    {
        throw DataDescriptorError();
    }
    if (! ult.isDataAvailable((uData)datatype))
    {
        throw DataError();
    }
    outbase.append(".bpr");
//    char fname[330];  // TODO: this isn't safe
// TODO: figure out why buffer and sz makes program crash
    //const int sz = 2 * 1024 * 1024;
    //char buffer[BUFFERSIZE];  // TODO: determine appropriate sizes on the fly
    int num_frames = ult.getCineDataCount((uData)datatype);
    for (int idx = 0; idx < num_frames; idx++)
    {
/*
        char frmnumstr[80];
        sprintf(frmnumstr, ".%d", idx);
        strcpy(fname, outbase.c_str());
        strcat(fname, frmnumstr);
        strcat(fname, ".bpr");
*/
        
        ofstream outfile (outbase, ios::out | ios::binary);
        ult.getCineData((uData)datatype, idx, false, (char**)&gBuffer, BUFFERSIZE);
        outfile.write(gBuffer, BUFFERSIZE);
        outfile.close();
    }

    return 1;
}
