// Original file: X/Client/Console/WebBrowserNX64.cpp

#include "../../Sigma/RefString.h"
#include "../../../NintendoSDK/Include/nn/web/web_ShowArg.h"

#include "../../../NintendoSDK/Include/nn/nn_Result.h"

// NintendoSDK/Include/nn/web/web_Show.h (header name is a best guess; the
// DWARF keeps ShowWebPage's declaring header out of the reference tree).
namespace nn { namespace web {
Result ShowWebPage(WebPageReturnValue * pWebPageReturn, const ShowWebPageArg & rWebPageArg);
}  // namespace web
}  // namespace nn

namespace Console { namespace WebBrowser {

void Update()
{
}

BOOL NavigateTo(
    const CRefString & sUri,
    int32 nControllerIndex)
{
    nn::web::ShowWebPageArg tWebPageArg(sUri.str());
    nn::web::WebPageReturnValue tWebPageReturn;

    const nn::Result tResult = nn::web::ShowWebPage(&tWebPageReturn, tWebPageArg);

    return tResult.IsSuccess();
}

}  // namespace WebBrowser
}  // namespace Console
