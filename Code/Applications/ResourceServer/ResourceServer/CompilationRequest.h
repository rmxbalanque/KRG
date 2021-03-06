#pragma once
#include "System/Resource/ResourceID.h"
#include "System/Core/Time/Time.h"
#include "System/Core/Types/UUID.h"
#include "QDateTime"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        class CompilationRequest
        {
            friend class ResourceServer;
            friend class CompilationWorker;

        public:

            enum class Status
            {
                Pending,
                Executing,
                Succeeded,
                SucceededWithWarnings,
                Failed
            };

        public:

            // Get the client that requested this resource
            inline uint64 GetClientID() const { KRG_ASSERT( !IsInternalRequest() ); return m_clientID; }

            // Get the resource ID for this request
            inline ResourceID GetResourceID() const { return m_resourceID; }

            // Returns whether the request was externally requested (i.e. by a client) or internally requested (i.e. due to a file changing and being detected)
            inline bool IsInternalRequest() const { return m_isHotReloadRequest; }

            // Status
            inline Status GetStatus() const { return m_status; }
            inline bool IsPending() const { return m_status == Status::Pending; }
            inline bool IsExecuting() const { return m_status == Status::Executing; }
            inline bool HasSucceeded() const { return m_status == Status::Succeeded || m_status == Status::SucceededWithWarnings; }
            inline bool HasSucceededWithWarnings() const { return m_status == Status::SucceededWithWarnings; }
            inline bool HasFailed() const { return m_status == Status::Failed; }
            inline bool IsComplete() const { return HasSucceeded() || HasFailed(); }

            // Request Info
            inline char const* GetLog() const { return m_log.c_str(); }
            inline char const* GetCompilerArgs() const { return m_compilerArgs.c_str(); }
            inline FileSystemPath const& GetSourceFilePath() const { return m_sourceFile; }
            inline FileSystemPath const& GetDestinationFilePath() const { return m_destinationFile; }

            inline QDateTime const& GetTimeRequested() const { return m_timeRequested; }

            inline Seconds GetCompilationElapsedTime() const
            {
                if ( !IsComplete() )
                {
                    return Seconds( SystemClock::GetTime() - m_compilationTimeStarted );
                }

                return Seconds( m_compilationTimeFinished - m_compilationTimeStarted );
            }

            inline Seconds GetUptoDateCheckElapsedTime() const
            {
                return Seconds( m_upToDateCheckTimeFinished - m_upToDateCheckTimeStarted );
            }

        private:

            uint64                                 m_clientID = 0;
            ResourceID                          m_resourceID;
            int32                                 m_compilerVersion = -1;
            uint64                                 m_fileTimestamp = 0;
            uint64                                 m_sourceTimestampHash = 0;
            FileSystemPath                      m_sourceFile;
            FileSystemPath                      m_destinationFile;
            String                              m_compilerArgs;

            QDateTime                           m_timeRequested = QDateTime::currentDateTime();
            Nanoseconds                               m_compilationTimeStarted;
            Nanoseconds                               m_compilationTimeFinished;
            Nanoseconds                               m_upToDateCheckTimeStarted;
            Nanoseconds                               m_upToDateCheckTimeFinished;

            String                              m_log;
            Status                              m_status = Status::Pending;
            bool                                m_isHotReloadRequest = false;
        };
    }
}
