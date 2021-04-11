using System.IO;

namespace Scripts
{
    class Program
    {
        static void Main( string[] args )
        {
            ResourceScripts.GenerateResources( new DirectoryInfo( "D:\\Kruger\\Data" ), new DirectoryInfo( "D:\\Kruger\\Data\\Packs\\Military" ) );
            //ResourceScripts.RemoveRootArraysFromResourceDescriptors( new DirectoryInfo( "D:\\Kruger\\Data" ) );
            //SceneScripts.GenerateECSTestMapFile( new FileInfo( @"D:\Kruger\Data\Maps\ECS_TestMap.map" ), 100, 100 );
        }
    }
}
