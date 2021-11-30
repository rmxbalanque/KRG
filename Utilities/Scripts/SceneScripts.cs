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
        public string Name { get; set; }
        public string TypeID { get; set; }
        public string SpatialParent { get; set; }
        public string AttachmentSocketID { get; set; }
        public List<Property> Properties { get; set; }
    }

    class EntitySystem
    {
        public string TypeID { get; set; }
    }

    class Entity
    {
        public string Name { get; set; }
        public string SpatialParent { get; set; }
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

            writer.WritePropertyName( "Name" );
            writer.WriteValue( value.Name );

            if( value.SpatialParent != null && value.SpatialParent.Length > 0 )
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
                                Value = "data://TestScenes/Floor/Floor.msh"
                            }
                        }
                    }
                }
            };

            map.Entities.Add( floorEntity );

            var lightEntity = new Entity()
            {
                Name = "DirectionalLight",
                Components = new List<EntityComponent>()
                {
                    new EntityComponent()
                    {
                        Name = "DirectionalLight",
                        TypeID = "KRG::Render::DirectionalLightComponent",
                        Properties = new List<Property>()
                        {
                            new Property()
                            {
                                Path = "m_shadowed",
                                Value = "True"
                            },
                            new Property()
                            {
                                Path = "m_transform",
                                Value = "135.000000,-0.000000,0.000000,0.000000,0.000000,10.000000,1.000000,1.000000,1.000000"
                            },
                            new Property()
                            {
                                Path = "m_intensity",
                                Value = "15"
                            }
                        }
                    }
                }
            };

            map.Entities.Add(lightEntity);

            //--------------------------------------------------------------------

            string[] animOptions =
            {
                "data://Packs/Amplify/Walk/Walk_Fwd_V2.anim",
                "data://Packs/Amplify/Dodge/Dodge_Bwd.anim",
                "data://Packs/Amplify/Run/Run_Fwd.anim",
                "data://Packs/Amplify/Sprint/Sprint_FW.anim",
                "data://Packs/Amplify/AssaultRifle/AR_Aim_Sweep_H.anim",
                "data://Packs/Amplify/Unarmed/Combat_Front_Kick.anim",
                "data://Packs/Amplify/Cover/LowCover_Exit_R_v3.anim",
            };

            float offset = 1.0f;
            float startX = -numColumns * offset / 2;
            float startY = -numRows * offset / 2;

            Random random = new Random();

            for ( var r = 0; r < numRows; r++ )
            {
                for ( var c = 0; c < numColumns; c++ )
                {
                    int characterNumber = ( r * numColumns ) + c;
                    float characterX = startX + c * offset;
                    float characterY = startY + r * offset;
                    string characterTransform = string.Format( "0, 0, 0, {0}, {1}, 0.0, 1.0, 1.0, 1.0", characterX, characterY );

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
                                        Value = "data://Packs/Amplify/AmplifyCharacter.smsh"
                                    },
                                    new Property()
                                    {
                                        Path = "m_pSkeleton",
                                        Value = "data://Packs/Amplify/AmplifySkeleton.skel"
                                    },
                                    new Property()
                                    {
                                        Path = "m_transform",
                                        Value = characterTransform
                                    }
                                }
                    };

                    var animationComponent = new EntityComponent()
                    {
                        Name = "SimpleAnimation",
                        TypeID = "KRG::Animation::AnimationClipPlayerComponent",
                        Properties = new List<Property>()
                        {
                            new Property()
                            {
                                Path = "m_pAnimation",
                                Value = animOptions[random.Next( 0, animOptions.Length )]
                            }
                        }
                    };

                    //------------------------------------------------------------------------

                    var characterEntity = new Entity()
                    {
                        Name = string.Format( "Character {0}", characterNumber ),
                        Components = new List<EntityComponent>()
                        {
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
