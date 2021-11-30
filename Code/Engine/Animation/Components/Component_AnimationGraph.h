#pragma once

#include "Component_Animation.h"
#include "Engine/Animation/Graph/AnimationGraphResources.h"
#include "Engine/Animation/Graph/AnimationGraphInstance.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class KRG_ENGINE_ANIMATION_API AnimationGraphComponent final : public AnimationComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( AnimationGraphComponent );

    public:

        inline AnimationGraphComponent() = default;
        inline AnimationGraphComponent( StringID name ) : AnimationComponent( name ) {}

        //-------------------------------------------------------------------------

        virtual Skeleton const* GetSkeleton() const override;
        virtual Pose const* GetPose() const override { return m_pPose; }
        virtual void PrePhysicsUpdate( Seconds deltaTime, Transform const& characterTransform ) override;
        virtual void PostPhysicsUpdate( Seconds deltaTime, Transform const& characterTransform ) override;

        //-------------------------------------------------------------------------

        // This function will change the graph and data-set used! Note: this can only be called for unloaded components
        void SetGraphVariation( ResourceID graphResourceID );

        // Debug Interface
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        inline bool IsNodeActive( NodeIndex nodeIdx ) const
        {
            KRG_ASSERT( m_pGraphInstance != nullptr );
            return m_pGraphInstance->IsNodeActive( const_cast<Graph::GraphContext&>( m_graphContext ), nodeIdx );
        }

        inline PoseNodeDebugInfo GetPoseNodeDebugInfo( NodeIndex nodeIdx ) const
        {
            KRG_ASSERT( m_pGraphInstance != nullptr );
            return m_pGraphInstance->GetPoseNodeDebugInfo( const_cast<Graph::GraphContext&>( m_graphContext ), nodeIdx );
        }
        #endif

    protected:

        virtual void Initialize() override;
        virtual void Shutdown() override;

    private:

        KRG_EXPOSE TResourcePtr<AnimationGraphVariation>        m_pGraphVariation = nullptr;

        Graph::GraphInstance*                                   m_pGraphInstance = nullptr;
        Graph::TaskSystem*                                      m_pTaskSystem = nullptr;
        Graph::GraphContext                                     m_graphContext;
        Pose*                                                   m_pPose = nullptr;
    };
}