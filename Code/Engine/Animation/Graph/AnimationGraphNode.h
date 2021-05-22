#pragma once

#include "AnimationGraphContexts.h"
#include "Engine/Animation/AnimationSyncTrack.h"
#include "Engine/Animation/AnimationTarget.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationClip;
}

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class KRG_ENGINE_ANIMATION_API GraphNode
    {
        friend class AnimationNode;
        friend class ValueNode;

    public:

        // This is the base for each node's individual settings
        // The settings are all shared for all graph instances since they are immutable, the nodes themselves contain the actual graph state
        struct KRG_ENGINE_ANIMATION_API Settings : public IRegisteredType
        {
            KRG_REGISTER_TYPE( Settings );

            enum class InitOptions
            {
                CreateNodeAndSetPointers,
                OnlySetPointers
            };

        protected:

            template<typename T>
            KRG_FORCE_INLINE static void SetNodePtrFromIndex( TVector<GraphNode*> const& nodePtrs, NodeIndex nodeIdx, T*& pTargetPtr )
            {
                KRG_ASSERT( nodeIdx >= 0 && nodeIdx < nodePtrs.size() );
                pTargetPtr = static_cast<T*>( nodePtrs[nodeIdx] );
            }

            template<typename T>
            KRG_FORCE_INLINE static void SetNodePtrFromIndex( TVector<GraphNode*> const& nodePtrs, NodeIndex nodeIdx, T const*& pTargetPtr )
            {
                KRG_ASSERT( nodeIdx >= 0 && nodeIdx < nodePtrs.size() );
                pTargetPtr = static_cast<T const*>( nodePtrs[nodeIdx] );
            }

            template<typename T>
            KRG_FORCE_INLINE static void SetOptionalNodePtrFromIndex( TVector<GraphNode*> const& nodePtrs, NodeIndex nodeIdx, T*& pTargetPtr )
            {
                if ( nodeIdx == InvalidIndex )
                {
                    pTargetPtr = nullptr;
                }
                else
                {
                    KRG_ASSERT( nodeIdx >= 0 && nodeIdx < nodePtrs.size() );
                    pTargetPtr = static_cast<T*>( nodePtrs[nodeIdx] );
                }
            }

            template<typename T>
            KRG_FORCE_INLINE static void SetOptionalNodePtrFromIndex( TVector<GraphNode*> const& nodePtrs, NodeIndex nodeIdx, T const*& pTargetPtr )
            {
                if ( nodeIdx == InvalidIndex )
                {
                    pTargetPtr = nullptr;
                }
                else
                {
                    KRG_ASSERT( nodeIdx >= 0 && nodeIdx < nodePtrs.size() );
                    pTargetPtr = static_cast<T const*>( nodePtrs[nodeIdx] );
                }
            }

        public:

            virtual ~Settings() = default;

            // Factory method, will create the node instance and set all necessary node ptrs
            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const = 0;

            // Serialization methods
            virtual void Load( cereal::BinaryInputArchive& archive ) { archive( m_nodeIdx, m_nodeID ); }
            virtual void Save( cereal::BinaryOutputArchive& archive ) const { archive( m_nodeIdx, m_nodeID ); }

        protected:

            template<typename T>
            KRG_FORCE_INLINE T* CreateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
            {
                T* pNode = static_cast<T*>( nodePtrs[m_nodeIdx] );

                if ( options == InitOptions::CreateNodeAndSetPointers )
                {
                    new ( pNode ) T();
                    pNode->m_pSettings = this;
                }

                return pNode;
            }

        public:

            NodeIndex                           m_nodeIdx = InvalidIndex; // The index of this node in the graph, we currently only support graphs with max of 32k nodes
            StringID                            m_nodeID;
        };

    public:

        // Node construction/destruction
        //-------------------------------------------------------------------------

        GraphNode() = default;
        virtual ~GraphNode();

        // Node state management
        virtual bool IsValid() const { return true; }
        inline bool IsInitialized() const { return m_initializationCount > 0; }
        virtual void Initialize( GraphContext& cContext );
        void Shutdown( GraphContext& context );

        // Node flags
        inline NodeIndex GetNodeIdx() const { return m_pSettings->m_nodeIdx; }

        // Update and activity tracking
        KRG_FORCE_INLINE bool IsNodeActive( GraphContext& context ) const { return m_lastUpdateID == context.m_updateID; }
        void MarkNodeActive( GraphContext& context );

    protected:

        virtual void InitializeInternal( GraphContext& context );
        virtual void ShutdownInternal( GraphContext& context );

        template<typename T>
        KRG_FORCE_INLINE typename T::Settings const* GetSettings() const
        {
            return static_cast<typename T::Settings const*>( m_pSettings );
        }

    private:

        Settings const*                 m_pSettings = nullptr;
        uint32                          m_lastUpdateID = 0xFFFFFFFF;
        uint32                          m_initializationCount = 0;
    };

    //-------------------------------------------------------------------------
    // Animation Nodes
    //-------------------------------------------------------------------------

    struct UpdateResult
    {
        KRG_FORCE_INLINE bool HasRegisteredTasks() const { return m_taskIdx != InvalidIndex; }

    public:

        TaskIndex                   m_taskIdx = InvalidIndex;
        Transform                   m_rootMotionDelta = Transform::Identity;
        SampledEventRange           m_sampledEventRange;
    };

    class KRG_ENGINE_ANIMATION_API AnimationNode : public GraphNode
    {

    public:

        // Get internal animation state
        virtual SyncTrack const& GetSyncTrack() const = 0;
        inline int32 GetLoopCount() const { return m_loopCount; }
        inline Percentage const& GetPreviousTime() const { return m_previousTime; }
        inline Percentage const& GetCurrentTime() const { return m_currentTime; }
        inline Seconds GetDuration() const { return m_duration; }

        // Initialize an animation node with a specific start time
        void Initialize( GraphContext& context, SyncTrackTime const& initialTime = SyncTrackTime() );
        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime );

        // Unsynchronized update
        virtual UpdateResult Update( GraphContext& context ) = 0;

        // Synchronized update
        virtual UpdateResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) = 0;

        // Deactivate a previous active branch, this is needed when trigger transitions
        virtual void DeactivateBranch( GraphContext& context ) { KRG_ASSERT( context.m_branchState == BranchState::Inactive && IsNodeActive( context ) ); }

    private:

        virtual void Initialize( GraphContext& context ) override final { Initialize( context, SyncTrackTime() ); }
        virtual void InitializeInternal( GraphContext& context ) override final { Initialize( context, SyncTrackTime() ); }

    protected:

        int32                           m_loopCount = 0;
        Seconds                         m_duration = 0.0f;
        Percentage                      m_currentTime = 0.0f;       // Clamped percentage over the duration
        Percentage                      m_previousTime = 0.0f;      // Clamped percentage over the duration
    };

    //-------------------------------------------------------------------------

    // An interface to directly access a selected animation, this is needed to ensure certain animation nodes only operate on animations directly
    class KRG_ENGINE_ANIMATION_API AnimationClipReferenceNode : public AnimationNode
    {
    public:

        virtual AnimationClip const* GetAnimation() const = 0;
        virtual void DisableRootMotionSampling() = 0;
        virtual bool IsLooping() const = 0;
    };

    //-------------------------------------------------------------------------
    // Value Nodes
    //-------------------------------------------------------------------------

    enum class ValueNodeType
    {
        Unknown = 0,
        Bool,
        ID,
        Int,
        Float,
        Vector,
        Target,
        BoneMask,
    };

    template<typename T> struct ValueTypeValidation { static ValueNodeType const Type = ValueNodeType::Unknown; };
    template<> struct ValueTypeValidation<bool> { static ValueNodeType const Type = ValueNodeType::Bool; };
    template<> struct ValueTypeValidation<StringID> { static ValueNodeType const Type = ValueNodeType::ID; };
    template<> struct ValueTypeValidation<int32> { static ValueNodeType const Type = ValueNodeType::Int; };
    template<> struct ValueTypeValidation<float> { static ValueNodeType const Type = ValueNodeType::Float; };
    template<> struct ValueTypeValidation<Vector> { static ValueNodeType const Type = ValueNodeType::Vector; };
    template<> struct ValueTypeValidation<Target> { static ValueNodeType const Type = ValueNodeType::Target; };
    template<> struct ValueTypeValidation<BoneMask const*> { static ValueNodeType const Type = ValueNodeType::BoneMask; };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ValueNode : public GraphNode
    {

    public:

        virtual ValueNodeType GetValueType() const = 0;

        template<typename T>
        KRG_FORCE_INLINE T GetValue( GraphContext& context )
        {
            KRG_ASSERT( ValueTypeValidation<T>::Type == GetValueType() );
            T Value;
            GetValueInternal( context, &Value );
            return Value;
        }

        template<typename T>
        KRG_FORCE_INLINE void SetValue( GraphContext& context, T const& outValue )
        {
            KRG_ASSERT( ValueTypeValidation<T>::Type == GetValueType() );
            SetValueInternal( context, &outValue );
        }

    protected:

        KRG_FORCE_INLINE bool IsUpToDate( GraphContext& context ) const { return IsNodeActive( context ); }
        KRG_FORCE_INLINE void MarkAsUpdated( GraphContext& context ) { MarkNodeActive( context ); }

    protected:

        virtual void GetValueInternal( GraphContext& context, void* pValue ) = 0;
        virtual void SetValueInternal( GraphContext& context, void const* pValue ) { KRG_ASSERT( false ); };
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ValueNodeBool : public ValueNode
    {
        virtual ValueNodeType GetValueType() const final { return ValueNodeType::Bool; }
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ValueNodeID : public ValueNode
    {
        virtual ValueNodeType GetValueType() const final { return ValueNodeType::ID; }
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ValueNodeInt : public ValueNode
    {
        virtual ValueNodeType GetValueType() const final { return ValueNodeType::Int; }
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ValueNodeFloat : public ValueNode
    {
        virtual ValueNodeType GetValueType() const final { return ValueNodeType::Float; }
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ValueNodeVector : public ValueNode
    {
        virtual ValueNodeType GetValueType() const final { return ValueNodeType::Vector; }
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ValueNodeTarget : public ValueNode
    {
        virtual ValueNodeType GetValueType() const final { return ValueNodeType::Target; }
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ValueNodeBoneMask : public ValueNode
    {
        virtual ValueNodeType GetValueType() const final { return ValueNodeType::BoneMask; }
    };
}

//-------------------------------------------------------------------------

#define KRG_SERIALIZE_GRAPHNODESETTINGS( BaseClassTypename, ... ) \
virtual void Load( cereal::BinaryInputArchive& archive ) override { BaseClassTypename::Load( archive ); archive( __VA_ARGS__ ); }\
virtual void Save( cereal::BinaryOutputArchive& archive ) const override { BaseClassTypename::Save( archive ); archive( __VA_ARGS__ ); }
