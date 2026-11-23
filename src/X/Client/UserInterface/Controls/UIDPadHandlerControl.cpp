#include "UIDPadHandlerControl.h"

#include "../Console/ConsoleInputUtilities.h"

#include "../../Input/Input.h"
#include "../../../Common/GameSwig.h"

// X/Client/UserInterface/UI.cpp
int32 UI_GetPrimaryUIController();
void UI_RegisterInputListener(UIControl * pControl);

BOOL UIDPadHandlerControl::HandleMessage(const UIMsg & msg, const uintptr dwParam)
{
    switch (msg)
    {
    case UI_INPUT:
    {
        const BufferedInput & tInput = *(const BufferedInput *)dwParam;
        if (UI_GetPrimaryUIController() == tInput.tBufControllerData.nControllerIndex)
        {
            if (tInput.eInputMsg != INPUTMSG_CONTROLLERBUTTONDOWN &&
                tInput.eInputMsg != INPUTMSG_CONTROLLERBUTTONSTILLDOWN &&
                tInput.eInputMsg != INPUTMSG_CONTROLLERBUTTONREPEAT)
            {
                return FALSE;
            }

            if (tInput.tBufControllerData.eInputButton != BUTTON_DPAD_UP &&
                tInput.tBufControllerData.eInputButton != BUTTON_DPAD_DOWN &&
                tInput.tBufControllerData.eInputButton != BUTTON_DPAD_LEFT &&
                tInput.tBufControllerData.eInputButton != BUTTON_DPAD_RIGHT)
            {
                return FALSE;
            }

            static Console::InputSpeedLimiter tSpeedLimiter;
            const int32 SPEED_LIMIT_MS = 250;

            BOOL bIgnoreInput = !tSpeedLimiter.HasWaitTimeExpired(SPEED_LIMIT_MS);
            if (bIgnoreInput)
            {
                return FALSE;
            }

            if (m_pFunctionsPtr != NULL &&
                m_pFunctionsPtr->pfnConsoleButtonDown[tInput.tBufControllerData.eInputButton] != NULL)
            {
                m_pFunctionsPtr->pfnConsoleButtonDown[tInput.tBufControllerData.eInputButton](
                    GetID(), tInput.tBufControllerData.nControllerIndex);
            }

            tSpeedLimiter.UpdateLastInputTime();
            return TRUE;
        }
        break;
    }

    case UI_REGISTER_INPUT_LISTENER:
        UI_RegisterInputListener(this);
        break;

    default:
        break;
    }

    return FALSE;
}
