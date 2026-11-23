// Original file: Contrib/Contrib/DemonWare/bdNet/bdUPnP/bdUPnPDevice.cpp

#include "bdUPnPDevice.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"
#include <stdlib.h>
#include "../../bdCore/bdContainers/bdSingleton.h"
#include "../../bdCore/bdUtilities/bdTrulyRandom.h"
// File-local constants of the original bdUPnPCommon.h (not in src; DWARF bdUPnPCommon.h:55,78,79,103,104), contents read from the binary.
static const bdNChar8 s_getRequest[] = "GET %s HTTP/1.1\r\nConnection: close\r\nHost: %s\r\n\r\n";
static const bdNChar8 s_removePortMappingCommand[] = "DeletePortMapping";
static const bdNChar8 s_removePortMappingArguments[] = "<NewRemoteHost></NewRemoteHost>\r\n<NewExternalPort>%d</NewExternalPort>\r\n<NewProtocol>UDP</NewProtocol>\r\n";
static const bdNChar8 s_getExternalAddressCommand[] = "GetExternalIPAddress";
static const bdNChar8 s_getExternalAddressArguments[] = "\r\n";



bdUPnPDevice::bdUPnPDevice()
    : m_state(BD_UPNP_DEVICE_UNINITIALISED),
      m_shutdownRequested(false),
      m_portStatus(BD_UPNP_PORT_STATUS_UNKNOWN),
      m_localAddrs(NULL),
      m_isIp(true),
      m_gamePort(0), m_externalPort(0),

      m_requestSize(0),
      m_bytesReceived(0),
      m_mappingAttempts(0)
{
}

bdUPnPDevice::~bdUPnPDevice()
{
}

bdBool bdUPnPDevice::init(const bdUPnPConfig config,
                          bdArray<bdInetAddr> * localAddrs,
                          const bdAddr deviceAddress,
                          const bdNChar8 *const fetchLocation,
                          const bdUInt fetchLocationLength,
                          const bdPort port)
{
    bdBool ok = false;

    if (m_state == BD_UPNP_DEVICE_UNINITIALISED)
    {
        m_deviceAddr = deviceAddress;
        m_gamePort = port;
        m_externalPort = port;
        m_config = config;
        m_localAddrs = localAddrs;
        bdSnprintf(m_fetchLocation, fetchLocationLength + 1, fetchLocation);

        m_state = BD_UPNP_DEVICE_INITIALISED;
        m_portStatus = BD_UPNP_PORT_STATUS_UNKNOWN;
        m_shutdownRequested = false;
        if (m_streamSocket.isConnected())
        {
            m_streamSocket.close();
        }
        ok = true;
    }
    else
    {
        bdLogWarn("upnp", "Device already initialised");
    }

    return ok;
}

