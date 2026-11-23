#include "bdNATTypeDiscoveryClient.h"
#include "bdPacket/bdNATTypeDiscoveryPacketReply.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"

// Release logging: only the surviving bdUseVAArgs calls and their arguments are known; channel and format text are not recoverable.






















































bdNATTypeDiscoveryClient::bdNATTypeDiscoveryClient()
	: m_NATType(BD_NAT_UNKNOWN),
	m_state(BD_NTDCS_UNINITIALIZED), m_resends(0)
{
}


bdNATTypeDiscoveryClient::~bdNATTypeDiscoveryClient()
{
}

bdBool bdNATTypeDiscoveryClient::init(bdDefaultSocket* socket,
	const bdAddr& serverAddr,
	bdNATTypeDiscoveryConfig config)
{
	bdBool result = false;
	if (m_state == BD_NTDCS_UNINITIALIZED)
	{
		if (socket)
		{
			m_socket = socket;

			bdUseVAArgs();

			m_serverAddr1 = serverAddr;

			m_config = config;

			m_config.sanityCheckConfig();
			// Inference: result is true on every path that reaches the send at 105 (w20 = 1 at 106 and 114) and has no row of its own.
			result = true;
		}
		else
		{
			bdUseVAArgs();
			m_state = BD_NTDCS_ERROR;
		}
	}
	else
	{
		bdUseVAArgs();
	}
    // Inference: the send at 105 runs only after the successful socket path; the test leaves no instruction (jump-threaded).
	if (result)
	{
		const bdBool sendResult = sendForTest1();
		if (sendResult)
		{
			m_state = BD_NTDCS_RUN_TEST_1;
			m_timer.start();
		}
		else
		{

			m_state = BD_NTDCS_ERROR;
		}
	}
	return result;
}

void bdNATTypeDiscoveryClient::pump(bdAddr fromAddr,
	bdUByte8* data,
	const bdInt dataSize)
{


	if (isRunning())
	{
		receiveReplies(fromAddr, data, dataSize);
		pumpActiveTest();
	}
	else
	{
		bdUseVAArgs();
	}
}

bdBool bdNATTypeDiscoveryClient::quit()
{
	bdBool result = false;
	if (m_state != BD_NTDCS_UNINITIALIZED)
	{
		bdUseVAArgs();
		m_socket = 0;

		m_state = BD_NTDCS_UNINITIALIZED;
		result = true;
	}
	else
	{
		bdUseVAArgs();
	}
	return result;
}
bdBool bdNATTypeDiscoveryClient::isRunning() const
{
	bdBool result = false;
	if (m_state >= BD_NTDCS_RUN_TEST_1 && m_state <= BD_NTDCS_RUN_TEST_3)
	{
		result = true;
	}


	return result;
}

bdNATType bdNATTypeDiscoveryClient::getNATType() const
{
	bdNATType result = BD_NAT_UNKNOWN;
	if (m_state == BD_NTDCS_FINI)
	{
		result = m_NATType;
	}
	else
	{
		bdUseVAArgs();
	}
	return result;
}

bdNATTypeDiscoveryClient::bdNATTypeDiscoveryClientState bdNATTypeDiscoveryClient::getStatus() const
{
	return m_state;
}


// Success log only (193:3); the fail log is the callee's merged one (line 0 in the original).
bdBool bdNATTypeDiscoveryClient::sendForTest1()
{
	bdBool result = sendNATTypeDiscoveryPacket(bdNATTypeDiscoveryPacket::BD_NTDP_SAME_ADDR, m_serverAddr1);
	if (result)
	{

		bdUseVAArgs();
	}

	return result;
}

bdBool bdNATTypeDiscoveryClient::sendForTest2()
{
	bdBool result = sendNATTypeDiscoveryPacket(bdNATTypeDiscoveryPacket::BD_NTDP_DIFF_PORT_IP, m_serverAddr1);
	if (result)
	{

		bdUseVAArgs();
	}
	return result;
}

bdBool bdNATTypeDiscoveryClient::sendForTest3()
{
	bdBool result = sendNATTypeDiscoveryPacket(bdNATTypeDiscoveryPacket::BD_NTDP_DIFF_IP, m_serverAddr2);
	if (result)
	{

		bdUseVAArgs();
	}
	return result;
}

