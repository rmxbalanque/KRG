#include "AnimationToolkit_AnimationEditorTool.h"
#include "Engine/Animation/Components/AnimationPlayerComponent.h"
#include "System/Resource/ResourceSystem.h"
#include "System/DevTools/CommonWidgets/InterfaceHelpers.h"
#include "System/Entity/EntityWorld.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::AnimationTools
{
    class AnimationTrackEditor final : public TimelineEditor
    {
    public:

        AnimationTrackEditor( Model& inModel, AnimationClip const* pAnimation )
            : TimelineEditor( TRange<int32>( 0, pAnimation->GetNumFrames() ) )
            , m_model( inModel )
            , m_animationID( pAnimation->GetResourceID() )
            , m_pAnimation( pAnimation )
        {}

        inline ResourceID const& GetAnimationID() const { return m_animationID; }

        void Update( UpdateContext const& context )
        {
            auto pPreviewAnimationComponent = m_model.GetPreviewAnimComponent();
            KRG_ASSERT( pPreviewAnimationComponent != nullptr );

            // Handle play state changes
            //-------------------------------------------------------------------------

            if ( IsPlaying() && pPreviewAnimationComponent->IsPosed() )
            {
                pPreviewAnimationComponent->SetPlayMode( IsLoopingEnabled() ? AnimationPlayerComponent::PlayMode::Loop : AnimationPlayerComponent::PlayMode::PlayOnce );
                pPreviewAnimationComponent->SetAnimTime( GetPlayheadPositionAsPercentage() );
            }
            else if ( IsPaused() && !pPreviewAnimationComponent->IsPosed() )
            {
                pPreviewAnimationComponent->SetPlayMode( AnimationPlayerComponent::PlayMode::Posed );
            }

            // Draw track editor and manage playhead data
            //-------------------------------------------------------------------------

            // If we are playing, then update the playhead position
            if ( IsPlaying() )
            {
                SetPlayheadPositionAsPercentage( pPreviewAnimationComponent->GetAnimTime() );
            }

            Draw();

            // Is we are paused, then update the animation component pose
            if ( IsPaused() )
            {
                pPreviewAnimationComponent->SetAnimTime( GetPlayheadPositionAsPercentage() );
            }
        }

    private:

        // This is separate since the animation ptr can be invalidated externally
        ResourceID                              m_animationID;
        AnimationClip const*                    m_pAnimation = nullptr;
        Model&                                  m_model;
    };
}

//-------------------------------------------------------------------------

namespace KRG::Animation::AnimationTools
{
    AnimationEditor::~AnimationEditor()
    {
        KRG_ASSERT( m_trackEditors.empty() );
    }

    void AnimationEditor::Initialize( UpdateContext const& context )
    {
        TEditorTool<Model>::Initialize( context );
    }

    void AnimationEditor::Shutdown( UpdateContext const& context )
    {
        for ( auto& pTrackEditor : m_trackEditors )
        {
            KRG::Delete( pTrackEditor );
        }

        m_trackEditors.clear();

        TEditorTool<Model>::Shutdown( context );
    }

    void AnimationEditor::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        // Manage preview states
        //-------------------------------------------------------------------------

        auto const& openResources = m_model.GetLoadedResources();
        for( int32 i = (int) m_trackEditors.size() - 1; i >= 0; i-- )
        {
            auto const SearchPredicate = [] ( Resource::ResourcePtr const& resourcePtr, ResourceID const& resourceID ) { return resourcePtr.GetResourceID() == resourceID; };
            auto const foundResourceIter = eastl::find( openResources.begin(), openResources.end(), m_trackEditors[i]->GetAnimationID(), SearchPredicate );
            if ( foundResourceIter == openResources.end() )
            {
                KRG::Delete( m_trackEditors[i] );
                m_trackEditors.erase_unsorted( m_trackEditors.begin() + i );
            }
        }

        // Draw UI
        //-------------------------------------------------------------------------
        
        // Prevent anything docking into this window, and remove the tab-bar
        ImGuiWindowClass windowClass;
        windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoDockingOverOther | ImGuiDockNodeFlags_NoDockingSplitOther | ImGuiDockNodeFlags_NoTabBar;
        ImGui::SetNextWindowClass( &windowClass );

