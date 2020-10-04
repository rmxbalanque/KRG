using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.IO;

namespace Scripts
{
    class Property
    {
        public string Path { get; set; }
        public string Value { get; set; }
    }

    class EntityComponent
    {
        public Guid ID { get; set; } = Guid.NewGuid();
        public string Name { get; set; }
        public string TypeID { get; set; }
        public string AttachmentSocketID { get; set; }
        public List<Property> Properties { get; set; }
        public List<EntityComponent> Components { get; set; }
    }

    class EntitySystem
    {
        public string TypeID { get; set; }
    }

    class Entity
    {
        public Guid ID { get; set; } = Guid.NewGuid();
        public string Name { get; set; }
        public string AttachmentSocketID { get; set; }
        public List<EntitySystem> Systems { get; set; }
        public List<EntityComponent> Components { get; set; }
        public List<Entity> Entities { get; set; }
    }

    class Map
    {
        public List<Entity> Entities { get; set; } = new List<Entity>();
    }

    class ComponentConverter : JsonConverter<EntityComponent>
    {
        public override void WriteJson( JsonWriter writer, EntityComponent value, JsonSerializer serializer )
        {
            writer.WriteStartObject();

            writer.WritePropertyName( "ID" );
            writer.WriteValue( value.ID );

            writer.WritePropertyName( "Name" );
            writer.WriteValue( value.Name );

            // Type Data
            {
                writer.WritePropertyName( "TypeData" );
                writer.WriteStartObject();

                writer.WritePropertyName( "TypeID" );
                writer.WriteValue( value.TypeID );

                foreach ( var prop in value.Properties )
                {
                    writer.WritePropertyName( prop.Path );
                    writer.WriteValue( prop.Value );
                }

                writer.WriteEndObject();
            }

            // Child Components
            if ( value.Components != null )
            {
                writer.WritePropertyName( "Components" );

                var settings = new JsonSerializerSettings();
                settings.Formatting = Formatting.None;
                settings.NullValueHandling = NullValueHandling.Ignore;
                settings.Converters.Add( new ComponentConverter() );

                writer.WriteRawValue( JsonConvert.SerializeObject( value.Components, settings ) );
            }

            writer.WriteEndObject();
        }

        public override EntityComponent ReadJson( JsonReader reader, Type objectType, [AllowNull] EntityComponent existingValue, bool hasExistingValue, JsonSerializer serializer )
        {
            throw new NotImplementedException();
        }
    }

