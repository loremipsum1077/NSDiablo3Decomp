// Original file: X/Common/ItemMailCommon.cpp

#include "ItemMailCommon.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../Contrib/Contrib/blz/include/blz/tuple.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer.
#include "SNO/SNOName.h"
#include "WorldPlace.h"
#include "../Client/Gfx/GfxVB.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Common/Networking/GameMessage.cpp:65; no adopted header declares it yet.
extern const XTypeDescriptor * DT_REQUIRED_MESSAGE_HEADER;

static const TypeDescriptorField MailSendMessage_Fields[3] = {  // line 31
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(MailSendMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(MailSendMessage, nToAccount), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(MailSendMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_MailSendMessage("MailSendMessage", 0, MailSendMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 34
const XTypeDescriptor * DT_MAIL_SEND_MESSAGE = &TypeDescriptor_MailSendMessage;

static const TypeDescriptorField MailSentResponseMessage_Fields[4] = {  // line 40
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(MailSentResponseMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(MailSentResponseMessage, idMail), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(MailSentResponseMessage, idErr), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(MailSentResponseMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_MailSentResponseMessage("MailSentResponseMessage", 0, MailSentResponseMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 44
const XTypeDescriptor * DT_MAIL_SENT_RESPONSE_MESSAGE = &TypeDescriptor_MailSentResponseMessage;

void ActorInventoryMailRecord::Import(const D3::Items::Mail & tMail)  // line 79
{
    tAccountTo = (uint32)tMail.account_to().id_low();
    tMailId = tMail.mail_id();
    tAccountFrom = (uint32)tMail.account_from().id_low();
    szTitle.assign(tMail.title().c_str(), tMail.title().size());
    szBody.assign(tMail.body().c_str(), tMail.body().size());
    sendTime = tMail.send_time();
    eStatus = (MailStatus)tMail.status();
    if (tMail.has_expire_at()) {
        tExpireTime = tMail.expire_at();
    } else {
        tExpireTime = 0;
    }
    attachments = tMail.attachments();
    uPartition = tMail.partition_id();
    uSeasonCreated = tMail.season_created();
    uFlags = tMail.flags();
}
