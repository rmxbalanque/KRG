#pragma once

#include "ReflectionDataTypes.h"
#include "Tools/Core/ThirdParty/sqlite/SqliteHelpers.h"
#include "System/Resource/ResourceTypeID.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/TypeSystem/PropertyPath.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem::Reflection
{
    class ReflectionDatabase : public SQLite::SQLiteDatabase
    {
    public:

        ReflectionDatabase();

        // Database functions
        bool ReadDatabase( FileSystem::Path const& databasePath );
        bool WriteDatabase( FileSystem::Path const& databasePath );
        bool CleanDatabase( FileSystem::Path const& databasePath );

        // Module functions
        TVector<ProjectInfo> const& GetAllRegisteredProjects() const { return m_reflectedProjects; }
        bool IsProjectRegistered( ProjectID projectID ) const;
        ProjectInfo const* GetProjectDesc( ProjectID projectID ) const;
        void UpdateProjectList( TVector<ProjectInfo> const& registeredProjects );

        bool IsHeaderRegistered( HeaderID headerID ) const;
        HeaderInfo const* GetHeaderDesc( HeaderID headerID ) const;
        void UpdateHeaderRecord( HeaderInfo const& header );

        // Type functions
        ReflectedType const* GetType( TypeID typeID ) const;
        TVector<ReflectedType> const& GetAllTypes() const { return m_reflectedTypes; }
        bool IsTypeRegistered( TypeID typeID ) const;
        bool IsTypeDerivedFrom( TypeID typeID, TypeID parentTypeID ) const;
        void GetAllTypesForHeader( HeaderID headerID, TVector<ReflectedType>& types ) const;
        void GetAllTypesForProject( ProjectID projectID, TVector<ReflectedType>& types ) const;
        void RegisterType( ReflectedType const* pType );

        // Property functions
        ReflectedProperty const* GetPropertyTypeDescriptor( TypeID typeID, PropertyPath const& pathID ) const;

        // Resource functions
        bool IsResourceRegistered( ResourceTypeID typeID ) const;
        void RegisterResource( ReflectedResourceType const* pDesc );
        TVector<ReflectedResourceType> const& GetAllRegisteredResourceTypes() const { return m_reflectedResourceTypes; }

        // Cleaning
        void DeleteTypesForHeader( HeaderID headerID );
        void DeleteObseleteHeadersAndTypes( TVector<HeaderID> const& registeredHeaders );
        void DeleteObseleteProjects( TVector<ProjectInfo> const& registeredProjects );

    private:

        bool CreateTables();
        bool DropTables();

        bool ReadAdditionalTypeData( ReflectedType& type );
        bool ReadAdditionalEnumData( ReflectedType& type );

        bool WriteAdditionalTypeData( ReflectedType const& type );
        bool WriteAdditionalEnumData( ReflectedType const& type );

    private:

        ReflectedType                       m_registeredTypeBase;
        TVector<ReflectedType>              m_reflectedTypes;
        TVector<HeaderInfo>                 m_reflectedHeaders;
        TVector<ProjectInfo>                m_reflectedProjects;
        TVector<ReflectedResourceType>      m_reflectedResourceTypes;
    };
}