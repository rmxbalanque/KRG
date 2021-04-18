#include "System/TypeSystem/TypeRegistry.h"
#include "Applications/Shared/ApplicationGlobalState.h"
#include "Applications/Shared/_AutoGenerated/EngineTypeRegistration.h"
#include "_AutoGenerated/ToolsTypeRegistration.h"
#include "Engine/Physics/PhysicsLayers.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "Tools/Core/TypeSystem/Serialization/TypeInstanceModelReader.h"
#include "Tools/Core/TypeSystem/TypeInstanceModel.h"

//-------------------------------------------------------------------------

using namespace KRG;
using namespace KRG::Physics;

//-------------------------------------------------------------------------

struct Test
{
    inline void SetPreviewedMeshPath( DataPath const& previewMeshPath )
    {
        KRG_ASSERT( previewMeshPath.IsValid() );
        m_dp = previewMeshPath;
    }

    DataPath m_dp;
};

int main( int argc, char *argv[] )
{
    {
        KRG::ApplicationGlobalState State;
        TypeSystem::TypeRegistry typeRegistry;
        AutoGenerated::Tools::RegisterTypes( typeRegistry );

        //-------------------------------------------------------------------------

        TSingleUserEventInternal<void( int )> e;

        //-------------------------------------------------------------------------

        AutoGenerated::Tools::UnregisterTypes( typeRegistry );
    }

    return 0;
}
