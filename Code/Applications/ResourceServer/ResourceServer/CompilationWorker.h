#pragma once

#include "CompilationRequest.h"
#include "Tools/Core/Thirdparty/subprocess/subprocess.h"
#include "System/Core/Types/String.h"
#include "System/Core/Threading/Threading.h"
#include "System/Core/Threading/TaskSystem.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class CompilationWorker : public IAsyncTask
    {
    public:

        enum class Status : uint8
        {
            Idle,
            Compiling,
            Complete
        };

    public:

        CompilationWorker( TaskSystem* pTaskSystem, String const& workerFullPath );
        ~CompilationWorker();

        // Worker Status
        inline Status GetStatus() const { return m_status; }
        inline bool IsIdle() const { return m_status == Status::Idle; }
        inline bool IsCompiling() const { return m_status == Status::Compiling; }
        inline bool IsComplete() const { return m_status == Status::Complete; }

        // Request an async resource compilation
        void Compile( CompilationRequest* pRequest );

        // Accept the result of a requesting compilation, will set the compiler back to idle
        inline CompilationRequest* AcceptResult()
        {
            KRG_ASSERT( IsComplete() );
            KRG_ASSERT( m_pRequest != nullptr && m_pRequest->IsComplete() );

            auto pResult = m_pRequest;
            m_pRequest = nullptr;
            m_status = Status::Idle;
            return pResult;
        }

        // Get the current request ID
        inline ResourceID const& GetRequestResourceID() const 
        {
            KRG_ASSERT( IsCompiling() );
            KRG_ASSERT( m_pRequest != nullptr );
            return m_pRequest->GetResourceID();
        }

    private:

        virtual void ExecuteRange( TaskSetPartition range, uint32 threadnum ) override final;

    private:

        TaskSystem*                             m_pTaskSystem = nullptr;
        String const                            m_workerFullPath;
        CompilationRequest*                     m_pRequest = nullptr;
        subprocess_s                            m_subProcess;
        std::atomic<Status>                     m_status = Status::Idle;
    };
}