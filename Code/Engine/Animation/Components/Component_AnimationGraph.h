#pragma once

#include "Component_Animation.h"
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Resources.h"
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Instance.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class KRG_ENGINE_ANIMATION_API AnimationGraphComponent final : public AnimationComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( AnimationGraphComponent );

        friend class AnimationDebugView;

    public:

        inline AnimationGraphComponent() = default;
        inline AnimationGraphComponent( StringID name ) : AnimationComponent( name ) {}

        //-------------------------------------------------------------------------

        virtual Skeleton const* GetSkeleton() const override;
        virtual Pose const* GetPose() const override { return m_pPose; }
        virtual void PrePhysicsUpdate( Seconds deltaTime, Transform const& characterTransform, Physics::Scene* pPhysicsScene ) override;
        virtual void PostPhysicsUpdate( Seconds deltaTime, Transform const& characterTransform, Physics::Scene* pPhysicsScene ) override;

        // Get the graph variation ID
        inline ResourceID const& GetGraphVariationID() const { return m_pGraphVariation.GetResourceID(); }

        // This function will change the graph and data-set used! Note: this can only be called for unloaded components
        void SetGraphVariation( ResourceID graphResourceID );

        // Control Parameters
        //-------------------------------------------------------------------------

        template<typename ParameterType>
        void SetControlParameterValue( GraphNodeIndex parameterIdx, ParameterType const& value )
        {
            m_pGraphInstance->SetControlParameterValue( m_graphContext, parameterIdx, value );
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
        #endif

    protected:

        virtual void Initialize() override;
        virtual void Shutdown() override;

    private:

        KRG_EXPOSE TResourcePtr<GraphVariation>                 m_pGraphVariation = nullptr;

        GraphInstance*                                          m_pGraphInstance = nullptr;
        TaskSystem*                                             m_pTaskSystem = nullptr;
        GraphContext                                            m_graphContext;
        Pose*                                                   m_pPose = nullptr;
    };
}