void bdUPnPDevice::pump()
{
	if (m_shutdownRequested)
	{
		m_shutdownRequested = false;
		m_state = BD_UPNP_DEVICE_SHUTTING_DOWN;
	}

	switch (m_state)
	{
		case BD_UPNP_DEVICE_UNINITIALISED:
		{
			bdLogWarn("upnp", "Device is not initialised");
			break;
		}
		case BD_UPNP_DEVICE_INITIALISED:
		{
			prepareGetDescriptionRequest();
			if (connectToDevice())
			{
				m_state = BD_UPNP_DEVICE_SENDING_DESCRIPTION_REQ;
			}
			else
			{
				bdLogWarn("upnp", "Failed to connect to the device");
				m_state = BD_UPNP_DEVICE_FAILED;
			}
			break;
		}
		case BD_UPNP_DEVICE_SENDING_DESCRIPTION_REQ:
		{
			bdBool isComplete = false;
			bdBool ok = pumpSendRequest(isComplete);

			if (isComplete && ok)
			{
				if (setLocalConnectedAddr())
				{
					m_state = BD_UPNP_DEVICE_GETTING_DESCRIPTION;
				}
				else
				{
					bdLogWarn("upnp", "Failed to find the local address");
					m_state = BD_UPNP_DEVICE_FAILED;
				}
			}
			else if (!ok)
			{
				bdLogWarn("upnp", "Failed to send the description request");
				m_state = BD_UPNP_DEVICE_FAILED;
			}
			break;
		}
		case BD_UPNP_DEVICE_GETTING_DESCRIPTION:
		{
			bdBool receiving = pumpReceive();
			if (!receiving)
			{
				if (parseDescriptionResponse())
				{
					m_state = BD_UPNP_DEVICE_GOT_DESCRIPTION;
				}
				else
				{
					bdLogWarn("upnp", "Failed to parse the description");
					m_state = BD_UPNP_DEVICE_FAILED;
				}
			}
			break;
		}
		case BD_UPNP_DEVICE_GOT_DESCRIPTION:
		{
			bdLogInfo("upnp", "Got the device description");

			prepareGetExternalIPRequest();
			if (connectToDevice())
			{
				m_state = BD_UPNP_DEVICE_SENDING_IP_REQ;
			}
			else
			{
				bdLogWarn("upnp", "Failed to connect to the device");
				m_state = BD_UPNP_DEVICE_FAILED;
			}
			break;
		}
		case BD_UPNP_DEVICE_SENDING_IP_REQ:
		{
			bdBool isComplete = false;
			bdBool ok = pumpSendRequest(isComplete);

			if (isComplete && ok)
			{
				m_state = BD_UPNP_DEVICE_DISCOVERING_IP;
			}
			else if (!ok)
			{
				bdLogWarn("upnp", "Failed to send the external IP request");
				m_state = BD_UPNP_DEVICE_FAILED;
			}
			break;
		}
		case BD_UPNP_DEVICE_DISCOVERING_IP:
		{
			bdBool receiving = pumpReceive();
			if (!receiving)
			{
				if (parseGetExternalIPResponse())
				{
					m_state = BD_UPNP_DEVICE_DISCOVERED_IP;
				}
				else
				{
					bdLogWarn("upnp", "Failed to get the external IP address");
					m_state = BD_UPNP_DEVICE_FAILED;
				}
			}
			break;
		}
		case BD_UPNP_DEVICE_DISCOVERED_IP:
		{
			if (m_config.m_runMode == bdUPnPConfig::BD_UPNP_DO_PORT_MAPPING)
			{

				sendCreatePortMappingRequest();
			}
			else
			{


				bdLogInfo("upnp", "External IP only, finished");
				m_state = BD_UPNP_DEVICE_FINISHED;
			}
			break;
		}
		case BD_UPNP_DEVICE_SENDING_MAPPING_REQ:
		{
			bdBool isComplete = false;
			bdBool ok = pumpSendRequest(isComplete);

			if (isComplete && ok)
			{
				m_state = BD_UPNP_DEVICE_MAPPING_PORT;
			}
			else if (!ok)
			{
				bdLogWarn("upnp", "Failed to send the mapping request");
				m_state = BD_UPNP_DEVICE_FAILED;
			}
			break;
		}
		case BD_UPNP_DEVICE_MAPPING_PORT:
		{
			bdBool receiving = pumpReceive();
			if (!receiving)
			{
				bdInt32 errorCode;
				if (confirmHttpSuccess())
				{
					m_portStatus = BD_UPNP_PORT_MAPPED;
					m_state = BD_UPNP_DEVICE_MAPPED_PORT;
				}
				else if (extractUPnPError(errorCode))
				{
					if (errorCode == 718)
					{
						bdLogInfo("upnp", "External port %u is already mapped", m_externalPort);
						if (m_mappingAttempts < m_config.m_mappingRetries + 1)
						{
							selectNewExternalPort();


							sendCreatePortMappingRequest();
						}
						else
						{
							bdLogWarn("upnp", "Giving up after %u mapping attempts", m_mappingAttempts);
							m_state = BD_UPNP_DEVICE_FAILED;
						}
					}
					else if (errorCode == 724)
					{
						bdLogWarn("upnp", "The device only supports matching internal and external ports");
						m_state = BD_UPNP_DEVICE_FAILED;
					}
					else
					{
						bdLogWarn("upnp", "Mapping failed with UPnP error %d", errorCode);
						m_state = BD_UPNP_DEVICE_FAILED;
					}
				}
				else
				{
					bdLogWarn("upnp", "Mapping failed");
					m_state = BD_UPNP_DEVICE_FAILED;
				}
			}
			break;
		}
		case BD_UPNP_DEVICE_MAPPED_PORT:
		{

			if (m_portStatus == BD_UPNP_PORT_MAPPED)
			{
				bdLogInfo("upnp", "Port mapped");
				m_state = BD_UPNP_DEVICE_FINISHED;
			}
			else
			{
				bdLogWarn("upnp", "Port not mapped");
				m_state = BD_UPNP_DEVICE_FAILED;
			}
			break;
		}
		case BD_UPNP_DEVICE_SENDING_UNMAPPING_REQ:
		{
			bdBool isComplete = false;
			bdBool ok = pumpSendRequest(isComplete);

			if (isComplete && ok)
			{
				m_state = BD_UPNP_DEVICE_UNMAPPING_PORT;
			}
			else if (!ok)
			{
				bdLogWarn("upnp", "Failed to send the unmapping request");
				m_state = BD_UPNP_DEVICE_FAILED;
			}
			break;
		}
		case BD_UPNP_DEVICE_UNMAPPING_PORT:
		{
			bdBool receiving = pumpReceive();
			if (!receiving)
			{
				if (confirmHttpSuccess())
				{
					bdLogInfo("upnp", "Unmapped port %u", m_gamePort);
					m_portStatus = BD_UPNP_PORT_NOT_MAPPED;
					m_state = BD_UPNP_DEVICE_UNMAPPED_PORT;
				}
				else
				{
					bdLogWarn("upnp", "Unmapping failed");
					m_state = BD_UPNP_DEVICE_FAILED;
				}
			}
			break;
		}
		case BD_UPNP_DEVICE_UNMAPPED_PORT:
		{
			if (m_portStatus == BD_UPNP_PORT_NOT_MAPPED)
			{
				m_shutdownType = BD_UPNP_DEVICE_SHUTDOWN_IMMEDIATE;
				m_state = BD_UPNP_DEVICE_SHUTTING_DOWN;
			}
			else
			{
				bdLogWarn("upnp", "Port is still mapped");
				m_state = BD_UPNP_DEVICE_FAILED;
			}
			break;
		}
		case BD_UPNP_DEVICE_SHUTTING_DOWN:
		{
			m_streamSocket.close();

			if (m_shutdownType == BD_UPNP_DEVICE_SHUTDOWN_ELEGANT
				&& m_portStatus == BD_UPNP_PORT_MAPPED)
			{
				preparePortUnMappingRequest();
				if (connectToDevice())
				{
					m_state = BD_UPNP_DEVICE_SENDING_UNMAPPING_REQ;
				}
				else
				{
					bdLogWarn("upnp", "Failed to connect to the device");
					m_state = BD_UPNP_DEVICE_FAILED;
				}
			}
			else
			{
				cleanup();
			}
			break;
		}
		case BD_UPNP_DEVICE_FINISHED:
		{
			break;
		}
		case BD_UPNP_DEVICE_FAILED:
		{



			if (m_streamSocket.isConnected())
			{
				m_streamSocket.close();
			}
			break;
		}
		default:
		{
			bdLogWarn("upnp", "Unknown device state");
			m_state = BD_UPNP_DEVICE_FAILED;
		}
	}
}
void bdUPnPDevice::startShutdown(const bdUPnPDevice::bdUPnPDeviceShutdownType shutdownType)
{
    if (m_state != BD_UPNP_DEVICE_UNINITIALISED)
    {
        switch (shutdownType)
        { case BD_UPNP_DEVICE_SHUTDOWN_IMMEDIATE:
            cleanup(); break;
        case BD_UPNP_DEVICE_SHUTDOWN_ELEGANT:
            if (m_state < BD_UPNP_DEVICE_SENDING_UNMAPPING_REQ || m_state > BD_UPNP_DEVICE_SHUTTING_DOWN)
            {



                m_shutdownType = shutdownType;
                m_shutdownRequested = true;
            }
        }
    }
}

