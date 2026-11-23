// Original file: Contrib/Contrib/DemonWare/bdNet/bdUPnP/bdUPnP.cpp
#include "bdUPnP.h"
#include "../../bdCore/bdContainers/bdArray.inl"
#include "../../bdCore/bdSocket/bdAddr.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformSocket/bdPlatformSocket.h"
#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"







bdUPnP::bdUPnP()
    : m_state(BD_UPNP_UNINITIALISED),
      m_gamePort(0),
      m_localAddrs(NULL),
      m_bytesReceived(0),
      m_numDiscoveryAttempts(0),
      m_shutdownRequested(false)
{
}

bdUPnP::~bdUPnP()
{
}

bdBool bdUPnP::init(bdPort port,
                    bdArray<bdInetAddr> * localAddrs,
                    bdUPnPConfig config)
{
    bdBool ok = false;









    if (config.m_disabled || bdPlatformSocket::getAdHoc())
    {
        if (bdPlatformSocket::getAdHoc())
        {
            bdLogInfo("upnp", "UPnP is not used in ad hoc mode");
        }
        else
        {
            bdLogInfo("upnp", "UPnP is disabled");
        }
        m_state = BD_UPNP_UNINITIALISED; ok = true;
    }
    else
    if (m_state == BD_UPNP_UNINITIALISED)
    {

        if (!localAddrs->isEmpty())
        {
            m_localAddrs = localAddrs;
            m_gamePort = port;
            m_config = config;
            m_config.sanityCheckConfig();

            if (!m_config.m_gatewayAddr.isValid())
            {
                bdLogInfo("upnp", "No gateway address configured");
            }

            m_state = BD_UPNP_INITIALISED; ok = true;
        }
        else
        {

            bdLogWarn("upnp", "No local addresses");
        }
    }
    else
    {
        bdLogWarn("upnp", "Already initialised (state %u)", m_state);
    }

    return ok;
}

