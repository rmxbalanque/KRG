#pragma once

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class ResourceServer;
    class CompilationRequest;

    //-------------------------------------------------------------------------

    class ResourceServerUI
    {

    public:

        ResourceServerUI( ResourceServer* pServer );
        void Draw();

    private:

        void DrawPendingRequests();
        void DrawCompletedRequests();
        void DrawWorkerStatus();
        void DrawRegisteredCompilers();

    private:

        ResourceServer*                 m_pResourceServer = nullptr;
        CompilationRequest const*       m_pSelectedCompletedRequest = nullptr;
    };
}