inline void bdUPnPDevice::cleanup()
{
    m_state = BD_UPNP_DEVICE_UNINITIALISED;
    m_shutdownRequested = false;
    m_portStatus = BD_UPNP_PORT_STATUS_UNKNOWN;
    m_deviceAddr = bdAddr();
    m_localAddrs = NULL;
    m_localConnectedAddr = bdInetAddr();
    m_streamSocket.close();
    m_config = bdUPnPConfig();
    m_gamePort = 0; m_externalPort = 0;

    m_externalDeviceAddr = bdInetAddr();
    m_requestSize = 0;
    m_bytesReceived = 0;
    m_mappingAttempts = 0;
}

bdAddr bdUPnPDevice::getDeviceAddr() const
{
    return m_deviceAddr;
}

bdAddr bdUPnPDevice::getExternalAddr() const
{
    return bdAddr(m_externalDeviceAddr, m_gamePort);
}

bdUPnPDevice::bdUPnPPortStatus bdUPnPDevice::getPortStatus() const
{
    return m_portStatus;
}

bdUPnPDevice::bdUPnPDeviceState bdUPnPDevice::getDeviceState() const
{
    return m_state;
}

bdBool bdUPnPDevice::connectToDevice()
{
    bdBool ok = false;

    if (m_streamSocket.create(false))
    {


        if (m_deviceAddr.getPort() == 0)
        {
            m_deviceAddr.setPort(80);
        }


        const bdSocketStatusCode status = m_streamSocket.connect(m_deviceAddr);
        if (status == BD_NET_SUCCESS || status == BD_NET_WOULD_BLOCK)
        {

            m_connectTimer.reset();
            m_connectTimer.start(); ok = true;
        }
    }
    return ok;
}

