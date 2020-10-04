using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.IO;
using System.Linq;

namespace Scripts
{
    class ResourceDescriptorProperty
    {
        public string Path { get; set; }
        public string Value { get; set; }
    }

    class ResourceDescriptor
    {
        public List<ResourceDescriptorProperty> properties { get; set; }
    }

    class ResourceDescriptorConverter : JsonConverter<ResourceDescriptor>
    {
        public override void WriteJson( JsonWriter writer, ResourceDescriptor value, JsonSerializer serializer )
        {
            writer.WriteStartObject();

            foreach ( var prop in value.properties )
            {
                writer.WritePropertyName( prop.Path );
                writer.WriteValue( prop.Value );
            }

            writer.WriteEndObject();
        }

        public override ResourceDescriptor ReadJson( JsonReader reader, Type objectType, [AllowNull] ResourceDescriptor existingValue, bool hasExistingValue, JsonSerializer serializer )
        {
            throw new NotImplementedException();
        }
    }

    public class ResourceScripts
    {
        #region Generation
        static void CreateSkeletalMeshResourceFile( JsonSerializerSettings settings, DirectoryInfo dataDir, FileInfo sourceFile )
        {
            var filename = Path.GetFileNameWithoutExtension( sourceFile.FullName );
            string dataPath = sourceFile.FullName.Replace( dataDir.FullName, "data:/" ).Replace( "\\", "/" );

            ResourceDescriptor desc = new ResourceDescriptor()
            {
                properties = new List<ResourceDescriptorProperty>()
                {
                    new ResourceDescriptorProperty()
                    {
                        Path = "TypeID", Value = "KRG::Render::SkeletalMeshResourceDescriptor"
                    },
                    new ResourceDescriptorProperty()
                    {
                        Path = "m_resourceTypeID", Value = "SMSH"
                    },
                    new ResourceDescriptorProperty()
                    {
                        Path = "m_meshDataPath", Value = dataPath
                    }
                }
            };

            string fileContents = JValue.Parse( JsonConvert.SerializeObject( desc, settings ) ).ToString( Formatting.Indented );
            var outFile = sourceFile.Directory.FullName + "\\" + filename + ".smsh";
            File.WriteAllText( outFile, fileContents );
        }

        static void CreateStaticMeshResourceFile( JsonSerializerSettings settings, DirectoryInfo dataDir, FileInfo sourceFile )
        {
            var filename = Path.GetFileNameWithoutExtension( sourceFile.FullName );
            string dataPath = sourceFile.FullName.Replace( dataDir.FullName, "data:/" ).Replace( "\\", "/" );

            ResourceDescriptor desc = new ResourceDescriptor()
            {
                properties = new List<ResourceDescriptorProperty>()
                {
                    new ResourceDescriptorProperty()
                    {
                        Path = "TypeID", Value = "KRG::Render::StaticMeshResourceDescriptor"
                    },
                    new ResourceDescriptorProperty()
                    {
                        Path = "m_resourceTypeID", Value = "MSH"
                    },
                    new ResourceDescriptorProperty()
                    {
                        Path = "m_meshDataPath", Value = dataPath
                    }
                }
            };

            string fileContents = JValue.Parse( JsonConvert.SerializeObject( desc, settings ) ).ToString( Formatting.Indented );
            var outFile = sourceFile.Directory.FullName + "\\" + filename + ".msh";
            File.WriteAllText( outFile, fileContents );
        }

        static void CreatePhysicsResourceFile( JsonSerializerSettings settings, DirectoryInfo dataDir, FileInfo sourceFile )
        {
            var filename = Path.GetFileNameWithoutExtension( sourceFile.FullName );
            string dataPath = sourceFile.FullName.Replace( dataDir.FullName, "data:/" ).Replace( "\\", "/" );

            ResourceDescriptor desc = new ResourceDescriptor()
            {
                properties = new List<ResourceDescriptorProperty>()
                {
                    new ResourceDescriptorProperty()
                    {
                        Path = "TypeID", Value = "KRG::Physics::PhysicsGeometryResourceDescriptor"
                    },
                    new ResourceDescriptorProperty()
                    {
                        Path = "m_resourceTypeID", Value = "PHYS"
                    },
                    new ResourceDescriptorProperty()
                    {
                        Path = "m_geometryDataPath", Value = dataPath
                    }
                }
            };

            string fileContents = JValue.Parse( JsonConvert.SerializeObject( desc, settings ) ).ToString( Formatting.Indented );
            var outFile = sourceFile.Directory.FullName + "\\" + filename + ".phys";
            File.WriteAllText( outFile, fileContents );
        }

