using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.IO;
using System.Linq;
using System.Text;

namespace Scripts
{
    class ResourceDescriptorProperty
    {

    }

    class ResourceDescriptorStringProperty : ResourceDescriptorProperty
    {
        public string Path { get; set; }
        public string Value { get; set; }
    }

    class ResourceDescriptorArrayProperty : ResourceDescriptorProperty
    {
        public string Path { get; set; }
        public List<string> Value { get; set; }
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
                if (prop is ResourceDescriptorStringProperty stringProp)
                {
                    writer.WritePropertyName(stringProp.Path);
                    writer.WriteValue(stringProp.Value);
                }
                else if (prop is ResourceDescriptorArrayProperty arrayProp)
                {
                    writer.WritePropertyName(arrayProp.Path);
                    writer.WriteStartArray();
                    foreach( var s in arrayProp.Value )
                    {
                        writer.WriteValue(s);
                    }
                    writer.WriteEndArray();
                }
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
        static string GetDefaultMaterialsForMesh(FileInfo meshFileInfo)
        {
            var dirName = meshFileInfo.Directory.FullName.ToLower();

            if (dirName.Contains("br") )
            {
                string meshName = meshFileInfo.Name.ToLower();
                if (meshName.Contains("road"))
                {
                    return "data://Packs/BR/Materials/PolygonBattleRoyale_Road_01.mtrl";
                }
                else if (meshName.Contains("bridge"))
                {
                    return "data://Packs/BR/Materials/PolygonBattleRoyale_Road_01.mtrl";
                }
                else if (meshName.Contains("plane"))
                {
                    return "data://Packs/BR/Materials/PolygonBattleRoyale_Plane_01.mtrl";
                }
                else if (meshName.Contains("destroyed"))
                {
                    return "data://Packs/BR/Materials/PolygonBattleRoyale_Vehicles_01_Damaged.mtrl";
                }
                else if (meshName.Contains("vehicle") || meshName.Contains("veh"))
                {
                    return "data://Packs/BR/Materials/PolygonBattleRoyale_Vehicles_01.mtrl";
                }

                return "data://Packs/BR/Materials/PolygonBattleRoyale_Texture_01_A.mtrl";
            }
            else if (dirName.Contains("city"))
            {
                return "data://Packs/City/Materials/PolygonCity_Texture_01_A.mtrl";
            }
            else if (dirName.Contains("gang"))
            {
                string meshName = meshFileInfo.Name.ToLower();
                if (meshName.Contains("vehicle") || meshName.Contains("veh"))
                {
                    return "data://Packs/Gang/Materials/PolygonGangWarfare_Vehicle_01.mtrl";
                }

                return "data://Packs/Gang/Materials/PolygonGangWarfare_Texture_01_A.mtrl";
            }
            else if (dirName.Contains("heist"))
            {
                return "data://Packs/Heist/Materials/T_PolygonHeist_01_A.mtrl";
            }
            else if (dirName.Contains("military"))
            {
                string meshName = meshFileInfo.Name.ToLower();
                if (meshName.Contains("sm_wep"))
                {
                    return "data://Packs/Military/Materials/PolygonMilitary_Weapons_01.mtrl";
                }
                else if ( meshName.Contains("sm_veh"))
                {
                    return "data://Packs/Military/Materials/Land_Vehicle_Master_01.mtrl";
                }

                return "data://Packs/Military/Materials/PolygonMilitary_Texture_01_A.mtrl";
            }

            return string.Empty;
        }

        static void CreateSkeletalMeshResourceFile(JsonSerializerSettings settings, DirectoryInfo dataDir, FileInfo sourceFile)
        {
            var filename = Path.GetFileNameWithoutExtension(sourceFile.FullName);
            string dataPath = sourceFile.FullName.Replace(dataDir.FullName, "data:/").Replace("\\", "/");

            ResourceDescriptor desc = new ResourceDescriptor()
            {
                properties = new List<ResourceDescriptorProperty>()
                {
                    new ResourceDescriptorStringProperty()
                    {
                        Path = "TypeID", Value = "KRG::Render::SkeletalMeshResourceDescriptor"
                    },
                    new ResourceDescriptorStringProperty()
                    {
                        Path = "m_meshDataPath", Value = dataPath
                    },
                    new ResourceDescriptorArrayProperty()
                    {
                        Path = "m_materials",
                        Value = new List<string>
                        {
                            GetDefaultMaterialsForMesh( sourceFile ),
                            GetDefaultMaterialsForMesh( sourceFile )
                        }
                    }
                }
            };

            string fileContents = JValue.Parse(JsonConvert.SerializeObject(desc, settings)).ToString(Formatting.Indented);
            var outFile = sourceFile.Directory.FullName + "\\" + filename + ".smsh";
            File.WriteAllText(outFile, fileContents);
        }

