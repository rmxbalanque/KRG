#pragma once
#include "RawFileInspector.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class FileInspectorImageFormats : public RawFileInspector
    {
        constexpr static char const* s_title = "Image Inspector";

    public:

        FileInspectorImageFormats( EditorModel* pModel, FileSystem::Path const& filePath );

    private:

        virtual bool DrawDialog() override;
    };
}