        static void CreateTextureResourceFile( JsonSerializerSettings settings, DirectoryInfo dataDir, FileInfo sourceFile )
        {
            var filename = Path.GetFileNameWithoutExtension( sourceFile.FullName );
            string dataPath = sourceFile.FullName.Replace( dataDir.FullName, "data:/" ).Replace( "\\", "/" );

            ResourceDescriptor desc = new ResourceDescriptor()
            {
                properties = new List<ResourceDescriptorProperty>()
                {
                    new ResourceDescriptorProperty()
                    {
                        Path = "TypeID", Value = "KRG::Render::TextureResourceDescriptor"
                    },
                    new ResourceDescriptorProperty()
                    {
                        Path = "m_resourceTypeID", Value = "TXTR"
                    },
                    new ResourceDescriptorProperty()
                    {
                        Path = "m_textureDataPath", Value = dataPath
                    }
                }
            };

            string fileContents = JValue.Parse( JsonConvert.SerializeObject( desc, settings ) ).ToString( Formatting.Indented );
            var outFile = sourceFile.Directory.FullName + "\\" + filename + ".txtr";
            File.WriteAllText( outFile, fileContents );
        }

        static void CreateMaterialResourceFile( JsonSerializerSettings settings, DirectoryInfo dataDir, FileInfo sourceFile )
        {
            var filename = Path.GetFileNameWithoutExtension( sourceFile.FullName );
            string dataPath = sourceFile.FullName.Replace( dataDir.FullName, "data:/" ).Replace( "\\", "/" ).Replace(".TGA", ".txtr").Replace( ".tga", ".txtr" ).Replace( ".PNG", ".txtr" ).Replace( ".png", ".txtr" );

            ResourceDescriptor desc = new ResourceDescriptor()
            {
                properties = new List<ResourceDescriptorProperty>()
                {
                    new ResourceDescriptorProperty()
                    {
                        Path = "TypeID", Value = "KRG::Render::MaterialResourceDescriptor"
                    },
                    new ResourceDescriptorProperty()
                    {
                        Path = "m_resourceTypeID", Value = "MTRL"
                    },
                    new ResourceDescriptorProperty()
                    {
                        Path = "m_diffuseTexture", Value = dataPath
                    }
                }
            };

            string fileContents = JValue.Parse( JsonConvert.SerializeObject( desc, settings ) ).ToString( Formatting.Indented );
            var outFile = sourceFile.Directory.FullName + "\\..\\" + filename + ".mtrl";
            File.WriteAllText( outFile, fileContents );
        }

        public static void GenerateResources( DirectoryInfo dataDir, DirectoryInfo directory )
        {
            var settings = new JsonSerializerSettings();
            settings.Formatting = Formatting.None;
            settings.NullValueHandling = NullValueHandling.Ignore;
            settings.Converters.Add( new ResourceDescriptorConverter() );

            var files = directory.GetFilesByExtensions( ".fbx", ".FBX", ".TGA", ".png", ".PNG", ".tga" );

            foreach ( var file in files )
            {
                if( file.Extension.ToLower() == ".fbx" )
                {
                    //if( file.Name.ToLower().StartsWith( "sk_" ) )
                    //{
                    //    CreateSkeletalMeshResourceFile( settings, dataDir, file );
                    //}

                    //if ( file.Name.ToLower().StartsWith( "sm_" ) )
                    //{
                    //    CreateStaticMeshResourceFile( settings, dataDir, file );
                    //    CreatePhysicsResourceFile( settings, dataDir, file );
                    //}
                }
                else if ( file.Extension.ToLower() == ".tga" || file.Extension.ToLower() == ".png" )
                {
                    //CreateTextureResourceFile( settings, dataDir, file );
                    CreateMaterialResourceFile( settings, dataDir, file );
                }
            }
        }
        #endregion

        #region Conversion
        public static void RemoveRootArraysFromResourceDescriptors( DirectoryInfo directory )
        {
            var files = directory.GetFilesByExtensions( ".mtrl", ".txtr", ".skms", ".stms", ".phys", ".anim", ".skel" );
            foreach ( var file in files )
            {
                try
                {
                    JArray arr = JArray.Parse( File.ReadAllText( file.FullName ) );
                    if( arr.Count == 1 )
                    {
                        string jsonString = arr.Children<JObject>().First().ToString();
                        File.WriteAllText( file.FullName, jsonString );
                    }
                }
                catch( Exception )
                {

                }
            }
        }
        #endregion
    }
}