bdBool bdUPnPDevice::setLocalConnectedAddr()
{
	bdBool result = false;






















	if (!m_localAddrs->isEmpty())
	{
		m_localConnectedAddr = (*m_localAddrs)[0];
		result = true;
	}
	return result;
}



inline void bdUPnPDevice::setupReceive()
{


    m_bytesReceived = 0;

    bdMemset(m_readBuffer, 0, sizeof(m_readBuffer)); m_responseTimer.reset();
    m_responseTimer.start();
}
bdBool bdUPnPDevice::pumpSendRequest(bdBool & isComplete)
{
    isComplete = false;
    bdBool ok = true;

    if (m_streamSocket.isConnected())
    {


        const bdInt sent = m_streamSocket.send(m_requestBuffer, m_requestSize);
        if (sent > 0)
        {
            setupReceive();
            isComplete = true;
        }
        else if (sent != BD_NET_WOULD_BLOCK)
        {
            bdLogWarn("upnp", "Failed to send the request"); m_streamSocket.close(); ok = false;
        }
    }
    else if (m_connectTimer.getElapsedTimeInSeconds() > m_config.m_connectTimeout)
    {
        bdLogWarn("upnp", "Timed out connecting to the device"); m_streamSocket.close(); ok = false;
    }

    return ok;
}






bdBool bdUPnPDevice::pumpReceive()
{
	bdBool result = false;

	if (m_responseTimer.getElapsedTimeInSeconds() <= m_config.m_responseTimeout)
	{

		const bdInt bytesReceived = m_streamSocket.recv(&m_readBuffer[m_bytesReceived],
		                                         sizeof(m_readBuffer) - m_bytesReceived);

		if (bytesReceived > 0)
		{
			m_readBuffer[m_bytesReceived + bytesReceived] = '\0';


			m_bytesReceived += bytesReceived;
			result = true;
		}
		else if (bytesReceived == BD_NET_WOULD_BLOCK)
		{
			result = true;
		}
		else if (bytesReceived != 0)
		{
			bdLogWarn("upnp", "Failed to receive from the device");
		}
	}
	else
	{
		bdLogWarn("upnp", "Timed out waiting for the response");
	}


	if (!result)
	{
		m_streamSocket.close();
	}
	return result;
}


