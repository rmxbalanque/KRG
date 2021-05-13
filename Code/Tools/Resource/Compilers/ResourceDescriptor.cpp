#include "ResourceDescriptor.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    void ResourceDescriptor::ReadCompileDependencies( String const& descriptorFileContents, TVector<DataPath>& outDependencies )
    {
        // Read all strings and return any data paths we encounter
        String stringToTest;
        size_t currentFilePosition = 0;

        while ( true )
        {
            size_t stringStartPos = descriptorFileContents.find_first_of( '\"', currentFilePosition );
            if ( stringStartPos == String::npos )
            {
                return;
            }

            currentFilePosition = stringStartPos + 1;
            size_t stringEndPos = descriptorFileContents.find_first_of( '\"', currentFilePosition );
            if ( stringEndPos == String::npos )
            {
                return;
            }
            currentFilePosition = stringEndPos + 1;

            // Check if string is a data path
            stringToTest = descriptorFileContents.substr( stringStartPos + 1, stringEndPos - stringStartPos - 1 );
            if ( DataPath::IsValidDataPath( stringToTest ) )
            {
                outDependencies.emplace_back( DataPath( stringToTest ) );
            }
        }
    }
}