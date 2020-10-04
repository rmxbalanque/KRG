#pragma once
#include "Tools/Core/TypeSystem/DynamicPropertyInstance.h"
#include "PropertyGridEditorWidget.h"

//-------------------------------------------------------------------------

class QWidget;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace PropertyGrid
    {
        PropertyEditor* CreateEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance );
    }
}