        static void CreateStaticMeshResourceFile(JsonSerializerSettings settings, DirectoryInfo dataDir, FileInfo sourceFile)
        {
            var filename = Path.GetFileNameWithoutExtension(sourceFile.FullName);
            string dataPath = sourceFile.FullName.Replace(dataDir.FullName, "data:/").Replace("\\", "/");

            ResourceDescriptor desc = new ResourceDescriptor()
            {
                properties = new List<ResourceDescriptorProperty>()
                {
                    new ResourceDescriptorStringProperty()
                    {
                        Path = "TypeID", Value = "KRG::Render::StaticMeshResourceDescriptor"
                    },
                    new ResourceDescriptorStringProperty()
                    {
                        Path = "m_meshDataPath", Value = dataPath
                    },
                    new ResourceDescriptorArrayProperty()
                    {
                        Path = "m_materials",
                        Value = new List<string>
                        {
                            GetDefaultMaterialsForMesh( sourceFile ),
                            GetDefaultMaterialsForMesh( sourceFile ),
                            GetDefaultMaterialsForMesh( sourceFile ),
                            GetDefaultMaterialsForMesh( sourceFile ),
                            GetDefaultMaterialsForMesh( sourceFile ),
                            GetDefaultMaterialsForMesh( sourceFile ),
                            GetDefaultMaterialsForMesh( sourceFile ),
                            GetDefaultMaterialsForMesh( sourceFile ),
                        }
                    }
                }
            };

            string fileContents = JValue.Parse(JsonConvert.SerializeObject(desc, settings)).ToString(Formatting.Indented);
            var outFile = sourceFile.Directory.FullName + "\\" + filename + ".msh";
            File.WriteAllText(outFile, fileContents);
        }

        static void CreatePhysicsResourceFile(JsonSerializerSettings settings, DirectoryInfo dataDir, FileInfo sourceFile)
        {
            var filename = Path.GetFileNameWithoutExtension(sourceFile.FullName);
            string dataPath = sourceFile.FullName.Replace(dataDir.FullName, "data:/").Replace("\\", "/");

            ResourceDescriptor desc = new ResourceDescriptor()
            {
                properties = new List<ResourceDescriptorProperty>()
                {
                    new ResourceDescriptorStringProperty()
                    {
                        Path = "TypeID", Value = "KRG::Physics::PhysicsMeshResourceDescriptor"
                    },
                    new ResourceDescriptorStringProperty()
                    {
                        Path = "m_meshDataPath", Value = dataPath
                    }
                }
            };

            string fileContents = JValue.Parse(JsonConvert.SerializeObject(desc, settings)).ToString(Formatting.Indented);
            var outFile = sourceFile.Directory.FullName + "\\" + filename + ".pmsh";
            File.WriteAllText(outFile, fileContents);
        }

        static void CreateTextureResourceFile(JsonSerializerSettings settings, DirectoryInfo dataDir, FileInfo sourceFile)
        {
            var filename = Path.GetFileNameWithoutExtension(sourceFile.FullName);
            string dataPath = sourceFile.FullName.Replace(dataDir.FullName, "data:/").Replace("\\", "/");

            ResourceDescriptor desc = new ResourceDescriptor()
            {
                properties = new List<ResourceDescriptorProperty>()
                {
                    new ResourceDescriptorStringProperty()
                    {
                        Path = "TypeID", Value = "KRG::Render::TextureResourceDescriptor"
                    },
                    new ResourceDescriptorStringProperty()
                    {
                        Path = "m_textureDataPath", Value = dataPath
                    }
                }
            };

            string fileContents = JValue.Parse(JsonConvert.SerializeObject(desc, settings)).ToString(Formatting.Indented);
            var outFile = sourceFile.Directory.FullName + "\\" + filename + ".txtr";
            File.WriteAllText(outFile, fileContents);
        }

        static void CreateMaterialResourceFile(JsonSerializerSettings settings, DirectoryInfo dataDir, FileInfo sourceFile)
        {
            var filename = Path.GetFileNameWithoutExtension(sourceFile.FullName);
            string dataPath = sourceFile.FullName.Replace(dataDir.FullName, "data:/").Replace("\\", "/").Replace(".TGA", ".txtr").Replace(".tga", ".txtr").Replace(".PNG", ".txtr").Replace(".png", ".txtr");

            ResourceDescriptor desc = new ResourceDescriptor()
            {
                properties = new List<ResourceDescriptorProperty>()
                {
                    new ResourceDescriptorStringProperty()
                    {
                        Path = "TypeID", Value = "KRG::Render::MaterialResourceDescriptor"
                    },
                    new ResourceDescriptorStringProperty()
                    {
                        Path = "m_diffuseTexture", Value = dataPath
                    }
                }
            };

            string fileContents = JValue.Parse(JsonConvert.SerializeObject(desc, settings)).ToString(Formatting.Indented);
            var outFile = sourceFile.Directory.FullName + "\\" + filename + ".mtrl";
            outFile = outFile.Replace("Textures", "Materials");
            var outputFile = new FileInfo(outFile);

            Directory.CreateDirectory( outputFile.Directory.FullName );
            File.WriteAllText(outFile, fileContents);
        }

