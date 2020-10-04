#pragma once

#include "Tools/UI/_Module/API.h"

#include <QObject>
#include <QSharedMemory>
#include <QSystemSemaphore>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Tools
    {
        class KRG_TOOLS_UI_API SingleInstanceLock
        {

        public:

            SingleInstanceLock( QString const& key );
            ~SingleInstanceLock();

            bool TryLock();
            void Release();

        private:

            SingleInstanceLock& operator=( SingleInstanceLock const& ) = delete;

        private:

            QSharedMemory               m_sharedMemory;
            QSystemSemaphore            m_semaphore;
        };
    }
}