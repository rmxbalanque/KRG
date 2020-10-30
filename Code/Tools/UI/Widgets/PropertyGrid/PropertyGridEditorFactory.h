#pragma once
#include "Tools/Core/TypeSystem/ToolTypeInstance.h"
#include "PropertyGridEditorWidget.h"

//-------------------------------------------------------------------------

class QWidget;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace PropertyGrid
    {
        PropertyEditor* CreateEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance );
    }
}