void bdUPnPDevice::selectNewExternalPort()
{
	switch (m_config.m_portSelectionMode)
	{
		case bdUPnPConfig::BD_UPNP_PORT_SELECTION_EXPANDING_RANDOM:
		{


			bdUInt poolPortion = m_config.m_portPoolSize * m_mappingAttempts / m_config.m_mappingRetries;
			m_externalPort = static_cast<bdPort>(m_gamePort + (bdSingleton<bdTrulyRandomImpl>::getInstance()->getRandomUInt() % poolPortion));
			break;
		}
		case bdUPnPConfig::BD_UPNP_PORT_SELECTION_RANDOM:
		{
			m_externalPort = static_cast<bdPort>(m_gamePort + (bdSingleton<bdTrulyRandomImpl>::getInstance()->getRandomUInt() % m_config.m_portPoolSize));
			break;
		}
		default:
		{

			m_externalPort++;
			break;
		}
	}
}


void bdUPnPDevice::sendCreatePortMappingRequest()
{
    prepareCreatePortMappingRequest();
    if (connectToDevice())
    {
        bdLogInfo("upnp", "Requesting mapping of external port %u to %u", m_externalPort, m_gamePort);

        ++m_mappingAttempts; m_state = BD_UPNP_DEVICE_SENDING_MAPPING_REQ;
    }
    else
    {

        bdLogWarn("upnp", "Failed to connect to the device"); m_state = BD_UPNP_DEVICE_FAILED;
    }

}

inline bdBool bdUPnPDevice::confirmHttpSuccess()
{
    return bdStrstr(m_readBuffer, "200 OK") != NULL;
}

bdBool bdUPnPDevice::extractUPnPError(bdInt32 & errorCode)
{
    bdBool found = false;
    bdNChar8 * errorLoc = NULL;
    bdUInt errorLen = 0;
    if (bdStrstr(m_readBuffer, "UPnPError") && extractTag("<errorCode", m_readBuffer, &errorLoc, errorLen))
    {



        bdNChar8 safeErrorCodeBuffer[5];
        bdMemset(safeErrorCodeBuffer, 0, sizeof(safeErrorCodeBuffer));

        bdMemcpy(safeErrorCodeBuffer, errorLoc, errorLen < 4 ? errorLen : 4);
        errorCode = atoi(safeErrorCodeBuffer); found = true;
    }
    else
    {
        bdLogWarn("upnp", "No UPnP error code in the response");
    }







    return found;
}






inline void bdUPnPDevice::genUPnPCommand(const bdNChar8 *const command, const bdNChar8 *const arguments)
{
    bdNChar8 soapBody[682];











    const bdUInt soapLength = bdSnprintf(soapBody, sizeof(soapBody), "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n<s:Body>\r\n<u:%s xmlns:u=\"urn:schemas-upnp-org:service:%s\">\r\n%s</u:%s>\r\n</s:Body>\r\n</s:Envelope>\r\n",
                                         command,


                                         m_isIp ? "WANIPConnection:1" : "WANPPPConnection:1", arguments, command);

    bdNChar8 deviceAddr[22];

    m_deviceAddr.toString(deviceAddr, sizeof(deviceAddr));










    m_requestSize = bdSnprintf(m_requestBuffer, sizeof(m_requestBuffer), "POST %s HTTP/1.1\r\nHOST: %s\r\nSOAPACTION: \"urn:schemas-upnp-org:service:%s#%s\"\r\nCONTENT-TYPE: text/xml; charset=\"utf-8\"\r\nCONTENT-LENGTH: %d\r\n\r\n%s",


                               m_controlURL, deviceAddr,

                               m_isIp ? "WANIPConnection:1" : "WANPPPConnection:1", command, soapLength, soapBody);
}






inline bdBool bdUPnPDevice::extractTag(const bdNChar8 * tag, bdNChar8 * buffer, bdNChar8 ** value, bdUInt & valueLen)
{
    bdBool found = false;






    bdNChar8 * start = bdStrstr(buffer, tag);
    if (start)
    {

        start = bdStrchr(start, '>');
        if (start)
        {

            ++start;
            bdNChar8 * end = bdStrstr(start, "</");


            if (end)
            {

                valueLen = static_cast<bdUInt>(end - start);
                if (valueLen != 0)
                {
                    *value = start; found = true;
                }
            }
            else
            {


                bdLogWarn("upnp", "Unterminated tag %s", tag);
            }
        }
    }
    return found;
}

