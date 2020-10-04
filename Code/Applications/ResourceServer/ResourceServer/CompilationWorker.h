#pragma once

#include "CompilationRequest.h"
#include <QProcess>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        class CompilationWorker : public QObject
        {
            Q_OBJECT

        public:

            CompilationWorker( String const& workerFullPath );
            ~CompilationWorker();

            inline bool IsBusy() const { return m_pRequest != nullptr; }
            inline CompilationRequest* GetActiveRequest() const { return m_pRequest; }

            void StartCompileTask( CompilationRequest* pRequest );

        private slots:

            void OnCompilationProcessComplete( int exitCode, QProcess::ExitStatus status );

        private:

            String const&                           m_workerFullPath;
            CompilationRequest*                     m_pRequest = nullptr;
            QProcess                                m_process;
        };
    }
}
