#pragma once
#include "System/Core/Types/StringID.h"
#include "System/Core/Serialization/Serialization.h"

//-------------------------------------------------------------------------
// General Tag
//-------------------------------------------------------------------------
// Categorized 4-Level 128bit tag consisting of 4 StringIDs
// Categories must be sequentially set for the tag to be valid, you cannot have an invalid ID between valid IDs.

namespace KRG
{
    class KRG_SYSTEM_CORE_API Tag
    {
        KRG_SERIALIZE_MEMBERS( m_c );

        enum State
        {
            HasValidCategories_0 = 1,
            HasValidCategories_01 = 3,
            HasValidCategories_012 = 7,
            HasValidCategories_0123 = 15,
        };

    public:

        using QueryResults = TInlineVector<Tag, 5>;

        // Utility Functions
        //-------------------------------------------------------------------------

        // Create a tag from a string in the format "C0.C1.C2.C3"
        static Tag FromTagFormatString( String const& tagFormatString );

        // Does this container have a tag that exactly matches the specified tag
        template<typename T>
        static bool HasExactlyMatchingTag( T const& tagContainer, Tag const& tagToMatch )
        {
            for ( Tag const& tag : tagContainer )
            {
                if ( tag == tagToMatch )
                {
                    return true;
                }
            }

            return false;
        }

        // Ensure that the supplied container doesnt have any duplicate tags
        template<typename T>
        static void EnsureUniqueTags( T& tagContainer )
        {
            auto SortPredicate = [] ( Tag const& a, Tag const& b )
            {
                for ( int32 i = 0; i < 4; i++ )
                {
                    if ( a.m_c[i] < b.m_c[i] )
                    {
                        return true;
                    }

                    if ( a.m_c[i] > b.m_c[i] )
                    {
                        return false;
                    }
                }

                return false;
            };

            eastl::sort( tagContainer.begin(), tagContainer.end(), SortPredicate );
            auto lastIter = eastl::unique( tagContainer.begin(), tagContainer.end() );
            tagContainer.erase( lastIter, tagContainer.end() );
        }

        // Get all the child tags for a given parent in this container, returns true if any children were found.
        template<typename T>
        static bool FindAllChildTags( T const& tagContainer, Tag const& parentTag, QueryResults& outResults )
        {
            bool childrenWereFound = false;
            outResults.clear();

            for ( Tag const& tag : tagContainer )
            {
                if ( tag.IsChildOf( parentTag ) )
                {
                    outResults.emplace_back( tag );
                    childrenWereFound = true;
                }
            }

            return childrenWereFound;
        }

        // Does this container has any tags that are children of the supplied tag
        template<typename T>
        static bool HasAnyChildTags( T const& tagContainer, Tag const& parentTag )
        {
            for ( Tag const& tag : tagContainer )
            {
                if ( tag.IsChildOf( parentTag ) )
                {
                    return true;
                }
            }

            return false;
        }

    public:

        Tag()
        {
            m_c[0] = m_c[1] = m_c[2] = m_c[3] = StringID();
        }

        explicit Tag( char const* p0, char const* p1 = nullptr, char const* p2 = nullptr, char const* p3 = nullptr )
        {
            m_c[0] = StringID( p0 );
            m_c[1] = StringID( p1 );
            m_c[2] = StringID( p2 );
            m_c[3] = StringID( p3 );
        }

        explicit Tag( StringID s0, StringID s1 = StringID(), StringID s2 = StringID(), StringID s3 = StringID() )
        {
            m_c[0] = s0;
            m_c[1] = s1;
            m_c[2] = s2;
            m_c[3] = s3;
        }

        // Is this tag valid? Categories must be sequentially set for the tag to be valid, you cannot have an invalid ID between valid IDs.
        inline bool IsValid() const
        {
            uint32 const state = GetTagState();
            return state == HasValidCategories_0 || state == HasValidCategories_01 || state == HasValidCategories_012 || state == HasValidCategories_0123;
        }

        // Get the tag as a string in the following format: "C0.C1.C2.C3"
        String ToString() const;

        // Accessors
        //-------------------------------------------------------------------------

        inline Tag GetParentTag() const
        {
            Tag parentTag;

            uint32 const state = GetTagState();
            switch ( state )
            {
                case HasValidCategories_01:
                {
                    return Tag( m_c[0] );
                }
                break;

                case HasValidCategories_012:
                {
                    return Tag( m_c[0], m_c[1] );
                }
                break;

                case HasValidCategories_0123:
                {
                    return Tag( m_c[0], m_c[1], m_c[2] );
                }
                break;
            }

            return Tag();
        }

        // Queries and Comparisons
        //-------------------------------------------------------------------------

        // Are these tags EXACTLY equal
        inline bool operator==( Tag const& rhs ) const
        {
            return m_c[0] == rhs.m_c[0] && m_c[1] == rhs.m_c[1] && m_c[2] == rhs.m_c[2] && m_c[3] == rhs.m_c[3];
        }

        // Are these tags different
        inline bool operator!=( Tag const& rhs ) const
        {
            return m_c[0] != rhs.m_c[0] || m_c[1] != rhs.m_c[1] || m_c[2] != rhs.m_c[2] || m_c[3] != rhs.m_c[3];
        }

        // Is this tag a parent of the specified tag? e.g., A.B is a parent to A.B.C and A.B.Q
        inline bool IsParentOf( Tag const& childTag ) const
        {
            KRG_ASSERT( IsValid() && childTag.IsValid() );

            if ( m_c[0] != childTag.m_c[0] )
            {
                return false;
            }

            if ( m_c[1].IsValid() )
            {
                if ( m_c[1] != childTag.m_c[1] )
                {
                    return false;
                }

                if ( m_c[2].IsValid() )
                {
                    if ( m_c[2] != childTag.m_c[2] )
                    {
                        return false;
                    }

                    // This is kinda silly, since this is just a inefficient tag comparison at this point
                    if ( m_c[3].IsValid() )
                    {
                        if ( m_c[3] != childTag.m_c[3] )
                        {
                            return false;
                        }
                    }
                }
            }

            return true;
        }

        // Is this tag a child of the specified tag? e.g., A.B.C derives from A and from A.B
        inline bool IsChildOf( Tag const& parentTag ) const
        {
            KRG_ASSERT( IsValid() && parentTag.IsValid() );

            if ( m_c[0] != parentTag.m_c[0] )
            {
                return false;
            }

            if ( parentTag.m_c[1].IsValid() )
            {
                if ( m_c[1] != parentTag.m_c[1] )
                {
                    return false;
                }

                if ( parentTag.m_c[2].IsValid() )
                {
                    if ( m_c[2] != parentTag.m_c[2] )
                    {
                        return false;
                    }

                    // This is kinda silly, since this is just a inefficient tag comparison at this point
                    if ( parentTag.m_c[3].IsValid() )
                    {
                        if ( m_c[3] != parentTag.m_c[3] )
                        {
                            return false;
                        }
                    }
                }
            }

            return true;
        }

    private:

        KRG_FORCE_INLINE uint32 GetTagState() const
        {
            uint32 v = 0;
            v |= ( m_c[0].IsValid() ? 1 : 0 );
            v |= ( m_c[1].IsValid() ? 1 : 0 ) << 1;
            v |= ( m_c[2].IsValid() ? 1 : 0 ) << 2;
            v |= ( m_c[3].IsValid() ? 1 : 0 ) << 3;
            return v;
        }

    private:

        StringID m_c[4];
    };
}