#include "Reflector.h"
#include "Applications/Shared/ApplicationGlobalState.h"
#include "Applications/Shared/cmdParser/krg_cmdparser.h"
#include "System/TypeSystem/CoreTypeIDs.h"
#include "System/Core/Logging/Log.h"
#include <fstream>
#include <iosfwd>
#include <iostream>

//-------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
    KRG::ApplicationGlobalState State;

    // Set precision of cout
    //-------------------------------------------------------------------------

    std::cout.setf( std::ios::fixed, std::ios::floatfield );
    std::cout.precision( 2 );

    // Read CMD line arguments
    //-------------------------------------------------------------------------

    cli::Parser cmdParser( argc, argv );
    cmdParser.set_required<std::string>( "s", "SlnPath", "Solution Path." );
    cmdParser.set_optional<bool>( "clean", "Clean", false, "Clean Solution." );
    cmdParser.set_optional<bool>( "rebuild", "Rebuild", false, "Clean Solution." );

    if ( !cmdParser.run() )
    {
        KRG_LOG_ERROR( "Reflector", "Invalid commandline arguments" );
        return 1;
    }

    KRG::FileSystem::Path slnPath = cmdParser.get<std::string>( "s" ).c_str();
    bool const shouldClean = cmdParser.get<bool>( "clean" );
    bool const shouldRebuild = cmdParser.get<bool>( "rebuild" );

    // Execute reflector
    //-------------------------------------------------------------------------

    // Parse solution
    KRG::TypeSystem::Reflection::Reflector reflector;
    if ( reflector.ParseSolution( slnPath ) )
    {
        if ( shouldRebuild )
        {
            reflector.Clean();
            return reflector.Build() ? 0 : 1;
        }
        else if ( shouldClean )
        {
            return reflector.Clean() ? 0 : 1;
        }
        else
        {
            return reflector.Build() ? 0 : 1;
        }
    }

    return 1;
}