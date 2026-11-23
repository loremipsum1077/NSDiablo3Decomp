// Original file: X/Client/LocalizedError.cpp

#include "../Sigma/UnicodeText.h"

// Empty body: the original emits a single return (4 bytes). This error code has
// no localized text, so the report is a no-op.
void ReportLocalizedError_MpqOpenError(XLocale eLocale)
{
    (void)eLocale;
}
