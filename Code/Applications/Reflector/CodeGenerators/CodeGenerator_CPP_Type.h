#pragma once

#include "Applications/Reflector/Database/ReflectionDatabase.h"

//-------------------------------------------------------------------------

using namespace KRG::TypeSystem::Reflection;
using namespace KRG::TypeSystem;

//-------------------------------------------------------------------------

namespace KRG::CPP::TypeGenerator
{
    void Generate( ReflectionDatabase const& database, std::stringstream& headerFile, std::stringstream& codeFile, String const& exportMacro, ReflectedType const& type, TVector<ReflectedType> const& parentDescs );
    void GenerateRegistrationFunctionCall( std::stringstream& file, ReflectedType const& type );
    void GenerateUnregistrationFunctionCall( std::stringstream& file, ReflectedType const& type );
}