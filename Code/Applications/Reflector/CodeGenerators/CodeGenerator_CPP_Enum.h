#pragma once
#include "Applications/Reflector/Database/ReflectionDatabase.h"

//-------------------------------------------------------------------------

using namespace KRG::TypeSystem::Reflection;

//-------------------------------------------------------------------------

namespace KRG::CPP::EnumGenerator
{
    void Generate( std::stringstream& headerFile, std::stringstream& codeFile, String const& exportMacro, ReflectedType const& type );
    void GenerateRegistrationFunctionCall( std::stringstream& file, ReflectedType const& type );
    void GenerateUnregistrationFunctionCall( std::stringstream& file, ReflectedType const& type );
}