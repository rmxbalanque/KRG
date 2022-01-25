#pragma once

#include "Engine/Core/Entity/EntityComponent.h"
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Resources.h"
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Instance.h"
#include "Engine/Animation/Graph/Animation_RuntimeGraph_RootMotionRecorder.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class KRG_ENGINE_ANIMATION_API AnimationGraphComponent final : public EntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( AnimationGraphComponent );

        friend class AnimationDebugView;

    public:

        inline AnimationGraphComponent() = default;
        inline AnimationGraphComponent( StringID name ) : EntityComponent( name ) {}
        virtual ~AnimationGraphComponent();

        //-------------------------------------------------------------------------

        Skeleton const* GetSkeleton() const;
        Pose const* GetPose() const { return m_pPose; }
        inline Transform const& GetRootMotionDelta() const { return m_rootMotionDelta; }
        inline bool RequiresManualUpdate() const { return m_requiresManualUpdate; }

        // Get the graph variation ID
        inline ResourceID const& GetGraphVariationID() const { return m_pGraphVariation.GetResourceID(); }

        // This function will change the graph and data-set used! Note: this can only be called for unloaded components
        void SetGraphVariation( ResourceID graphResourceID );

        //-------------------------------------------------------------------------

        // This function will evaluate the graph and produce the desired root motion delta for the character
        void EvaluateGraph( Seconds deltaTime, Transform const& characterWorldTransform, Physics::Scene* pPhysicsScene );

        // This function will execute all pre-physics tasks - it assumes that the character has already been moved in the scene, so expects the final transform for this frame
        void ExecutePrePhysicsTasks( Transform const& characterWorldTransform );

        // The function will execute the post-physics tasks (if any)
        void ExecutePostPhysicsTasks();

        // Control Parameters
        //-------------------------------------------------------------------------

        template<typename ParameterType>
        void SetControlParameterValue( GraphNodeIndex parameterIdx, ParameterType const& value )
        {
            m_pGraphInstance->SetControlParameterValue( m_graphContext, parameterIdx, value );
        }

        template<typename ParameterType>
        ParameterType GetControlParameterValue( GraphNodeIndex parameterIdx ) const
        {
            return m_pGraphInstance->GetControlParameterValue<ParameterType>( const_cast<GraphContext&>( m_graphContext ), parameterIdx );
        }

        KRG_FORCE_INLINE GraphNodeIndex GetControlParameterIndex( StringID parameterID ) const
        {
            return m_pGraphInstance->GetControlParameterIndex( parameterID );
        }

        KRG_FORCE_INLINE GraphValueType GetControlParameterValueType( GraphNodeIndex parameterIdx ) const
        {
            return m_pGraphInstance->GetControlParameterType( parameterIdx );
        }

        // Development Interface
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        inline bool IsNodeActive( GraphNodeIndex nodeIdx ) const
        {
            KRG_ASSERT( m_pGraphInstance != nullptr );
            return m_pGraphInstance->IsNodeActive( const_cast<GraphContext&>( m_graphContext ), nodeIdx );
        }

        inline PoseNodeDebugInfo GetPoseNodeDebugInfo( GraphNodeIndex nodeIdx ) const
        {
            KRG_ASSERT( m_pGraphInstance != nullptr );
            return m_pGraphInstance->GetPoseNodeDebugInfo( const_cast<GraphContext&>( m_graphContext ), nodeIdx );
        }

        template<typename T>
        inline T GetRuntimeNodeValue( GraphNodeIndex nodeIdx ) const
        {
            KRG_ASSERT( m_pGraphInstance != nullptr );
            return m_pGraphInstance->GetRuntimeNodeValue<T>( const_cast<GraphContext&>( m_graphContext ), nodeIdx );
        }

        // Task system debug
        inline void SetTaskSystemDebugMode( TaskSystem::DebugMode mode ) { m_pTaskSystem->SetDebugMode( mode ); }
        inline TaskSystem::DebugMode GetTaskSystemDebugMode() const { return m_pTaskSystem->GetDebugMode(); }

        // Root motion debug
        inline void SetRootMotionDebugMode( RootMotionRecorder::DebugMode mode ) { m_graphContext.GetRootMotionActionRecorder()->SetDebugMode(mode); }
        inline RootMotionRecorder::DebugMode GetRootMotionDebugMode() const { return m_graphContext.GetRootMotionActionRecorder()->GetDebugMode(); }

        // Draw all debug visualizations
        void DrawDebug( Drawing::DrawContext& drawingContext );
        #endif

    protected:

        virtual void Initialize() override;
        virtual void Shutdown() override;

    private:

        KRG_EXPOSE TResourcePtr<GraphVariation>                 m_pGraphVariation = nullptr;

        GraphInstance*                                          m_pGraphInstance = nullptr;
        TaskSystem*                                             m_pTaskSystem = nullptr;

        Transform                                               m_rootMotionDelta = Transform::Identity;
        GraphContext                                            m_graphContext;
        Pose*                                                   m_pPose = nullptr;
        KRG_EXPOSE bool                                         m_requiresManualUpdate = false;

        #if KRG_DEVELOPMENT_TOOLS
        RootMotionRecorder*                                     m_pRootMotionActionRecorder = nullptr; // Allows nodes to record root motion operations
        #endif
    };
}