        // Create timeline window
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 2, 2 ) );
        int32 const windowFlags = ImGuiWindowFlags_NoTitleBar;
        if ( ImGui::Begin( "Animation Timeline", nullptr, windowFlags ) )
        {
            if ( openResources.empty() )
            {
                ImGui::Text( "No previewed items" );
            }
            else // Draw tab bar and timeline hosts
            {
                TInlineVector<ResourceID, 4> resourcesToClose;
                int32 const tabBarFlags = ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton;
                if ( ImGui::BeginTabBar( "OpenResourcesTabBar", tabBarFlags ) )
                {
                    for ( auto& resourcePtr : openResources )
                    {
                        bool isOpened = true;
                        if ( ImGui::BeginTabItem( resourcePtr.GetResourceID().c_str(), &isOpened, ImGuiTabItemFlags_None ) )
                        {
                            m_model.SetActiveResource( resourcePtr.GetResourceID() );

                            if ( m_model.IsActiveResourceLoading() )
                            {
                                ImGui::Text( "Loading:" );
                                ImGui::SameLine();
                                ImGuiX::DrawSpinner( "Loading" );
                            }
                            else if ( m_model.GetActiveResourcePtr().HasLoadingFailed() )
                            {
                                ImGui::Text( "Loading Failed: %s", m_model.GetActiveResource().c_str() );
                            }
                            else // Draw animation info and controls
                            {
                                DrawAnimationInfo( context );
                                DrawAnimationTimeline( context );
                            }
                            ImGui::EndTabItem();
                        }

                        if ( !isOpened )
                        {
                            resourcesToClose.emplace_back( resourcePtr.GetResourceID() );
                        }
                    }
                    ImGui::EndTabBar();
                }

                for ( auto const& resourceID : resourcesToClose )
                {
                    m_model.UnloadResource( resourceID );
                }
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void AnimationEditor::DrawAnimationInfo( UpdateContext const& context )
    {
        KRG_ASSERT( m_model.HasActiveResource() );
        TResourcePtr<AnimationClip> const pAnimation( m_model.GetActiveResourcePtr() );

        ImGui::AlignTextToFramePadding();

        //-------------------------------------------------------------------------

        ImGui::TextColored( Colors::LightPink.ToFloat4(), "Average Velocity: %.2fm/s", 4.0f );

        ImGui::SameLine( 0, 15 );
        ImGui::TextColored( Colors::LightPink.ToFloat4(), "Distance Covered: %.2fm", 10 );

        ImGui::SameLine( 0, 15 );
        ImGui::TextColored( Colors::LightPink.ToFloat4(), "Frame: %.2f / %d", 0, pAnimation->GetNumFrames() );

        ImGui::SameLine( 0, 15 );
        ImGui::TextColored( Colors::LightPink.ToFloat4(), "Time: %.2fs / %.2fs", 0, pAnimation->GetDuration() );
    }

    void AnimationEditor::DrawAnimationTimeline( UpdateContext const& context )
    {
        KRG_ASSERT( m_model.HasActiveResource() );
        TResourcePtr<AnimationClip> const pAnimation( m_model.GetActiveResourcePtr() );

        //-------------------------------------------------------------------------

        AnimationTrackEditor* pTrackEditor = nullptr;

        auto const SearchPredicate = [] ( AnimationTrackEditor*& pTrackEditor, ResourceID const& resourceID ) { return pTrackEditor->GetAnimationID() == resourceID; };
        auto foundStateIter = eastl::find( m_trackEditors.begin(), m_trackEditors.end(), pAnimation->GetResourceID(), SearchPredicate );
        if ( foundStateIter != m_trackEditors.end() )
        {
            pTrackEditor = *foundStateIter;
        }

        //-------------------------------------------------------------------------

        if( pTrackEditor == nullptr )
        {
            pTrackEditor = m_trackEditors.emplace_back( KRG::New<AnimationTrackEditor>( m_model, pAnimation.GetPtr() ) );
        }

        //-------------------------------------------------------------------------

        pTrackEditor->Update( context );
    }
}