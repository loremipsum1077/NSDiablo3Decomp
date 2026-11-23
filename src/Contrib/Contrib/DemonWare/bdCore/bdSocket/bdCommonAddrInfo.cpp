#include "bdCommonAddrInfo.h"
#include "bdCommonAddr.h"
#include "../bdContainers/bdArray.inl"
#include "../../bdPlatform/bdPlatformSocket/bdPlatformSocket.h"
#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"
// Blank lines keep the original line numbers










bdUInt bdCommonAddrInfo::getInfo(const bdCommonAddr& addr, bdNChar8* buf, const bdUInt length)
{

    bdNChar8* const end = buf + length;



    if (!bdPlatformSocket::getAdHoc())
    {
        const bdNChar8* const open = "BD_NAT_OPEN";
        const bdNChar8* const moderate = "BD_NAT_MODERATE";
        const bdNChar8* const strict = "BD_NAT_STRICT";
        const bdNChar8* const unknown = "**UNKNOWN**";

        const bdNChar8* natType;

        switch (addr.getNATType())
        {
        case BD_NAT_OPEN:
            natType = open;
            break;
        case BD_NAT_MODERATE:
            natType = moderate;
            break;
        case BD_NAT_STRICT:
            natType = strict;
            break;
        default:
            natType = unknown;
            break;
        }








        bdInt written = bdSnprintf(buf, static_cast<bdUInt>(end - buf), "NAT Type: %s\n", natType);
        if (written > 0 && written <= static_cast<bdInt>(end - buf))
        {
            buf += written;
        }
        else
        {
            buf = end - 1;
            if (length > 0)
            {
                *buf = '\0';
            }
        }
    }

    {

        bdInt written = bdSnprintf(buf, static_cast<bdUInt>(end - buf), "%u Local Addrs Found: \n", addr.getLocalAddrs().getSize());
        if (written > 0 && written <= static_cast<bdInt>(end - buf))
        {
            buf += written;
        }
        else
        {
            buf = end - 1;
            if (length > 0)
            {
                *buf = '\0';
            }
        }
    }
    for (bdUInt i = 0; i < addr.getLocalAddrs().getSize(); ++i)
    {
        bdNChar8 addrString[22];
        addr.getLocalAddrs()[i].toString(addrString, sizeof(addrString));

        bdInt written = bdSnprintf(buf, static_cast<bdUInt>(end - buf), "Local Addr %u :  %s \n", i, addrString);
        if (written > 0 && written <= static_cast<bdInt>(end - buf))
        {
            buf += written;
        }
        else
        {
            buf = end - 1;
            if (length > 0)
            {
                *buf = '\0';
            }
        }
    }




    if (!bdPlatformSocket::getAdHoc())
    {
        bdNChar8 addrString[22];
        addr.getPublicAddr().toString(addrString, sizeof(addrString));

        bdInt written = bdSnprintf(buf, static_cast<bdUInt>(end - buf), "Public Addr :  %s \n", addrString);
        if (written > 0 && written <= static_cast<bdInt>(end - buf))
        {
            buf += written;
        }
        else
        {
            buf = end - 1;
            if (length > 0)
            {
                *buf = '\0';
            }
        }
    }

    {
        const bdNChar8* const isLoopback = "TRUE";
        const bdNChar8* const isNotLoopback = "FALSE";





        const bdNChar8* loop = addr.isLoopback() ? isLoopback : isNotLoopback;








        const bdUInt32 tempHash = addr.getHash();
        bdInt written = bdSnprintf(buf, static_cast<bdUInt>(end - buf), "Addr is loopback : %s  -- Addr Hash : %u \n", loop, tempHash);
        if (written > 0 && written <= static_cast<bdInt>(end - buf))
        {
            buf += written;
        }
        else
        {
            buf = end - 1;
            if (length > 0)
            {
                *buf = '\0';
            }
        }
    }

    const bdUInt bytesWritten = length - static_cast<bdUInt>(end - buf);
    return bytesWritten;
}


bdUInt bdCommonAddrInfo::getInfo(const bdCommonAddrRef addr, bdNChar8* buf, const bdUInt length)
{


    bdUInt bytesWritten = 0;
    if (addr.notNull())
    {
        bytesWritten = getInfo(*addr, buf, length);
    }

    return bytesWritten;
}

bdUInt bdCommonAddrInfo::getBriefInfo(const bdCommonAddr& addr, bdNChar8* buf, const bdUInt length)
{


    bdNChar8* const end = buf + length;
    bdInt written;

    if (addr.getPublicAddr().getAddress().isValid())
    {
        bdNChar8 addrString[22];
        addr.getPublicAddr().toString(addrString, sizeof(addrString));

        written = bdSnprintf(buf, length, "Public Addr: %s\n", addrString);
    }
    else if (addr.getLocalAddrs().getSize())
    {
        const bdUInt addrsStringSize = 240;
        bdNChar8 addrsString[addrsStringSize];
        bdNChar8* start = addrsString;




        for (bdUInt i = 0; i < addr.getLocalAddrs().getSize(); ++i)
        {
            start += addr.getLocalAddrByIndex(i).toString(start, addrsString + addrsStringSize - start);

            start += bdSnprintf(start, addrsString + addrsStringSize - start, ", ");
        }

        start[-2] = '\0';

        written = bdSnprintf(buf, length, "Local Addr: %s\n", addrsString);
    }
    else
    {


        written = bdSnprintf(buf, length, "Empty Common Addr\n");
    }

    if (written > 0 && written <= static_cast<bdInt>(length))
    {
        buf += written;
    }
    else
    {
        buf = end - 1;
        if (length > 0)
        {
            *buf = '\0';
        }
    }

    const bdUInt bytesWritten = length - static_cast<bdUInt>(end - buf);
    return bytesWritten;
}

bdUInt bdCommonAddrInfo::getBriefInfo(const bdCommonAddrRef addr, bdNChar8* buf, const bdUInt length)
{
    bdUInt bytesWritten = 0;


    if (addr.notNull())
    {
        bytesWritten = getBriefInfo(*addr, buf, length);
    }
    return bytesWritten;
}
