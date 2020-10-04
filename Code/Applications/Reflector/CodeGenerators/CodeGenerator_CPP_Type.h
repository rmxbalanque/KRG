#pragma once

#include "Applications/Reflector/Database/ReflectionDatabase.h"

//-------------------------------------------------------------------------

using namespace KRG::TypeSystem::Reflection;
using namespace KRG::TypeSystem;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace CPP
    {
        namespace TypeGenerator
        {
            void Generate( ReflectionDatabase const& database, std::stringstream& headerFile, std::stringstream& codeFile, String const& exportMacro, TypeDescriptor const& type, TVector<TypeDescriptor> const& parentDescs );
            void GenerateRegistrationFunctionCall( std::stringstream& file, TypeDescriptor const& type );
            void GenerateUnregistrationFunctionCall( std::stringstream& file, TypeDescriptor const& type );
        }
    }
}