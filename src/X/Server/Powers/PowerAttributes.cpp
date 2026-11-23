// X/Server/Powers/PowerAttributes.cpp: attribute accessors over a running power.
// A power owns its own FastAttribGroup (Power::idFastAttrib), so each accessor is
// one PowerGet followed by one group read.

#include "../../Common/Attributes/FastAttributeStructs.h"
#include "Powers.h"

// X/Common/Attributes/FastAttribute.cpp
int32 FastAttribGetValueInt(AttribGroupID idAttribGroup, FastAttribKey tKey);
float FastAttribGetValueFloat(AttribGroupID idAttribGroup, FastAttribKey tKey);

float PowerAttributesGetFloat(PowerID idPower, FastAttribKey tKey)
{
    Power * ptPower = PowerGet(idPower);

    return FastAttribGetValueFloat(ptPower->idFastAttrib, tKey);
}

int32 PowerAttributesGetInt(const PowerID idPower, FastAttribKey tKey)
{
    Power * ptPower = PowerGet(idPower);

    return FastAttribGetValueInt(ptPower->idFastAttrib, tKey);
}
