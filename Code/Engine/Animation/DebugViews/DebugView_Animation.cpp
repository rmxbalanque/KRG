#include "DebugView_Animation.h"
#include "Engine/Animation/Systems/WorldSystem_Animation.h"
#include "Engine/Animation/Components/Component_AnimationGraph.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/Core/Math/MathStringHelpers.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Animation
{
    AnimationDebugView::AnimationDebugView()
    {
        m_menus.emplace_back( DebugMenu( "Animation", [this] ( EntityUpdateContext const& context ) { DrawMenu( context ); } ) );
    }

    void AnimationDebugView::Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld )
    {
        m_pWorld = pWorld;
        m_pAnimationWorldSystem = pWorld->GetWorldSystem<AnimationWorldSystem>();
    }

    void AnimationDebugView::Shutdown()
    {
        m_pAnimationWorldSystem = nullptr;
        m_pWorld = nullptr;
    }

    void AnimationDebugView::DrawWindows( EntityUpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        for ( int32 i = (int32) m_activeControlParameterDebuggers.size() - 1; i >= 0; i-- )
        {
            auto ppGraphComponent = m_pAnimationWorldSystem->m_graphComponents.FindItem( m_activeControlParameterDebuggers[i] );
            if ( ppGraphComponent != nullptr )
            {
                if ( !DrawControlParameters( *ppGraphComponent ) )
                {
                    m_activeControlParameterDebuggers.erase_unsorted( m_activeControlParameterDebuggers.begin() + i );
                }
            }
            else // Erase invalid component ID
            {
                m_activeControlParameterDebuggers.erase_unsorted( m_activeControlParameterDebuggers.begin() + i );
            }
        }
    }

    void AnimationDebugView::DrawMenu( EntityUpdateContext const& context )
    {
        if ( ImGui::BeginMenu( "Control Parameter Debug" ) )
        {
            for ( AnimationGraphComponent* pGraphComponent : m_pAnimationWorldSystem->m_graphComponents )
            {
                EntityID const entityID = pGraphComponent->GetEntityID();
                auto pEntity = m_pWorld->FindEntity( entityID );
                KRG_ASSERT( pEntity != nullptr );

                if( ImGui::MenuItem( pEntity->GetName().c_str() ) )
                {
                    if ( !VectorContains( m_activeControlParameterDebuggers, pGraphComponent->GetID() ) )
                    {
                        m_activeControlParameterDebuggers.emplace_back( pGraphComponent->GetID() );
                    }
                }
            }

            ImGui::EndMenu();
        }
    }

    bool AnimationDebugView::DrawControlParameters( AnimationGraphComponent* pGraphComponent )
    {
        EntityID const entityID = pGraphComponent->GetEntityID();
        auto pEntity = m_pWorld->FindEntity( entityID );
        KRG_ASSERT( pEntity != nullptr );
        InlineString<128> const title( InlineString<128>::CtorSprintf(), "Control Parameters: %s (%s)", pGraphComponent->GetName().c_str(), pEntity->GetName().c_str() );

        bool shouldDraw = true;
        ImGui::SetNextWindowSize( ImVec2( 600, 700 ), ImGuiCond_FirstUseEver );
        if ( ImGui::Begin( title.c_str(), &shouldDraw, ImGuiWindowFlags_NoSavedSettings ) )
        {
            if ( ImGui::BeginTable( "OverlayActionsTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable ) )
            {
                ImGui::TableSetupColumn( "Parameter", ImGuiTableColumnFlags_WidthStretch, 0.5f );
                ImGui::TableSetupColumn( "Value", ImGuiTableColumnFlags_WidthStretch );
                ImGui::TableHeadersRow();

                int32 const numControlParameters = pGraphComponent->m_pGraphInstance->GetNumControlParameters();
                for ( NodeIndex i = 0; i < numControlParameters; i++ )
                {
                    StringID const nodeID = pGraphComponent->m_pGraphInstance->GetControlParameterID( i );
                    Graph::ValueType const valueType = pGraphComponent->m_pGraphInstance->GetControlParameterType( i );

                    InlineString<128> stringValue;
                    switch ( valueType )
                    {
                        case Graph::ValueType::Bool:
                        {
                            if ( pGraphComponent->m_pGraphInstance->GetControlParameterValue<bool>( pGraphComponent->m_graphContext, i ) )
                            {
                                stringValue = "True";
                            }
                            else
                            {
                                stringValue = "False";
                            }
                        }
                        break;

                        case Graph::ValueType::ID:
                        {
                            StringID const value = pGraphComponent->m_pGraphInstance->GetControlParameterValue<StringID>( pGraphComponent->m_graphContext, i );
                            if ( value.IsValid() )
                            {
                                stringValue.sprintf( "%s", value.c_str() );
                            }
                            else
                            {
                                stringValue = "Unset";
                            }
                        }
                        break;

                        case Graph::ValueType::Int:
                        {
                            stringValue.sprintf( "%d", pGraphComponent->m_pGraphInstance->GetControlParameterValue<int32>( pGraphComponent->m_graphContext, i ) );
                        }
                        break;

                        case Graph::ValueType::Float:
                        {
                            stringValue.sprintf( "%.3f", pGraphComponent->m_pGraphInstance->GetControlParameterValue<float>( pGraphComponent->m_graphContext, i ) );
                        }
                        break;

                        case Graph::ValueType::Vector:
                        {
                            Vector const value = pGraphComponent->m_pGraphInstance->GetControlParameterValue<Vector>( pGraphComponent->m_graphContext, i );
                            stringValue = Math::ToString( value );
                        }
                        break;

                        case Graph::ValueType::Target:
                        {
                            Target const value = pGraphComponent->m_pGraphInstance->GetControlParameterValue<Target>( pGraphComponent->m_graphContext, i );
                            if ( !value.IsTargetSet() )
                            {
                                stringValue = "Unset";
                            }
                            else if ( value.IsBoneTarget() )
                            {
                                stringValue.sprintf( "Bone: %s", value.GetBoneID().c_str() );
                            }
                            else
                            {
                                stringValue = "TODO";
                            }
                        }
                        break;

                        default:
                        {
                            KRG_UNREACHABLE_CODE();
                        }
                        break;
                    }

                    //-------------------------------------------------------------------------

                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex( 0 );
                    ImGui::TextColored( ImGuiX::ConvertColor( GetColorForValueType( valueType ) ), "%s", nodeID.c_str() );

                    ImGui::TableSetColumnIndex( 1 );
                    ImGui::Text( stringValue.c_str() );
                }

                ImGui::EndTable();
            }
        }
        ImGui::End();

        return shouldDraw;
    }

}
#endif