void bdUPnP::pump()
{
	if (m_shutdownRequested)
	{
		m_shutdownRequested = false;
		m_state = BD_UPNP_SHUTTING_DOWN;
	}

	switch (m_state)
	{
		case BD_UPNP_UNINITIALISED:
		{
			bdLogWarn("upnp", "UPnP is not initialised");
			break;
		}
		case BD_UPNP_INITIALISED:
		{
			if (setupDiscoverySocket())
			{
				if (startDiscovery())
				{
					m_bytesReceived = 0;
					bdMemset(m_readBuffer, 0, sizeof(m_readBuffer));
					m_discoveryTimer.reset();
					m_discoveryTimer.start();
					m_numDiscoveryAttempts++;
					m_state = BD_UPNP_DISCOVERING_DEVICES;
				}
				else
				{
					bdLogWarn("upnp", "Failed to start discovery");
					m_state = BD_UPNP_FAILED;
				}
			}
			else
			{
				bdLogWarn("upnp", "Failed to set up the discovery socket");
				m_state = BD_UPNP_FAILED;
			}
			break;
		}
		case BD_UPNP_DISCOVERING_DEVICES:
		{
			if (m_discoveryTimer.getElapsedTimeInSeconds() >= m_config.m_discoveryTimeout)
			{
				if (m_cachedDevice.getDeviceState() == bdUPnPDevice::BD_UPNP_DEVICE_INITIALISED)
				{
					bdLogInfo("upnp", "Using cached device after %f seconds", m_discoveryTimer.getElapsedTimeInSeconds());
					m_device = m_cachedDevice;
					m_state = BD_UPNP_DISCOVERED_DEVICE;
				}
				else
				{
					if (m_discoverySocket.close())
					{
						if (m_numDiscoveryAttempts <= m_config.m_discoveryRetries)
						{

							bdLogInfo("upnp", "No device found, retrying discovery");
							m_state = BD_UPNP_INITIALISED;
						}
						else
						{
							bdLogInfo("upnp", "No device found");
							m_state = BD_UPNP_FINISHED;
						}
					}
					else
					{
						bdLogWarn("upnp", "Failed to close the discovery socket");
						m_state = BD_UPNP_FAILED;
					}
				}
			}
			else
			{
				bdUPnPDevice discoveredDevice;

				bdBool ok = checkForDiscoveredDevices(discoveredDevice);
				if (discoveredDevice.getDeviceState() == bdUPnPDevice::BD_UPNP_DEVICE_INITIALISED && ok)
				{
					if (!m_config.m_gatewayAddr.isValid()





						|| (m_config.m_gatewayAddr.isValid() &&
						discoveredDevice.getDeviceAddr().getAddress() == m_config.m_gatewayAddr))
					{
						bdLogInfo("upnp", "Configuring the discovered device");
						m_device = discoveredDevice;
						m_state = BD_UPNP_CONFIGURING_DEVICE;
					}
					else
					{
						if (m_cachedDevice.getDeviceState() == bdUPnPDevice::BD_UPNP_DEVICE_UNINITIALISED
							&& m_config.m_gatewayAddr.isValid() && !m_config.m_onlyUseGateway)
						{
							bdLogInfo("upnp", "Caching a device that is not the gateway");
							m_cachedDevice = discoveredDevice;
						}
						else
						{
							discoveredDevice.startShutdown(bdUPnPDevice::BD_UPNP_DEVICE_SHUTDOWN_IMMEDIATE);
							bdLogInfo("upnp", "Ignoring a device that is not the gateway");
						}
					}
				}
				else if (!ok)
				{
					bdLogWarn("upnp", "Discovery failed");
					m_state = BD_UPNP_FAILED;
				}
			}
			break;
		}
		case BD_UPNP_DISCOVERED_DEVICE:
		{
			if (m_discoverySocket.close())
			{
				m_state = BD_UPNP_CONFIGURING_DEVICE;
			}
			else
			{
				bdLogWarn("upnp", "Failed to close the discovery socket");
				m_state = BD_UPNP_FAILED;
			}
			break;
		}
		case BD_UPNP_CONFIGURING_DEVICE:
		{
			m_device.pump();
			if (m_device.getDeviceState() == bdUPnPDevice::BD_UPNP_DEVICE_FINISHED)
			{
				m_state = BD_UPNP_FINISHED;
			}
			else if (m_device.getDeviceState() == bdUPnPDevice::BD_UPNP_DEVICE_FAILED ||
				m_device.getDeviceState() == bdUPnPDevice::BD_UPNP_DEVICE_UNINITIALISED)
			{
				bdLogWarn("upnp", "Failed to configure the device");
				m_state = BD_UPNP_FAILED;
			}
			break;
		}
		case BD_UPNP_SHUTTING_DOWN:
		{
			m_device.pump();
			if (m_device.getDeviceState() == bdUPnPDevice::BD_UPNP_DEVICE_UNINITIALISED)
			{
				cleanup();
			}
			else if (m_device.getDeviceState() == bdUPnPDevice::BD_UPNP_DEVICE_FAILED)
			{


				startShutdown(bdUPnPDevice::BD_UPNP_DEVICE_SHUTDOWN_IMMEDIATE);
			}
			break;
		}
		case BD_UPNP_FINISHED:
		{
			break;
		}
		case BD_UPNP_FAILED:
		{


			m_discoverySocket.close();
			break;
		}
		default:
		{
			bdLogWarn("upnp", "Unknown state");
			m_state = BD_UPNP_FAILED;
		}
	}
}


bdBool bdUPnP::isRunning()
{
    switch (m_state)
    {
    case BD_UPNP_UNINITIALISED: case BD_UPNP_FINISHED: case BD_UPNP_FAILED: return false; default: break;
    }
    return true;
}

void bdUPnP::startShutdown(const bdUPnPDevice::bdUPnPDeviceShutdownType shutdownType)
{
    if (m_state != BD_UPNP_UNINITIALISED)
    {
        switch (shutdownType)
        { case bdUPnPDevice::BD_UPNP_DEVICE_SHUTDOWN_IMMEDIATE:
            m_device.startShutdown(shutdownType);
            cleanup(); break;
        case bdUPnPDevice::BD_UPNP_DEVICE_SHUTDOWN_ELEGANT:
            if (m_state != BD_UPNP_SHUTTING_DOWN)
            {
                m_device.startShutdown(shutdownType);
                m_shutdownRequested = true;
            }
        }
    }
}
inline void bdUPnP::cleanup()
{
    m_state = BD_UPNP_UNINITIALISED;
    m_shutdownRequested = false;
    m_gamePort = 0;
    m_localAddrs = NULL;
    m_config.reset();
    m_discoverySocket.close();
    m_bytesReceived = 0;
    m_device = bdUPnPDevice();
    m_cachedDevice = bdUPnPDevice();
    m_discoveryTimer.reset();
}

bdUPnP::bdUPnPState bdUPnP::getState() const
{
    return m_state;
}

