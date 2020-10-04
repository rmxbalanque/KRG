#pragma once
#include "../../_Module/API.h"
#include "Tools/Core/TypeSystem/DynamicTypeInstance.h"
#include "System/Core/Core.h"
#include "System/TypeSystem/TypeInfo.h"
#include <QWidget>

//-------------------------------------------------------------------------

class QScrollArea;
class QVBoxLayout;

//-------------------------------------------------------------------------

namespace KRG
{
    struct EditorContext;
    class KSearchBoxWidget;
    class KCollapsibleArrowWidget;

    //-------------------------------------------------------------------------

    namespace PropertyGrid
    {
        class PropertyEditor;
    }

    //-------------------------------------------------------------------------

    class KRG_TOOLS_UI_API PropertyGridWidget final : public QWidget
    {
        Q_OBJECT

    public:

        PropertyGridWidget( QWidget* pParent = nullptr );

        void SetTypeInstanceToEdit( DynamicTypeInstance* pTypeInstance );
        void ClearEditedTypeInstance() { SetTypeInstanceToEdit( nullptr ); }

    private:

        void GenerateGridWidgets();

    private:

        DynamicTypeInstance*                            m_pTypeInstance = nullptr;
        QWidget*                                        m_pDefaultMessage = nullptr;
        QScrollArea*                                    m_pScrollArea = nullptr;
        QVBoxLayout*                                    m_pContentLayout = nullptr;
        TVector<PropertyGrid::PropertyEditor*>          m_propertyEditors;
    };
}