void bdUPnPDevice::prepareGetDescriptionRequest()
{
	bdNChar8 deviceAddrBuffer[22];
	const bdUWord addrStringSize = m_deviceAddr.toString(deviceAddrBuffer, sizeof(deviceAddrBuffer));


	// inferred: totalRequestSize (DWARF line 759, unused) reads m_fetchLocation at 759:69; its exact expression is not recoverable
	bdUInt totalRequestSize = static_cast<bdUInt>(sizeof(s_getRequest) + addrStringSize + bdStrlen(m_fetchLocation));







	m_requestSize = bdSnprintf(m_requestBuffer, sizeof(m_requestBuffer), s_getRequest, m_fetchLocation, deviceAddrBuffer);
}





bdBool bdUPnPDevice::parseDescriptionResponse()
{


    bdNChar8 * baseLoc = NULL;
    bdUInt baseLen = 0;

    bdNChar8 * controlLoc = NULL;
    bdUInt controlLen = 0;



    if (!confirmHttpSuccess()) return false;

    bdBool ok = extractURLBase(&baseLoc, baseLen);


    if (ok)
    {
        bdBool isIp = false;
        ok = extractServiceType(isIp);


        ok = ok && extractControlURL(&controlLoc, controlLen);


        if (ok)
        {

            m_isIp = isIp;


            if (baseLen != 0 && baseLen + controlLen < 128)
            {


                if (baseLoc[baseLen - 1] == '/' && controlLoc[0] == '/')
                {

                    ++controlLoc;
                    --controlLen;
                }


                bdStrlcpy(m_controlURL, baseLoc, baseLen + 1);
                bdStrlcpy(m_controlURL + baseLen, controlLoc, controlLen + 1);
            }
            else if (controlLen > 0 && controlLen < 128)
            {

                bdStrlcpy(m_controlURL, controlLoc, controlLen + 1);
            }
            else { ok = false; }
        }
    }


    return ok;
}

bdBool bdUPnPDevice::extractURLBase(bdNChar8 ** baseLoc, bdUInt & baseLen)
{
    bdBool ok = true;
    *baseLoc = NULL;
    baseLen = 0;
    bdNChar8 * urlBase = NULL;
    bdUInt urlBaseLen = 0;


    bdNChar8 tag[] = "<URLBase";
    if (extractTag(tag, m_readBuffer, &urlBase, urlBaseLen))
    {




        if (bdStrncmp(urlBase, "http://", 7) == 0)
        {
            bdNChar8 * end = urlBase + urlBaseLen;


            bdNChar8 * addrStart = urlBase + 7;
            bdNChar8 * slash = bdStrchr(addrStart, '/');



            if (slash < end)
            {
                *baseLoc = slash;
                baseLen = static_cast<bdUInt>(end - slash); end = slash;
            }







            const bdUInt addrLen = static_cast<bdUInt>(end - addrStart);
            bdNChar8 addrBuffer[22];

            if (addrLen < sizeof(addrBuffer))
            {
                bdStrlcpy(addrBuffer, addrStart, addrLen + 1);
                m_deviceAddr.set(addrBuffer);
            }
            else
            {
                ok = false;
            }
        }
        else
        {
            bdLogWarn("upnp", "URLBase is not an http URL"); ok = false;
        }
    }



    return ok;
}

inline bdBool bdUPnPDevice::extractServiceType(bdBool & serviceType)
{
    bdBool found = true;


    if (bdStrstr(m_readBuffer, "WANIPConnection:1"))
    {
        serviceType = true;
    }
    else



    if (bdStrstr(m_readBuffer, "WANPPPConnection:1"))
    {
        serviceType = false;
    }
    else
    {


        bdLogWarn("upnp", "No WAN connection service in the description"); found = false;
    }
    return found;
}


