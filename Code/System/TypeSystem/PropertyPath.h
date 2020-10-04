#pragma once

#include "_Module/API.h"
#include "TypeID.h"

//-------------------------------------------------------------------------
// The path to a property within a reflected type
//-------------------------------------------------------------------------
// Examples:
//  "Foo/Bar" -> set the bar value of nested type foo
//  "Foo/BarArray/0" -> set the first value of BarArray of nested type foo

namespace KRG
{
    namespace TypeSystem
    {
        class KRG_SYSTEM_TYPESYSTEM_API PropertyPath
        {
            KRG_SERIALIZE_MEMBERS( m_pathElements );

        public:

            struct PathElement
            {
                KRG_SERIALIZE_MEMBERS( m_propertyID, m_arrayElementIdx );

                PathElement() : m_arrayElementIdx( InvalidIndex ) {}
                PathElement( StringID ID ) : m_propertyID( ID ), m_arrayElementIdx( InvalidIndex ) {}
                PathElement( StringID ID, S32 arrayElementIdx ) : m_propertyID( ID ), m_arrayElementIdx( arrayElementIdx ) {}

                inline bool IsArrayElement() const { return m_arrayElementIdx != InvalidIndex; }

                inline bool operator==( PathElement const& other ) const
                {
                    return m_propertyID == other.m_propertyID && m_arrayElementIdx == other.m_arrayElementIdx;
                }

                inline bool operator!=( PathElement const& other ) const
                {
                    return !( *this == other );
                }

            public:

                StringID    m_propertyID;
                S32         m_arrayElementIdx;
            };

        public:

            PropertyPath() {}
            PropertyPath( String const& pathString );

            inline bool IsValid() const
            {
                return !m_pathElements.empty();
            }

            inline size_t GetNumElements() const
            {
                return m_pathElements.size();
            }

            inline PathElement const& FirstElement() const
            {
                KRG_ASSERT( IsValid() );
                return m_pathElements.front();
            }

            inline PathElement const& GetLastElement() const
            {
                KRG_ASSERT( IsValid() );
                return m_pathElements.back();
            }

            inline bool IsPathToArrayElement() const
            {
                KRG_ASSERT( IsValid() );
                return GetLastElement().IsArrayElement();
            }

            inline void Append( StringID newElement, S32 arrayElementIdx = InvalidIndex )
            {
                KRG_ASSERT( newElement.IsValid() && arrayElementIdx >= InvalidIndex );
                m_pathElements.emplace_back( PathElement( newElement, arrayElementIdx ) );
            }

            inline void RemoveLastElement()
            {
                KRG_ASSERT( IsValid() );
                m_pathElements.pop_back();
            }

            inline void ReplaceLastElement( StringID newElement, S32 arrayElementIdx = InvalidIndex )
            {
                KRG_ASSERT( IsValid() );
                KRG_ASSERT( newElement.IsValid() && arrayElementIdx >= InvalidIndex );
                m_pathElements.back() = PathElement( newElement, arrayElementIdx );
            }

            inline PropertyPath GetPathWithoutFirstElement() const
            {
                KRG_ASSERT( IsValid() );
                KRG_ASSERT( m_pathElements.size() > 1 );
                PropertyPath subPath;
                subPath.m_pathElements = TInlineVector<PathElement, 10>( m_pathElements.begin() + 1, m_pathElements.end() );
                return subPath;
            }

            inline PathElement const& operator []( size_t idx ) const
            {
                KRG_ASSERT( idx < m_pathElements.size() );
                return m_pathElements[idx];
            }

            inline bool operator==( PropertyPath const& other ) const
            {
                S32 const numElements = (S32) m_pathElements.size();
                if ( numElements != other.GetNumElements() )
                {
                    return false;
                }

                for ( auto i = 0; i < numElements; i++ )
                {
                    if ( m_pathElements[i] != other.m_pathElements[i] )
                    {
                        return false;
                    }
                }

                return true;
            }

            inline bool operator!=( PropertyPath const& other ) const
            {
                return !( *this == other );
            }

            inline PropertyPath& operator+=( StringID newElement )
            {
                m_pathElements.emplace_back( PathElement( newElement ) );
                return *this;
            }

            String ToString() const;

        private:

            TInlineVector<PathElement, 10>    m_pathElements;
        };
    }
}