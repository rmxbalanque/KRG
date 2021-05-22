using System.IO;

namespace Scripts
{
    class Program
    {
        static void Main( string[] args )
        {
            //ResourceScripts.GenerateResources( new DirectoryInfo( "D:\\Kruger\\Data" ), new DirectoryInfo( "D:\\Kruger\\Data\\Packs\\Gang" ) );
            //ResourceScripts.RemoveRootArraysFromResourceDescriptors( new DirectoryInfo( "D:\\Kruger\\Data" ) );
            //SceneScripts.GenerateECSTestMapFile( new FileInfo( @"D:\Kruger\Data\Maps\ECS_TestMap.map" ), 100, 100 );

            ResourceScripts.GenerationAnimDataSetFile(new FileInfo("D:\\Kruger\\Data\\Animation\\AnimGraphDataSet.agds"));
        }
    }
}