void bdNATTypeDiscoveryClient::handleResponse(const bdAddr& addr,
	const bdNATTypeDiscoveryPacketReply& reply)
{
	switch (m_state)
	{
	case BD_NTDCS_RUN_TEST_1:
		{
			m_serverAddr2 = reply.getSecAddr();
			m_mappedAddr = reply.getMappedAddr();

			bdUseVAArgs();

			const bdBool sendResult = sendForTest2();
			if (!sendResult)
			{
				m_state = BD_NTDCS_ERROR;
			}
			else
			{
				m_state = BD_NTDCS_RUN_TEST_2;
				m_resends = 0;
			}
			break;
		}

	case BD_NTDCS_RUN_TEST_2:
		{
				const bdBool addressMatch = addr.getAddress() == m_serverAddr2.getAddress();


				const bdBool portMatch = addr.getPort() == m_serverAddr2.getPort();

			if (!portMatch && addressMatch)
			{


				bdUseVAArgs();


				m_NATType = BD_NAT_OPEN;
				m_state = BD_NTDCS_FINI;
			}
			else {
				if (!addressMatch)
				{
					bdNChar8 receivedAddrString[22];
					addr.toString(receivedAddrString, sizeof(receivedAddrString));
					bdNChar8 expectedAddrString[22];
					m_serverAddr2.toString(expectedAddrString, sizeof(expectedAddrString));
					bdUseVAArgs(receivedAddrString, expectedAddrString);
				}
				if (portMatch)
				{
					bdUseVAArgs();
				}
			}
			break;
		}

	case BD_NTDCS_RUN_TEST_3:
		{

		if (m_mappedAddr == reply.getMappedAddr())
		{
				bdUseVAArgs();
				m_state = BD_NTDCS_FINI;
				m_NATType = BD_NAT_MODERATE;
		}
		else
		{
				bdUseVAArgs();
				m_state = BD_NTDCS_FINI;
				m_NATType = BD_NAT_STRICT;
		}
		break;
		}

	default:
		break;
	}
// Inferred: the three failure logs in sendNATTypeDiscoveryPacket have no rows; its removed copy (0x98, ours now 0x98)
// has one merged line-0 call block before the return at 339, and all callers match only with them.
}

bdBool bdNATTypeDiscoveryClient::sendNATTypeDiscoveryPacket(bdNATTypeDiscoveryPacket::bdNATTypeDiscoveryPacketRequest packetType,
	const bdAddr& serverAddr)
{
	bdBool result = false;

	bdNATTypeDiscoveryPacket packet(packetType);
	const bdUInt bufferSize = 1288;
	bdUByte8 buffer[bufferSize];
	bdUInt len = 0;
	const bdBool serializeResult = packet.serialize(buffer, bufferSize, 0, len);
	if (serializeResult)
	{
		const bdInt lenDataSent = m_socket->sendTo(serverAddr, buffer, len);
		if (lenDataSent > 0)
		{
			if (static_cast<bdUInt>(lenDataSent) == len)
			{
				result = true;
			}
			else
			{
				bdUseVAArgs();
			}
		}
		else
		{
			bdUseVAArgs();
		}
	}
	else
	{
		bdUseVAArgs();
	}

	return result;
}

void bdNATTypeDiscoveryClient::receiveReplies(bdAddr fromAddr, bdUByte8* data,
	const bdInt dataSize)
{
	if (dataSize > 0)
	{



		bdNATTypeDiscoveryPacketReply reply;
		bdUInt tmpUInt;
		const bdBool deserializeResult = reply.deserialize(data, dataSize, 0, tmpUInt);
		if (deserializeResult)
		{
			if (reply.getType() == 21) // Literal 21 (0x15): reply packet type; no named constant in src or the owner CU.
			{
				handleResponse(fromAddr, reply);
			}
		}
	}
}

void bdNATTypeDiscoveryClient::pumpActiveTest()
{

	switch (m_state)
	{
	case BD_NTDCS_RUN_TEST_1:

		if (m_timer.getElapsedTimeInSeconds() > m_config.m_NtdcsSendTimeout)
		{
			if (m_resends++ < m_config.m_NtdcsMaxResends)
			{
				m_timer.start();
				const bdBool sendResult = sendForTest1();
				if (!sendResult)
				{
					m_state = BD_NTDCS_ERROR;
				}
			}
			else
			{
				bdUseVAArgs();
				m_state = BD_NTDCS_ERROR;
			}
		}
		break;

	case BD_NTDCS_RUN_TEST_2:

		if (m_timer.getElapsedTimeInSeconds() > m_config.m_NtdcsSendTimeout)
		{
			if (m_resends++ < m_config.m_NtdcsMaxResends)
			{
				m_timer.start();
				const bdBool sendResult = sendForTest2();
				if (!sendResult)
				{
					m_state = BD_NTDCS_ERROR;
				}
			}
			else
			{
				bdUseVAArgs();
				m_resends = 0;
				m_state = BD_NTDCS_RUN_TEST_3;
			}
		}
		break;

	case BD_NTDCS_RUN_TEST_3:

		if (m_timer.getElapsedTimeInSeconds() > m_config.m_NtdcsSendTimeout)
		{
			if (m_resends++ < m_config.m_NtdcsMaxResends)
			{
				m_timer.start();
				const bdBool sendResult = sendForTest3();
				if (!sendResult)
				{
					m_state = BD_NTDCS_ERROR;
				}
			}
			else
			{
				bdUseVAArgs();
				m_NATType = BD_NAT_STRICT;
				m_state = BD_NTDCS_FINI;
			}
		}
		break;








	case BD_NTDCS_UNINITIALIZED:

		bdUseVAArgs();
		break;
	default:
		break;
	}
}
