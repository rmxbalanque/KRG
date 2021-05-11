#pragma once

#include "System/Core/_Module/API.h"
#include "Serialization.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Memory/MemoryStreamHelpers.h"
#include <istream>
#include <ostream>

#include "cereal/archives/json.hpp"

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

        class KRG_SYSTEM_CORE_API JsonFileArchive
        {
        public:

            JsonFileArchive( Mode mode, FileSystem::Path const& filePath );
            ~JsonFileArchive();

            inline Mode GetMode() const { return m_mode; }
            inline bool IsReading() const { return m_mode == Mode::Read; }
            inline bool IsWriting() const { return m_mode == Mode::Write; }

            bool IsValid() const;

            template<typename T>
            inline JsonFileArchive& operator<<( T&& type )
            {
                KRG_ASSERT( m_mode == Mode::Write );
                auto& archive = *reinterpret_cast<cereal::JSONOutputArchive*>( m_pArchive );
                archive << std::forward<T>( type );
                return *this;
            }

            template<typename T>
            inline JsonFileArchive& operator>>( T&& type )
            {
                KRG_ASSERT( m_mode == Mode::Read );
                auto& archive = *reinterpret_cast<cereal::JSONInputArchive*>( m_pArchive );
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

        class KRG_SYSTEM_CORE_API JsonMemoryArchive
        {
        public:

            JsonMemoryArchive( Mode mode, TVector<Byte>& data );
            ~JsonMemoryArchive();

            inline Mode GetMode() const { return m_mode; }
            inline bool IsReading() const { return m_mode == Mode::Read; }
            inline bool IsWriting() const { return m_mode == Mode::Write; }

            bool IsValid() const { return true; }

            template<typename T>
            inline JsonMemoryArchive& operator<<( T&& type )
            {
                KRG_ASSERT( m_mode == Mode::Write );
                auto& archive = *reinterpret_cast<cereal::JSONOutputArchive*>( m_pArchive );
                archive << std::forward<T>( type );
                return *this;
            }

            template<typename T>
            inline JsonMemoryArchive& operator>>( T&& type )
            {
                KRG_ASSERT( m_mode == Mode::Read );
                auto& archive = *reinterpret_cast<cereal::JSONInputArchive*>( m_pArchive );
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