#include "CompilationWorker.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        CompilationWorker::CompilationWorker( String const& workerFullPath )
            : m_workerFullPath( workerFullPath )
            , m_process(  this )
        {
            QObject::connect( &m_process, SIGNAL( finished( int, QProcess::ExitStatus ) ), this, SLOT( OnCompilationProcessComplete( int, QProcess::ExitStatus ) ) );
        }

        CompilationWorker::~CompilationWorker()
        {
            m_process.waitForFinished();
        }

        void CompilationWorker::StartCompileTask( CompilationRequest* pRequest )
        {
            KRG_ASSERT( !IsBusy() && pRequest != nullptr && pRequest->IsPending() );
            m_pRequest = pRequest;

            QStringList args( "-compile" );
            args += m_pRequest->m_compilerArgs.c_str();

            m_pRequest->m_compilationTimeStarted = SystemClock::GetTime();
            m_pRequest->m_status = CompilationRequest::Status::Executing;

            m_process.start( m_workerFullPath.c_str(), args );
        }

        void CompilationWorker::OnCompilationProcessComplete( int exitCode, QProcess::ExitStatus status )
        {
            if ( status == QProcess::ExitStatus::CrashExit )
            {
                m_pRequest->m_status = CompilationRequest::Status::Failed;
                m_pRequest->m_log = "Resource compiler crashed!";
            }
            else
            {
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

                m_pRequest->m_log = m_process.readAllStandardError();
                m_pRequest->m_log += m_process.readAllStandardOutput();
            }

            m_pRequest->m_compilationTimeFinished = SystemClock::GetTime();
            m_pRequest = nullptr;
        }
    }
}