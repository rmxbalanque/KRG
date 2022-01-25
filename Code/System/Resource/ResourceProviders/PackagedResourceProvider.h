#pragma once

#include "System/Resource/ResourceProvider.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class Settings;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_RESOURCE_API PackagedResourceProvider final : public ResourceProvider
    {

    public:

        PackagedResourceProvider( Settings const* pSettings );

        virtual bool IsReady() const override final;

    private:

        PackagedResourceProvider() = delete;

        virtual void RequestRawResource( ResourceRequest* pRequest ) override;
        virtual void CancelRequest( ResourceRequest* pRequest ) override;

    private:

        FileSystem::Path const m_compiledResourcesPath;
    };
}