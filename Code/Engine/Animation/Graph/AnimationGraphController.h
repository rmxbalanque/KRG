#pragma once
#include "Engine/Animation/_Module/API.h"
#include "Engine/Animation/Components/Component_AnimationGraph.h"
#include "Engine/Animation/Graph/AnimationGraphCommon.h"
#include "System/Core/Algorithm/Hash.h"
#include "System/Core/Types/StringID.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    //-------------------------------------------------------------------------
    // Animation graph controller
    //-------------------------------------------------------------------------
    // Basic functions to help facilitate communications with animation graphs

    class KRG_ENGINE_ANIMATION_API GraphController
    {
        friend class GraphControllerRegistry;

    protected:

        template<typename ParameterType>
        struct ControlParameter
        {
            friend GraphController;
            ControlParameter( char const* parameterName ) : m_ID( parameterName ) {}

        private:

            StringID                m_ID;
            Graph::NodeIndex        m_index = InvalidIndex;
        };

    public:

        GraphController( AnimationGraphComponent* pGraphComponent ) : m_pGraphComponent( pGraphComponent ) { KRG_ASSERT( m_pGraphComponent != nullptr ); }
        virtual ~GraphController() = default;

        // Get the ID for this controller
        virtual uint32 GetGraphControllerID() const = 0;

        // Optional update that runs before the graph evaluation allowing you to set more parameters, etc...
        virtual void PreGraphUpdate( Seconds deltaTime ) {}

        // Optional update that runs after the graph evaluation allowing you to read events and clear transient state
        virtual void PostGraphUpdate( Seconds deltaTime ) {}

        #if KRG_DEVELOPMENT_TOOLS
        // Returns a friendly name for this controller
        virtual char const* GetName() const = 0;
        #endif

    protected:

        template<typename T>
        void BindControlParameter( ControlParameter<T>& parameter )
        {
            KRG_ASSERT( parameter.m_ID.IsValid() );
            parameter.m_index = m_pGraphComponent->GetControlParameterIndex( parameter.m_ID );
            if ( parameter.m_index != InvalidIndex )
            {
                if ( m_pGraphComponent->GetControlParameterValueType( parameter.m_index ) != ValueTypeValidation<T>::Type )
                {
                    KRG_LOG_WARNING( "Animation", "Failed to bind to control parameter (%s): type mismatch. Controller (%s) and graph (%s)", parameter.m_ID.c_str(), GetName(), m_pGraphComponent->GetGraphVariationID().c_str() );
                    parameter.m_index = InvalidIndex;
                }
            }
            else
            {
                KRG_LOG_WARNING( "Animation", "Failed to bind to control parameter (%s): parameter not found. Controller (%s) and graph (%s)", parameter.m_ID.c_str(), GetName(), m_pGraphComponent->GetGraphVariationID().c_str() );
            }
        }

        // Helper to set control parameter values without caring if they are set or not!
        template<typename T>
        void SetControlParameterValue( ControlParameter<T>& parameter, T const& value )
        {
            if ( parameter.m_index != InvalidIndex )
            {
                m_pGraphComponent->SetControlParameterValue<T>( parameter.m_index, value );
            }
            else
            {
                KRG_LOG_WARNING( "Animation", "Trying to unbound control parameter %s in controller %s", parameter.m_ID.c_str(), GetName() );
            }
        }

    private:

        AnimationGraphComponent* m_pGraphComponent = nullptr;
    };

    //-------------------------------------------------------------------------
    // Simple registry for all graph controllers
    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API GraphControllerRegistry
    {
    public:

        inline bool HasControllers() const { return !m_controllers.empty(); }

        void CreateControllers( AnimationGraphComponent* pGraphComponent );
        void DestroyControllers();

        // Get a specific graph controller
        template<typename T>
        inline T* GetController() const
        {
            for ( auto pController : m_controllers )
            {
                if ( pController->GetGraphControllerID() == T::s_graphControllerID )
                {
                    return static_cast<T*>( pController );
                }
            }
            return nullptr;
        }

    protected:

        virtual void CreateControllersInternal( AnimationGraphComponent* pGraphComponent ) = 0;

    protected:

        TInlineVector<GraphController*, 6>    m_controllers;
    };
}

//-------------------------------------------------------------------------

#define KRG_ANIM_GRAPH_CONTROLLER_ID( TypeName ) \
constexpr static uint32 const s_graphControllerID = Hash::FNV1a::GetHash32( #TypeName ); \
virtual uint32 GetGraphControllerID() const override final { return TypeName::s_graphControllerID; }\
KRG_DEVELOPMENT_TOOLS_LINE_IN_MACRO( char const* GetName() const override final { return #TypeName; } )