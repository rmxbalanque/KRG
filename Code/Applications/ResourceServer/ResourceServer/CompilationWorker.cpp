#include "CompilationWorker.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    CompilationWorker::CompilationWorker( TaskSystem* pTaskSystem, String const& workerFullPath )
        : m_pTaskSystem( pTaskSystem )
        , m_workerFullPath( workerFullPath )
    {
        KRG_ASSERT( pTaskSystem != nullptr && !m_workerFullPath.empty() );
        m_SetSize = 1;
    }

    CompilationWorker::~CompilationWorker()
    {
        KRG_ASSERT( !subprocess_alive( &m_subProcess ) );
    }

    void CompilationWorker::Compile( CompilationRequest* pRequest )
    {
        KRG_ASSERT( IsIdle() );
        KRG_ASSERT( pRequest != nullptr && pRequest->IsPending() );
        m_pRequest = pRequest;
        m_status = Status::Compiling;
        m_pTaskSystem->ScheduleTask( this );
    }

    void CompilationWorker::ExecuteRange( TaskSetPartition range, uint32 threadnum )
    {
        KRG_ASSERT( IsCompiling() );
        KRG_ASSERT( !m_pRequest->m_compilerArgs.empty() );
        char const* processCommandLineArgs[4] = { m_workerFullPath.c_str(), "-compile", m_pRequest->m_compilerArgs.c_str(), nullptr };

        // Start compiler process
        //-------------------------------------------------------------------------

        int32 result = subprocess_create( processCommandLineArgs, subprocess_option_combined_stdout_stderr, &m_subProcess );
        if ( result != 0 )
        {
            m_pRequest->m_status = CompilationRequest::Status::Failed;
            m_pRequest->m_log = "Resource compiler failed to start!";
            m_pRequest->m_compilationTimeFinished = SystemClock::GetTime();
            m_status = Status::Complete;
            return;
        }

        // Wait for compilation to complete
        //-------------------------------------------------------------------------

        int32 exitCode;
        result = subprocess_join( &m_subProcess, &exitCode );
        if ( result != 0 )
        {
            m_pRequest->m_status = CompilationRequest::Status::Failed;
            m_pRequest->m_log = "Resource compiler failed to complete!";
            m_pRequest->m_compilationTimeFinished = SystemClock::GetTime();
            m_status = Status::Complete;
            subprocess_destroy( &m_subProcess );
            return;
        }

        // Handle completed compilation
        //-------------------------------------------------------------------------

        m_pRequest->m_compilationTimeFinished = SystemClock::GetTime();

        switch ( exitCode )
        {
            case 0:
            {
                m_pRequest->m_status = CompilationRequest::Status::Succeeded;
            }
            break;

            case 1:
            {
                m_pRequest->m_status = CompilationRequest::Status::SucceededWithWarnings;
            }
            break;

            default:
            {
                m_pRequest->m_status = CompilationRequest::Status::Failed;
            }
            break;
        }

        // Read error and output of process
        //-------------------------------------------------------------------------

        char readBuffer[512];
        while ( fgets( readBuffer, 512, subprocess_stdout( &m_subProcess ) ) )
        {
            m_pRequest->m_log += readBuffer;
        }

        m_status = Status::Complete;

        //-------------------------------------------------------------------------

        subprocess_destroy( &m_subProcess );
    }
}