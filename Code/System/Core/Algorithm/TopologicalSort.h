#pragma once

#include "System/Core/_Module/API.h"
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_SYSTEM_CORE_API TopologicalSorter
    {
    public:

        // This is a utility class to enable sorting of types in a topological manner
        struct Node
        {
            enum class Mark
            {
                None,
                Permanent,
                Temporary,
            };

            Node() : m_mark( Mark::None ), m_ID( -1 ) {}
            Node( int32 ID ) : m_mark( Mark::None ), m_ID( ID ) {}

            int32             m_ID;           // Some way to identify the actual data this refers to
            TVector<Node*>  m_children;     // The children of this node
            Mark            m_mark;         // Marking mechanism for algorithm
        };

        //-------------------------------------------------------------------------

        static bool Sort( TVector<Node>& list );
    };
}