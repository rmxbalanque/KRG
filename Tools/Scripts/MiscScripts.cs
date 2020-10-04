using System.Globalization;
using System.IO;

namespace Scripts
{
    class MiscScripts
    {
        public static void ReformatIconList( FileInfo iconPath )
        {
            string[] lines = System.IO.File.ReadAllLines( iconPath.FullName );
            string reformattedFileContents = string.Empty;

            TextInfo textInfo = new CultureInfo( "en-US", false ).TextInfo;

            foreach ( string line in lines )
            {
                var lineElements = line.Split( " " );
                lineElements[0] = textInfo.ToTitleCase( lineElements[0].ToLower() );

                lineElements[1] = lineElements[1].Replace( "u8\"\\u", " = 0x" );
                lineElements[1] = lineElements[1].Replace( "\"", "," );

                reformattedFileContents += lineElements[0] + lineElements[1] + "\r\n";
            }

            File.WriteAllText( iconPath.FullName, reformattedFileContents );
        }
    }
}
