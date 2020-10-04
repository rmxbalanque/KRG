#if _WIN32
#pragma once

#include "ReflectionDataTypes.h"
#include "Tools/Core/ThirdParty/sqlite/SqliteHelpers.h"
#include "System/Resource/ResourceTypeID.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/TypeSystem/PropertyPath.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace Reflection
        {
            class ReflectionDatabase : public SQLite::SQLiteDatabase
            {
            public:

                // Database functions
                bool ReadDatabase( FileSystemPath const& databasePath );
                bool WriteDatabase( FileSystemPath const& databasePath );
                bool CleanDatabase( FileSystemPath const& databasePath );

                // Module functions
                TVector<ProjectDesc> const& GetAllRegisteredProjects() const { return m_projectDescs; }
                bool IsProjectRegistered( ProjectID projectID ) const;
                ProjectDesc const* GetProjectDesc( ProjectID projectID ) const;
                void UpdateProjectList( TVector<ProjectDesc> const& registeredProjects );

                bool IsHeaderRegistered( HeaderID headerID ) const;
                HeaderDesc const* GetHeaderDesc( HeaderID headerID ) const;
                void UpdateHeaderRecord( HeaderDesc const& header );

                // Type functions
                TypeDescriptor const* GetType( TypeID typeID ) const;
                TVector<TypeDescriptor> const& GetAllTypes() const { return m_typeDescs; }
                bool IsTypeRegistered( TypeID typeID ) const;
                bool IsTypeDerivedFrom( TypeID typeID, TypeID parentTypeID ) const;
                void GetAllTypesForHeader( HeaderID headerID, TVector<TypeDescriptor>& types ) const;
                void GetAllTypesForProject( ProjectID projectID, TVector<TypeDescriptor>& types ) const;
                void RegisterType( TypeDescriptor const* pType );

                // Property functions
                PropertyDescriptor const* GetPropertyTypeDescriptor( TypeID typeID, PropertyPath const& pathID ) const;

                // Resource functions
                bool IsResourceRegistered( ResourceTypeID typeID ) const;
                void RegisterResource( ResourceDesc const* pDesc );
                TVector<ResourceDesc> const& GetAllRegisteredResourceTypes() const { return m_resourceDescs; }

                // Cleaning
                void DeleteTypesForHeader( HeaderID headerID );
                void DeleteObseleteHeadersAndTypes( TVector<HeaderID> const& registeredHeaders );
                void DeleteObseleteProjects( TVector<ProjectDesc> const& registeredProjects );

            private:

                bool CreateTables();
                bool DropTables();

                bool ReadAdditionalTypeData( TypeDescriptor& type );
                bool ReadAdditionalEnumData( TypeDescriptor& type );

                bool WriteAdditionalTypeData( TypeDescriptor const& type );
                bool WriteAdditionalEnumData( TypeDescriptor const& type );

            private:

                TVector<TypeDescriptor>             m_typeDescs;
                TVector<HeaderDesc>                 m_headerDescs;
                TVector<ProjectDesc>                m_projectDescs;
                TVector<ResourceDesc>               m_resourceDescs;
            };
        }
    }
}

#endif