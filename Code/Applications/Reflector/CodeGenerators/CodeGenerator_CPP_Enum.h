#pragma once
#include "Applications/Reflector/Database/ReflectionDatabase.h"

//-------------------------------------------------------------------------

using namespace KRG::TypeSystem::Reflection;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace CPP
    {
        namespace EnumGenerator
        {
            void Generate( std::stringstream& headerFile, std::stringstream& codeFile, String const& exportMacro, TypeDescriptor const& type );
            void GenerateRegistrationFunctionCall( std::stringstream& file, TypeDescriptor const& type );
            void GenerateUnregistrationFunctionCall( std::stringstream& file, TypeDescriptor const& type );
        }
    }
}