#pragma once
#include "../_Module/API.h"
#include "System/Resource/ResourcePtr.h"
#include "System/Core/Types/StringID.h"
#include "Engine/Animation/AnimationSkeleton.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "Tools/Core/ThirdParty/KRG_RapidJson.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem { class TypeRegistry;}

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    struct Variation : public IRegisteredType
    {
        KRG_REGISTER_TYPE( Variation );

        inline bool IsValid() const { return m_ID.IsValid(); }
        inline bool operator==( StringID const& variationID ) const { return m_ID == variationID; }
        inline bool operator!=( StringID const& variationID ) const { return m_ID != variationID; }

        EXPOSE      StringID                m_ID;
        REGISTER    StringID                m_parentID;
        EXPOSE      TResourcePtr<Skeleton>  m_pSkeleton;
    };

    //-------------------------------------------------------------------------

    class VariationHierarchy
    {

    public:

        VariationHierarchy();

        void Reset();

        // Does the specified variation exist?
        inline bool IsValidVariation( StringID variationID ) const { return eastl::find( m_variations.begin(), m_variations.end(), variationID ) != m_variations.end(); }

        // Get a specified variation
        Variation* GetVariation( StringID variationID );

        // Get a specified variation
        Variation const* GetVariation( StringID variationID ) const { return const_cast<VariationHierarchy*>( this )->GetVariation( variationID ); }

        // Returns the ID of the parent variation (invalid ID if called for the default variation)
        StringID GetParentVariationID( StringID variationID ) const;

        // Get all children for a specified variation
        TInlineVector<StringID, 5> GetChildVariations( StringID variationID ) const;

        // This will create a new variation under the specified parent variation
        void CreateVariation( StringID variationID, StringID parentVariationID );

        // This will rename the specified variation
        void RenameVariation( StringID oldVariationID, StringID newVariationID );

        // This will destroy the specified variation and all children
        void DestroyVariation( StringID variationID );

        // Serialization
        bool Serialize( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& objectValue );
        void Serialize( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const;

    private:

        inline int32 GetVariationIndex( StringID variationID ) const
        {
            auto iter = eastl::find( m_variations.begin(), m_variations.end(), variationID );
            if ( iter != m_variations.end() )
            {
                return int32( iter - m_variations.begin() );
            }

            return InvalidIndex;
        }

    private:

        TVector<Variation>                          m_variations;
    };
}