bdBool bdUPnP::getExternalAddr(bdAddr & externalAddr) const
{

    externalAddr = m_device.getExternalAddr();
    return externalAddr.getAddress().isValid();
}






bdUPnPDevice::bdUPnPPortStatus bdUPnP::getPortStatus() const
{
    if (m_device.getDeviceState() == bdUPnPDevice::BD_UPNP_DEVICE_FINISHED)
    {
        return m_device.getPortStatus();
    }




    return bdUPnPDevice::BD_UPNP_PORT_STATUS_UNKNOWN;
}
bdBool bdUPnP::setupDiscoverySocket()
{
	bdBool result = false;
	const bdBool blocking = false;

	if (m_discoverySocket.create(blocking, true, true))
	{
		result = true;
	}
	else
	{

		bdLogWarn("upnp", "Failed to create the discovery socket");
	}
	return result;
}

bdBool bdUPnP::startDiscovery()
{
    bdBool ok = true;
    const bdNChar8 *const serviceTypes[2] = { "WANIPConnection:1", "WANPPPConnection:1" };

    for (bdUInt i = 0; i < 2; ++i)
    {
        bdNChar8 buffer[136];


        const bdUInt length = bdSnprintf(buffer, sizeof(buffer), "M-SEARCH * HTTP/1.1\r\nMX: 1\r\nHOST: 239.255.255.250:1900\r\nMAN: \"ssdp:discover\"\r\nST: urn:schemas-upnp-org:service:%s\r\n\r\n", serviceTypes[i]);




        const bdAddr destination("239.255.255.250:1900");
        const bdInt sent = m_discoverySocket.sendTo(destination, buffer, length);
        if (sent != static_cast<bdInt>(length))
        {

            bdLogWarn("upnp", "Failed to send discovery request"); ok = false;
        }
    }

    return ok;
}

bdBool bdUPnP::checkForDiscoveredDevices(bdUPnPDevice & discoveredDevice)
{
	bdBool result = false;

	bdAddr incomingAddress;
	const bdInt received = m_discoverySocket.receiveFrom(incomingAddress, m_readBuffer, sizeof(m_readBuffer) - 1);

	if (received > 0 && received < static_cast<bdInt>(sizeof(m_readBuffer)))
	{
		m_bytesReceived = received;
		m_readBuffer[m_bytesReceived] = '\0';
		result = handleDiscoveryResponse(discoveredDevice);
	}
	else if (received == BD_NET_WOULD_BLOCK)
	{
		result = true;
	}
	return result;
}


bdBool bdUPnP::handleDiscoveryResponse(bdUPnPDevice & discoveredDevice)
{
    bdBool ok = false;

    bdAddr deviceAddr;
    bdNChar8 * fetchLocation = NULL;
    bdUInt fetchLength = 0;

    ok = extractDeviceInfo(deviceAddr, &fetchLocation, fetchLength);

    if (ok)
    {
        bdNChar8 addrString[22];
        deviceAddr.toString(addrString, sizeof(addrString));
        bdLogInfo("upnp", "Discovered device at %s", addrString);

        ok = discoveredDevice.init(m_config,
                                   m_localAddrs,
                                   deviceAddr,
                                   fetchLocation,
                                   fetchLength,
                                   m_gamePort);
    }


    return ok;
}

bdBool bdUPnP::extractDeviceInfo(bdAddr & deviceAddr, bdNChar8 ** fetchLoc, bdUInt & fetchLen)
{
    bdBool ok = false;


    bdNChar8 * location = bdStrstr(m_readBuffer, "http://");
    if (location)
    {

        location += 7;
        bdNChar8 * path = bdStrchr(location, '/');



        if (path)
        {

            const bdUInt addrLength = static_cast<bdUInt>(path - location);


            if (addrLength >= 1 && addrLength <= 21)
            {
                bdNChar8 addrBuffer[22]; bdStrlcpy(addrBuffer, location, addrLength + 1);
                deviceAddr = bdAddr(addrBuffer);



                const bdNChar8 * end = bdStrchr(static_cast<const bdNChar8 *>(location), '\r');
                if (end)
                {
                    *fetchLoc = path;
                    fetchLen = static_cast<bdUInt>(end - path); ok = true;
                }
                else
                {
                    bdLogWarn("upnp", "Malformed discovery response");
                }
            }
        }
        else
        {
            bdLogWarn("upnp", "Malformed discovery response");
        }
    }
    else
    {
        bdLogWarn("upnp", "Malformed discovery response");
    }


    return ok;
}
