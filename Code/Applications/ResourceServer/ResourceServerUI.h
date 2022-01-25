#pragma once
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class ResourceServer;
    class CompilationRequest;

    //-------------------------------------------------------------------------

    class ResourceServerUI final
    {

    public:

        ResourceServerUI( ResourceServer* pServer );
        void Draw();

    private:

        void DrawCompletedRequests();
        void DrawWorkerStatus();
        void DrawServerInfo();
        void DrawConnectionInfo();

    private:

        ResourceServer*                         m_pResourceServer = nullptr;
        CompilationRequest const*               m_pSelectedCompletedRequest = nullptr;
        TVector<CompilationRequest const*>      m_combinedRequests; // Temp working memory for UI
    };
}
