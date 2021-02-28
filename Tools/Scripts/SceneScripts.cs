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
        public Guid SpatialParent { get; set; }
        public string AttachmentSocketID { get; set; }
        public List<Property> Properties { get; set; }
    }

    class EntitySystem
    {
        public string TypeID { get; set; }
    }

    class Entity
    {
        public Guid ID { get; set; } = Guid.NewGuid();
        public string Name { get; set; }
        public Guid SpatialParent { get; set; }
        public string AttachmentSocketID { get; set; }
        public List<EntitySystem> Systems { get; set; }
        public List<EntityComponent> Components { get; set; }
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

            if( value.SpatialParent != null  && value.SpatialParent != Guid.Empty )
            {
                writer.WritePropertyName( "SpatialParent" );
                writer.WriteValue( value.SpatialParent );
            }

            if ( !string.IsNullOrEmpty( value.AttachmentSocketID ) )
            {
                writer.WritePropertyName( "AttachmentSocketID" );
                writer.WriteValue( value.AttachmentSocketID );
            }

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

            writer.WriteEndObject();
        }

        public override EntityComponent ReadJson( JsonReader reader, Type objectType, [AllowNull] EntityComponent existingValue, bool hasExistingValue, JsonSerializer serializer )
        {
            throw new NotImplementedException();
        }
    }

    class SceneScripts
    {
        public static void GenerateECSTestMapFile( FileInfo outputFile, int numRows, int numColumns )
        {
            var map = new Map();

            //------------------------------------------------------------------------

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

            map.Entities.Add( floorEntity );

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

                    //--------------------------------------------------------------------

                    var meshComponent = new EntityComponent()
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
                                }
                    };

                    var armorMeshComponent = new EntityComponent()
                    {
                        Name = "Armor",
                        TypeID = "KRG::Animation::AnimatedMeshComponent",
                        SpatialParent = meshComponent.ID,
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
                    };

                    var hairMeshComponent = new EntityComponent()
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
                            new Property()
                            {
                                Path = "m_mobility",
                                Value = "Dynamic"
                            }
                        }
                    };

                    var animationComponent = new EntityComponent()
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
                    };

                    //------------------------------------------------------------------------

                    var characterEntity = new Entity()
                    {
                        Name = string.Format( "Character {0}", characterNumber ),
                        Components = new List<EntityComponent>()
                        {
                            armorMeshComponent,
                            animationComponent,
                            meshComponent,
                        },
                        Systems = new List<EntitySystem>()
                        {
                            new EntitySystem()
                            {
                                TypeID = "KRG::Animation::AnimationSystem"
                            }
                        }
                    };

                    map.Entities.Add( characterEntity );

                    //------------------------------------------------------------------------

                    var hairEntity = new Entity()
                    {
                        Name = string.Format( "Prop {0}", characterNumber ),
                        SpatialParent = characterEntity.ID,
                        AttachmentSocketID = "head",

                        Components = new List<EntityComponent>()
                        {
                            hairMeshComponent
                        }
                    };

                    map.Entities.Add( hairEntity );
                }
            }

            //------------------------------------------------------------------------

            var settings = new JsonSerializerSettings();
            settings.Formatting = Formatting.None;
            settings.NullValueHandling = NullValueHandling.Ignore;
            settings.Converters.Add( new ComponentConverter() );

            string output = JsonConvert.SerializeObject( map, settings );
            string outputFormatted = JValue.Parse( output ).ToString( Formatting.Indented );

            File.WriteAllText( outputFile.FullName, outputFormatted );
        }

        // Hack: create human hats to test longer spatial attachment chains
        public static void GenerateECSTestMapFile2( FileInfo outputFile, int numRows, int numColumns )
        {
            var map = new Map();

            //------------------------------------------------------------------------

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

                    //--------------------------------------------------------------------

                    var meshComponent = new EntityComponent()
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
                                }
                    };

                    var armorMeshComponent = new EntityComponent()
                    {
                        Name = "Armor",
                        TypeID = "KRG::Animation::AnimatedMeshComponent",
                        SpatialParent = meshComponent.ID,
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
                    };

                    var animationComponent = new EntityComponent()
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
                    };

                    //------------------------------------------------------------------------

                    var characterEntity = new Entity()
                    {
                        Name = string.Format( "Character {0}", characterNumber ),
                        Components = new List<EntityComponent>()
                        {
                            armorMeshComponent,
                            animationComponent,
                            meshComponent,
                        },
                        Systems = new List<EntitySystem>()
                        {
                            new EntitySystem()
                            {
                                TypeID = "KRG::Animation::AnimationSystem"
                            }
                        }
                    };

                    map.Entities.Add( characterEntity );
                }
            }

            //------------------------------------------------------------------------

            for( int i = 2; i < map.Entities.Count; i += 3 )
            {
                var ParentEntity = map.Entities[i - 2];
                var ChildEntity1 = map.Entities[i - 1];
                var ChildEntity2 = map.Entities[i];

                ChildEntity1.SpatialParent = ParentEntity.ID;
                ChildEntity1.AttachmentSocketID = "head";
                ChildEntity1.Components[2].Properties.RemoveAt( 3 );

                ChildEntity2.SpatialParent = ChildEntity1.ID;
                ChildEntity2.AttachmentSocketID = "head";
                ChildEntity2.Components[2].Properties.RemoveAt( 3 );
            }

            //------------------------------------------------------------------------

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

            map.Entities.Add( floorEntity );

            //------------------------------------------------------------------------

            var settings = new JsonSerializerSettings();
            settings.Formatting = Formatting.None;
            settings.NullValueHandling = NullValueHandling.Ignore;
            settings.Converters.Add( new ComponentConverter() );

            string output = JsonConvert.SerializeObject( map, settings );
            string outputFormatted = JValue.Parse( output ).ToString( Formatting.Indented );

            File.WriteAllText( outputFile.FullName, outputFormatted );
        }
    }
}
