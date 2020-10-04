#include "SingleInstanceLock.h"
#include <QCryptographicHash>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Tools
    {
        namespace
        {
            QString GenerateKeyHash( const QString& key, const QString& salt )
            {
                QByteArray data;
                data.append( key.toUtf8() );
                data.append( salt.toUtf8() );
                data = QCryptographicHash::hash( data, QCryptographicHash::Sha1 ).toHex();
                return data;
            }
        }

        //-------------------------------------------------------------------------

        SingleInstanceLock::SingleInstanceLock( QString const& key )
            : m_sharedMemory( GenerateKeyHash( key, "sharedMemorySalt" ) )
            , m_semaphore( GenerateKeyHash( key, "semaphoreSalt" ), 1 )
        {}

        SingleInstanceLock::~SingleInstanceLock()
        {
            Release();
        }

        bool SingleInstanceLock::TryLock()
        {
            // Try attach to see if the block is already created
            //-------------------------------------------------------------------------

            m_semaphore.acquire();
            bool const isSharedMemoryAlreadyCreated = m_sharedMemory.attach();
            if ( isSharedMemoryAlreadyCreated )
            {
                m_sharedMemory.detach();
            }
            m_semaphore.release();

            if ( isSharedMemoryAlreadyCreated )
            {
                return false;
            }

            // Create shared memory block
            //-------------------------------------------------------------------------

            m_semaphore.acquire();
            bool const isSharedMemoryCreated = m_sharedMemory.create( sizeof( quint64 ) );
            m_semaphore.release();

            if ( !isSharedMemoryCreated )
            {
                Release();
                return false;
            }

            return true;
        }

        void SingleInstanceLock::Release()
        {
            m_semaphore.acquire();
            if ( m_sharedMemory.isAttached() )
            {
                m_sharedMemory.detach();
            }
            m_semaphore.release();
        }
    }
}

