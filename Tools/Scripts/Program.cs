using System.IO;

namespace Scripts
{
    class Program
    {
        static void Main( string[] args )
        {
            //ResourceScripts.GenerateResources( new DirectoryInfo( "D:\\Kruger\\Data" ), new DirectoryInfo( "D:\\Kruger\\Data\\Packs\\br" ) );
            //ResourceScripts.RemoveRootArraysFromResourceDescriptors( new DirectoryInfo( "D:\\Kruger\\Data" ) );
            SceneScripts.GenerateTestMapFile();
        }
    }
}
