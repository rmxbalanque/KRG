#pragma once

#include "System/Core/_Module/API.h"
#include "Serialization.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include <istream>
#include <ostream>

#include <cereal/archives/binary.hpp>

// Cereal support for common STL types
#include <cereal/types/complex.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/common.hpp>
#include <cereal/types/polymorphic.hpp>

// Custom serialization types
#include "UserTypes/eastl_string.hpp"
#include "UserTypes/eastl_bitset.hpp"
#include "UserTypes/eastl_vector.hpp"
#include "UserTypes/eastl_set.hpp"
#include "UserTypes/eastl_stack.hpp"
#include "UserTypes/eastl_deque.hpp"
#include "UserTypes/eastl_queue.hpp"
#include "UserTypes/eastl_hash_map.hpp"
#include "UserTypes/eastl_list.hpp"
#include "UserTypes/eastl_map.hpp"
#include "UserTypes/eastl_utility.hpp"
#include "UserTypes/eastl_tuple.hpp"
#include "UserTypes/eastl_hash_set.hpp"
#include "UserTypes/eastl_chrono.hpp"
#include "UserTypes/krg_stringid.hpp"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Serialization
    {
        enum class Mode
        {
            None,
            Read,
            Write,
        };

        //-------------------------------------------------------------------------

        class KRG_SYSTEM_CORE_API BinaryArchive
        {
            union InternalArchive
            {
                cereal::BinaryInputArchive*                 m_pInputArchive;
                cereal::BinaryOutputArchive*                m_pOutputArchive;
            };

            union InternalStream
            {
                std::istream*                               m_pInStream;
                std::ostream*                               m_pOutStream;
            };

        public:

            BinaryArchive( Mode mode, FileSystemPath const& filePath );
            BinaryArchive( TVector<Byte>& data ); // Read only
            ~BinaryArchive();

            inline Mode GetMode() const { return m_mode; }
            inline bool IsReading() const { return m_mode == Mode::Read; }
            inline bool IsWriting() const { return m_mode == Mode::Write; }

            bool IsValid() const;

            template<typename T>
            inline BinaryArchive& operator<<( T&& type )
            {
                KRG_ASSERT( m_mode == Mode::Write );
                ( *m_archive.m_pOutputArchive )( std::forward<T>( type ) );
                return *this;
            }

            template<typename T>
            inline BinaryArchive& operator>>( T&& type )
            {
                KRG_ASSERT( m_mode == Mode::Read );
                ( *m_archive.m_pInputArchive )( std::forward<T>( type ) );
                return *this;
            }

        private:

            FileSystemPath                                  m_filePath;
            Mode                                            m_mode;
            InternalStream                                  m_stream;
            InternalArchive                                 m_archive;
        };
    }
}