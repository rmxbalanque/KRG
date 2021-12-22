﻿using System.IO;

namespace Scripts
{
    class Program
    {
        static void Main( string[] args )
        {
            ResourceScripts.GenerateResources( new DirectoryInfo( "D:\\Kruger\\Data" ), new DirectoryInfo( "D:\\Kruger\\Data\\Packs\\Prototype" ) );
            //ResourceScripts.RemoveRootArraysFromResourceDescriptors( new DirectoryInfo( "D:\\Kruger\\Data" ) );
            //SceneScripts.GenerateECSTestMapFile( new FileInfo( @"D:\Kruger\Data\Maps\AnimTest_Full.map" ), 10, 10 );

            //ResourceScripts.GenerationAnimDataSetFile(new FileInfo("D:\\Kruger\\Data\\Animation\\AnimGraphDataSet.agds"));

            //ResourceScripts.GenerationAmplifyAnimDescriptors(new DirectoryInfo("D:\\Kruger\\Data\\Packs\\Amplify"));
        }
    }
}
