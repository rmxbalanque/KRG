#pragma once

#include "AnimationGraphContexts.h"
#include "Engine/Animation/AnimationSyncTrack.h"
#include "Engine/Animation/AnimationTarget.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationClip;
}

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class GraphNode
    {
        friend class AnimationNode;
        friend class ValueNode;

    public:

        // This is the base for each node's individual settings
        // The settings are all shared for all graph instances since they are immutable, the nodes themselves contain the actual graph state
        struct Settings
        {
            NodeIndex                           m_nodeIdx = InvalidIndex; // The index of this node in the graph, we currently only support graphs with max of 32k nodes
            StringID                            m_nodeID;
        };

    public:

        // Node construction/destruction
        //-------------------------------------------------------------------------

        GraphNode() = default;
        virtual ~GraphNode();

        // Called once all the nodes are allocated and constructed for a graph instance, used to convert node indices to ptrs
        virtual void OnConstruct( Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet );

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

        template<typename T>
        KRG_FORCE_INLINE void SetNodePtrFromIndex( TVector<GraphNode*> const& nodePtrs, NodeIndex nodeIdx, T*& pTargetPtr )
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
        KRG_FORCE_INLINE void SetNodePtrFromIndex( TVector<GraphNode*> const& nodePtrs, NodeIndex nodeIdx, T const*& pTargetPtr )
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

    class AnimationNode : public GraphNode
    {

    public:

        // Get internal animation state
        virtual SyncTrack const& GetSyncTrack() const = 0;
        inline int32 GetLoopCount() const { return m_loopCount; }
        inline Percentage const& GetPreviousTime() const { return m_previousTime; }
        inline Percentage const& GetCurrentTime() const { return m_currentTime; }
        inline Seconds GetDuration() const { return m_duration; }
        virtual TRange<Percentage> GetUpdateRange() const { return TRange<Percentage>( m_previousTime, m_currentTime ); }

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
    class AnimationClipReferenceNode : public AnimationNode
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

    class ValueNode : public GraphNode
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

    class ValueNodeBool : public ValueNode
    {
        virtual ValueNodeType GetValueType() const final { return ValueNodeType::Bool; }
    };

    //-------------------------------------------------------------------------

    class ValueNodeID : public ValueNode
    {
        virtual ValueNodeType GetValueType() const final { return ValueNodeType::ID; }
    };

    //-------------------------------------------------------------------------

    class ValueNodeInt : public ValueNode
    {
        virtual ValueNodeType GetValueType() const final { return ValueNodeType::Int; }
    };

    //-------------------------------------------------------------------------

    class ValueNodeFloat : public ValueNode
    {
        virtual ValueNodeType GetValueType() const final { return ValueNodeType::Float; }
    };

    //-------------------------------------------------------------------------

    class ValueNodeVector : public ValueNode
    {
        virtual ValueNodeType GetValueType() const final { return ValueNodeType::Vector; }
    };

    //-------------------------------------------------------------------------

    class ValueNodeTarget : public ValueNode
    {
        virtual ValueNodeType GetValueType() const final { return ValueNodeType::Target; }
    };

    //-------------------------------------------------------------------------

    class ValueNodeBoneMask : public ValueNode
    {
        virtual ValueNodeType GetValueType() const final { return ValueNodeType::BoneMask; }
    };
}