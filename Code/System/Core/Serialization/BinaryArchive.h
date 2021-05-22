#pragma once

#include "System/Core/_Module/API.h"
#include "Serialization.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Memory/MemoryStreamHelpers.h"
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
#include "UserTypes/eastl_fixed_vector.hpp"
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

        class KRG_SYSTEM_CORE_API BinaryFileArchive
        {
        public:

            BinaryFileArchive( Mode mode, FileSystem::Path const& filePath );
            ~BinaryFileArchive();

            inline Mode GetMode() const { return m_mode; }
            inline bool IsReading() const { return m_mode == Mode::Read; }
            inline bool IsWriting() const { return m_mode == Mode::Write; }

            bool IsValid() const;

            inline cereal::BinaryInputArchive* GetInputArchive() { return reinterpret_cast<cereal::BinaryInputArchive*>( m_pArchive ); }
            inline cereal::BinaryOutputArchive* GetOutputArchive() { return reinterpret_cast<cereal::BinaryOutputArchive*>( m_pArchive ); }

            template<typename T>
            inline BinaryFileArchive& operator<<( T&& type )
            {
                KRG_ASSERT( m_mode == Mode::Write );
                auto& archive = *reinterpret_cast<cereal::BinaryOutputArchive*>( m_pArchive );
                archive << std::forward<T>( type );
                return *this;
            }

            template<typename T>
            inline BinaryFileArchive& operator>>( T&& type )
            {
                KRG_ASSERT( m_mode == Mode::Read );
                auto& archive = *reinterpret_cast<cereal::BinaryInputArchive*>( m_pArchive );
                archive >> std::forward<T>( type );
                return *this;
            }

        private:

            FileSystem::Path                                m_filePath;
            Mode                                            m_mode;
            void*                                           m_pStream = nullptr;
            void*                                           m_pArchive = nullptr;
        };

        //-------------------------------------------------------------------------

        class KRG_SYSTEM_CORE_API BinaryMemoryArchive
        {
        public:

            BinaryMemoryArchive( Mode mode, TVector<Byte>& data );
            ~BinaryMemoryArchive();

            inline Mode GetMode() const { return m_mode; }
            inline bool IsReading() const { return m_mode == Mode::Read; }
            inline bool IsWriting() const { return m_mode == Mode::Write; }

            bool IsValid() const { return true; }

            inline cereal::BinaryInputArchive* GetInputArchive() { return reinterpret_cast<cereal::BinaryInputArchive*>( m_pArchive ); }
            inline cereal::BinaryOutputArchive* GetOutputArchive() { return reinterpret_cast<cereal::BinaryOutputArchive*>( m_pArchive ); }

            template<typename T>
            inline BinaryMemoryArchive& operator<<( T&& type )
            {
                KRG_ASSERT( m_mode == Mode::Write );
                auto& archive = *reinterpret_cast<cereal::BinaryOutputArchive*>( m_pArchive );
                archive << std::forward<T>( type );
                return *this;
            }

            template<typename T>
            inline BinaryMemoryArchive& operator>>( T&& type )
            {
                KRG_ASSERT( m_mode == Mode::Read );
                auto& archive = *reinterpret_cast<cereal::BinaryInputArchive*>( m_pArchive );
                archive >> std::forward<T>( type );
                return *this;
            }

        private:

            Mode                                            m_mode;
            MemoryStream                                    m_stream;
            void*                                           m_pArchive = nullptr;
        };
    }
}