        public static void GenerateResources(DirectoryInfo dataDir, DirectoryInfo directory)
        {
            var settings = new JsonSerializerSettings();
            settings.Formatting = Formatting.None;
            settings.NullValueHandling = NullValueHandling.Ignore;
            settings.Converters.Add(new ResourceDescriptorConverter());

            var files = directory.GetFilesByExtensions(".fbx", ".FBX", ".TGA", ".png", ".PNG", ".tga");

            foreach (var file in files)
            {
                if (file.Extension.ToLower() == ".fbx")
                {
                    if (file.Name.ToLower().StartsWith("sk_"))
                    {
                        CreateSkeletalMeshResourceFile(settings, dataDir, file);
                    }

                    if (file.Name.ToLower().StartsWith("sm_"))
                    {
                        CreateStaticMeshResourceFile(settings, dataDir, file);
                        CreatePhysicsResourceFile(settings, dataDir, file);
                    }
                }
                else if (file.Extension.ToLower() == ".tga" || file.Extension.ToLower() == ".png")
                {
                    CreateTextureResourceFile( settings, dataDir, file );
                    CreateMaterialResourceFile(settings, dataDir, file);
                }
            }
        }
        #endregion

        #region Conversion
        public static void RemoveRootArraysFromResourceDescriptors(DirectoryInfo directory)
        {
            var files = directory.GetFilesByExtensions(".mtrl", ".txtr", ".skms", ".stms", ".pmsh", ".anim", ".skel");
            foreach (var file in files)
            {
                try
                {
                    JArray arr = JArray.Parse(File.ReadAllText(file.FullName));
                    if (arr.Count == 1)
                    {
                        string jsonString = arr.Children<JObject>().First().ToString();
                        File.WriteAllText(file.FullName, jsonString);
                    }
                }
                catch (Exception)
                {

                }
            }
        }
        #endregion

        #region misc
        public static void GenerationAnimDataSetFile( FileInfo file )
        {
            StringBuilder sb = new StringBuilder();
            StringWriter sw = new StringWriter(sb);

            System.Random random = new System.Random();

            using (JsonWriter writer = new JsonTextWriter(sw))
            {
                writer.Formatting = Formatting.Indented;

                writer.WriteStartObject();
                writer.WritePropertyName("Name");
                writer.WriteValue("Default");

                writer.WritePropertyName("Skeleton");
                writer.WriteValue("data://Animation/SyntyStudiosSkel.skel");

                writer.WritePropertyName("Data");
                writer.WriteStartArray();

                for( var i=0; i < 5000; i++)
                {
                    writer.WriteStartObject();

                    writer.WritePropertyName("ID");
                    writer.WriteValue( Guid.NewGuid().ToString() );

                    string dataPath = string.Format("data://Animation/Stand_Crowd_Ambient{0}.anim", random.Next(0, 6));
                    writer.WritePropertyName("DataPath");
                    writer.WriteValue(dataPath);

                    writer.WriteEndObject();
                }

                writer.WriteEndArray();
                writer.WriteEndObject();
            }

            Directory.CreateDirectory(file.Directory.FullName);
            File.WriteAllText(file.FullName, sb.ToString());
        }

        internal static void GenerationAmplifyAnimDescriptors(DirectoryInfo directory)
        {
            var files = directory.GetFilesByExtensions(".fbx", ".FBX" );

            foreach (var file in files)
            {
                var outFile = new FileInfo( Path.ChangeExtension( file.FullName, ".anim" ) );

                if (file.Extension.ToLower() == ".fbx")
                {
                    string dataPath = file.FullName.Replace('\\', '/');
                    dataPath = dataPath.Replace("D:/Kruger/Data/", "data://");

                    StringBuilder sb = new StringBuilder();
                    StringWriter sw = new StringWriter(sb);

                    System.Random random = new System.Random();

                    using (JsonWriter writer = new JsonTextWriter(sw))
                    {
                        writer.Formatting = Formatting.Indented;

                        writer.WriteStartObject();

                        writer.WritePropertyName("TypeID");
                        writer.WriteValue("KRG::Animation::AnimationClipResourceDescriptor");

                        writer.WritePropertyName("m_animationDataPath");
                        writer.WriteValue( dataPath );

                        writer.WritePropertyName("m_pSkeleton");
                        writer.WriteValue("data://animation/amplify/amplifyskeleton.skel");

                        writer.WritePropertyName("m_animationEventData");
                        writer.WriteValue("");

                        writer.WritePropertyName("m_animationName");
                        writer.WriteValue("Scene");

                        writer.WriteEndObject();
                    }

                    File.WriteAllText(outFile.FullName, sb.ToString());
                }
            }
        }
        #endregion
    }
}