    class SceneScripts
    {
        public static void GenerateTestMapFile()
        {
            var map = new Map();

            //------------------------------------------------------------------------

            var cameraEntity = new Entity()
            {
                Name = "Camera",
                Components = new List<EntityComponent>()
                {
                    new EntityComponent()
                    {
                        Name = "CameraComponent",
                        TypeID = "KRG::CameraComponent",
                        Properties = new List<Property>()
                        {
                            new Property()
                            {
                                Path = "m_transform",
                                Value = "0, 0, 0, -2.000000, -1.50000, 1.000000, 1.000000, 1.0, 1.0"
                            }
                        }
                    }
                },
                Systems = new List<EntitySystem>()
                {
                    new EntitySystem()
                    {
                        TypeID = "KRG::DefaultPlayerController"
                    }
                }
            };

            var floorEntity = new Entity()
            {
                Name = "Floor",
                Components = new List<EntityComponent>()
                {
                    new EntityComponent()
                    {
                        Name = "FloorMesh",
                        TypeID = "KRG::Render::StaticMeshComponent",
                        Properties = new List<Property>()
                        {
                            new Property()
                            {
                                Path = "m_pMesh",
                                Value = "data://Geometry/FloorMesh.msh"
                            },
                            new Property()
                            {
                                Path = "m_materials/0",
                                Value = "data://Geometry/FloorMaterial.mtrl"
                            }
                        }
                    }
                }
            };

            map.Entities.Add( cameraEntity );
            map.Entities.Add( floorEntity );

            int numRows = 100;
            int numColumns = 100;
            float offset = 1.0f;
            float startX = -numColumns * offset / 2;
            float startY = -numRows * offset / 2;

            Random random = new Random();

            string[] characterOptions =
            {
                "data://Packs/BR/Characters/SK_Chr_MercenaryFemale_01.smsh",
                "data://Packs/BR/Characters/SK_Chr_SportsBraFemale_01.smsh",
                "data://Packs/BR/Characters/SK_Chr_MilitaryFemale_01.smsh",
                "data://Packs/BR/Characters/SK_Chr_SportyFemale_01.smsh",
                "data://Packs/BR/Characters/SK_Chr_GothFemale_01.smsh",
            };

            string[] armorOptions =
            {
                "data://Packs/BR/Characters/SK_Chr_Attach_Female_Armor_01.smsh",
                "data://Packs/BR/Characters/SK_Chr_Attach_Female_Armor_02.smsh",
                "data://Packs/BR/Characters/SK_Chr_Attach_Female_Armor_03.smsh"
            };

            for ( var r = 0; r < numRows; r++ )
            {
                for ( var c = 0; c < numColumns; c++ )
                {
                    int characterNumber = ( r * numColumns ) + c;
                    float characterX = startX + c * offset;
                    float characterY = startY + r * offset;
                    string characterTransform = string.Format( "0, 0, 0, {0}, {1}, 0.0, 1.0, 1.0, 1.0", characterX, characterY );

                    int randomCharacterOption = random.Next( 0, 5 );
                    int randomArmorOption = random.Next( 0, 3 );
                    int randomMaterialOption = random.Next( 1, 5 );
                    int randomAnimOption = random.Next( 0, 7 );

                    var characterEntity = new Entity()
                    {
                        Name = string.Format( "Character {0}", characterNumber ),
                        Components = new List<EntityComponent>()
                        {
                            new EntityComponent()
                            {
                                Name = "CharacterMesh",
                                TypeID = "KRG::Animation::AnimatedMeshComponent",
                                Properties = new List<Property>()
                                {
                                    new Property()
                                    {
                                        Path = "m_pMesh",
                                        Value = characterOptions[randomCharacterOption]
                                    },
                                    new Property()
                                    {
                                        Path = "m_pSkeleton",
                                        Value = "data://Animation/SyntyStudiosSkel.skel"
                                    },
                                    new Property()
                                    {
                                        Path = "m_materials/0",
                                        Value = string.Format( "data://Packs/BR/Materials/PolygonBattleRoyale_Texture_0{0}_A.mtrl", randomMaterialOption )
                                    },
                                    new Property()
                                    {
                                        Path = "m_transform",
                                        Value = characterTransform
                                    }
                                },
                                Components = new List<EntityComponent>()
                                {
                                    new EntityComponent()
                                    {
                                        Name = "Armor",
                                        TypeID = "KRG::Animation::AnimatedMeshComponent",
                                        Properties = new List<Property>()
                                        {
                                            new Property()
                                            {
                                                Path = "m_pMesh",
                                                Value = armorOptions[randomArmorOption]
                                            },
                                            new Property()
                                            {
                                                Path = "m_pSkeleton",
                                                Value = "data://Animation/SyntyStudiosSkel.skel"
                                            },
                                            new Property()
                                            {
                                                Path = "m_materials/0",
                                                Value = string.Format( "data://Packs/BR/Materials/PolygonBattleRoyale_Texture_0{0}_A.mtrl", randomMaterialOption )
                                            }
                                        }
                                    }
                                }
                            },
                            new EntityComponent()
                            {
                                Name = "SimpleAnimation",
                                TypeID = "KRG::Animation::SimpleAnimationComponent",
                                Properties = new List<Property>()
                                {
                                    new Property()
                                    {
                                        Path = "m_pAnimation",
                                        Value = string.Format( "data://Animation/Stand_Crowd_Ambient{0}.anim", randomAnimOption )
                                    }
                                }
                            }
                        },
                        Systems = new List<EntitySystem>()
                        {
                            new EntitySystem()
                            {
                                TypeID = "KRG::Animation::AnimationSystem"
                            }
                        },
                        Entities = new List<Entity>()
                        {
                            new Entity()
                            {
                                Name = string.Format( "Prop {0}", characterNumber ),
                                AttachmentSocketID = "head",

                                Components = new List<EntityComponent>()
                                {
                                    new EntityComponent()
                                    {
                                        Name = "Prop",
                                        TypeID = "KRG::Render::StaticMeshComponent",
                                        Properties = new List<Property>()
                                        {
                                            new Property()
                                            {
                                                Path = "m_pMesh",
                                                Value = "data://Packs/BR/Characters/SM_Chr_Attach_Female_Hair_01.msh"
                                            },
                                            new Property()
                                            {
                                                Path = "m_materials/0",
                                                Value = string.Format( "data://Packs/BR/Materials/PolygonBattleRoyale_Texture_0{0}_A.mtrl", randomMaterialOption )
                                            },
                                            new Property()
                                            {
                                                Path = "m_transform",
                                                Value = "0, 0, 0, 0, 0, 0.0, 1.0, 1.0, 1.0"
                                            },
                                            //new Property()
                                            //{
                                            //    Path = "m_mobility",
                                            //    Value = "Dynamic"
                                            //}
                                        }
                                    }
                                }
                            }
                        }
                    };

                    map.Entities.Add( characterEntity );
                }
            }

            //------------------------------------------------------------------------

            var settings = new JsonSerializerSettings();
            settings.Formatting = Formatting.None;
            settings.NullValueHandling = NullValueHandling.Ignore;
            settings.Converters.Add( new ComponentConverter() );

            string output = JsonConvert.SerializeObject( map, settings );
            string outputFormatted = JValue.Parse( output ).ToString( Formatting.Indented );

            var outDir = new DirectoryInfo( @"D:\Kruger\Data\Maps\" );
            var mapFile = outDir.FullName + "ECS_TestMap.map";
            File.WriteAllText( mapFile, outputFormatted );
        }
    }
}