bdBool bdUPnPDevice::extractControlURL(bdNChar8 ** controlLoc, bdUInt & controlLen)
{
    bdBool ok = false;
    *controlLoc = NULL;
    controlLen = 0;
    bdNChar8 * url = NULL;
    bdUInt urlLen = 0;
    const bdBool isIp = m_isIp;
    bdNChar8 * service = bdStrstr(m_readBuffer, isIp ? "WANIPConnection:1" : "WANPPPConnection:1");
    if (service)
    {

        bdNChar8 tag[] = "<controlURL";
        if (extractTag(tag, service, &url, urlLen))
        {



            bdNChar8 * end = url + urlLen;


            if (bdStrncmp(url, "http://", 7) == 0)
            {


                bdNChar8 * addrStart = url + 7;
                bdNChar8 * slash = bdStrchr(addrStart, '/');


                bdNChar8 * pathStart = (slash != NULL && slash <= end) ? slash : end;





                const bdUInt addrLen = static_cast<bdUInt>(pathStart - addrStart);
                bdNChar8 addrBuffer[22];
                if (addrLen < sizeof(addrBuffer))
                {

                    bdStrlcpy(addrBuffer, addrStart, addrLen + 1);
                    m_deviceAddr.set(addrBuffer);
                }
                *controlLoc = pathStart;
                controlLen = static_cast<bdUInt>(end - pathStart);
            }
            else
            {

                *controlLoc = url;
                controlLen = urlLen;
            }
            ok = true;
        }
        else
        {







            bdLogWarn("upnp", "No control URL in the description");
        }
    }

    return ok;
}



void bdUPnPDevice::prepareGetExternalIPRequest()
{
	genUPnPCommand(s_getExternalAddressCommand, s_getExternalAddressArguments);
}

bdBool bdUPnPDevice::parseGetExternalIPResponse()
{
	bdBool ok = true;
	bdInetAddr externalAddress;

	ok = confirmHttpSuccess();

	if (ok)
	{
		ok = extractExternalAddress(externalAddress);
	}

	if (ok)
	{
		m_externalDeviceAddr = externalAddress;
	}

	return ok;
}

bdBool bdUPnPDevice::extractExternalAddress(bdInetAddr & externalAddress)
{

    externalAddress = bdInetAddr(); bdBool ok = true;
    bdNChar8 * value = NULL;
    bdUInt valueLen = 0;


    bdNChar8 tag[] = "<NewExternalIPAddress";
    if (extractTag(tag, m_readBuffer, &value, valueLen))
    {
        bdNChar8 external_ip_address_string[22];

        if (valueLen <= 21)
        {
            bdStrlcpy(external_ip_address_string, value, valueLen + 1);
            externalAddress.set(external_ip_address_string);
            bdLogInfo("upnp", "External address %s", external_ip_address_string);
        }
        else
        {
            ok = false;
        }
    }
    return ok;
}




























































































inline void bdUPnPDevice::prepareCreatePortMappingRequest()
{
    bdNChar8 portMapArguments[412];
    bdNChar8 localAddr[22];





    m_localConnectedAddr.toString(localAddr, sizeof(localAddr));













    bdSnprintf(portMapArguments, sizeof(portMapArguments), "      <NewRemoteHost></NewRemoteHost>\r\n      <NewExternalPort>%d</NewExternalPort>\r\n      <NewProtocol>UDP</NewProtocol>\r\n      <NewInternalPort>%d</NewInternalPort>\r\n      <NewInternalClient>%s</NewInternalClient>\r\n      <NewEnabled>1</NewEnabled>\r\n      <NewPortMappingDescription>DemonwarePortMapping</NewPortMappingDescription>\r\n      <NewLeaseDuration>0</NewLeaseDuration>\r\n",
               m_externalPort, m_gamePort, localAddr);
    genUPnPCommand("AddPortMapping", portMapArguments);
}



void bdUPnPDevice::preparePortUnMappingRequest()
{
	const bdUInt maxArgLength = 110;


	bdNChar8 unMapArguments[maxArgLength];






	bdSnprintf(unMapArguments, maxArgLength, s_removePortMappingArguments, m_gamePort);

	genUPnPCommand(s_removePortMappingCommand, unMapArguments);
}

