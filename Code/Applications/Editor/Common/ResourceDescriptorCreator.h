#pragma once
#include "Applications/Editor/EditorModel.h"
#include "Tools/Core/TypeSystem/Serialization/NativeTypeWriter.h"
#include "System/TypeSystem/TypeID.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Resource/ResourceTypeID.h"

//-------------------------------------------------------------------------
// Base class for custom resource descriptor creators
//-------------------------------------------------------------------------

namespace KRG
{
    class ResourceDescriptorCreator
    {
    public:

        struct DescriptorType
        {
            String                          m_name;
            ResourceTypeID                  m_typeID;
        };

    public:

        bool ShowCreatorDialog( FileSystem::Path const& rawFile );

    protected:

        ResourceDescriptorCreator( EditorModel* pModel );

        inline uint32 GetSelectedTypeIndex() const { return m_selectedTypeIdx; }

        FileSystem::Path CreateDescriptorFilePath( FileSystem::Path const& rawFile ) const;

        // Derived types need to implement this function, generally this will take the form of something like this:
        //-------------------------------------------------------------------------
        //{
        //    ResourceDescriptor desc;
        //    // Set your data

        //    auto newDescriptorFilePath = ;
        //    TypeSystem::NativeTypeWriter typeWriter( typeRegistry );
        //    typeWriter << desc;
        //    typeWriter.WriteToFile( CreateDescriptorFilePath( rawFile ) );
        //}
        virtual void CreateNewDescriptor( FileSystem::Path const& rawFile ) const = 0;

    protected:

        EditorModel*                        m_pModel = nullptr;

        // Derived classes are expected to fill this out!
        TVector<DescriptorType>             m_decriptorTypes;

    private:

        uint32                              m_selectedTypeIdx = 0;
        char                                m_filenameBuffer[256] = { 0 };
        bool                                m_isDialogClosed = true;
    };
}