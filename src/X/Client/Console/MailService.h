// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Console/MailService.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Online.h"

namespace D3 { namespace Console { class MailItem; } }
class bdUserAccountID;

namespace Console {
namespace Online {
class MailService {  // size 0x8
    // vtable order: GetMail..MarkRead, then the destructor (MailServiceInternal's vtable).
    // vtable pointer
public:
    static void Create();
    static Console::Online::MailService * Get(int32 nUserIndex);
    static void Idle();
    static void SetTimeout(Console::Online::MailType eType, int32 nTimeoutDays);
    virtual void GetMail(Console::Online::MailType, Console::Online::MailArray &, uint32, uint32) = 0;  // vtable slot 0
    virtual BOOL GetHasMail(Console::Online::MailType) = 0;  // vtable slot 1
    virtual void SendMail(Console::Online::MailType, const D3::Console::MailItem &, const bdUserAccountID &, Console::Online::SendMailCallback) = 0;  // vtable slot 2
    virtual void DeleteMail(uint64, Console::Online::DeleteMailCallback) = 0;  // vtable slot 3
    virtual void VerifyMail(uint64, Console::Online::VerifyMailCallback) = 0;  // vtable slot 4
    virtual void MarkRead(uint64) = 0;  // vtable slot 5
protected:
    virtual ~MailService() {}
};
static_assert(sizeof(MailService) == 0x8, "MailService");

}  // namespace Online
}  // namespace Console
