import bpy
import math
import uuid
import os
import json
from json import JSONEncoder
from mathutils import *
from math import degrees

#-------------------------------------------------------------------------
class Property:
    def __init__( self, path, value ):
        self.Path = path
        self.Value = value

class NavmeshComponent:
    def __init__(self):
        self.ID = uuid.uuid4()
        self.Name = "NavmeshComponent"
        self.TypeData = { "TypeID" : "KRG::Navmesh::NavmeshComponent" }

class PhysicsMeshComponent:
    def __init__(self, name):
        self.ID = uuid.uuid4()
        self.Name = name
        self.TypeData = { "TypeID" : "KRG::Physics::PhysicsMeshComponent" }

class StaticMeshComponent:
    def __init__(self, name):
        self.ID = uuid.uuid4()
        self.Name = name
        self.TypeData = { "TypeID" : "KRG::Render::StaticMeshComponent" }

class Entity:
    def __init__(self, name):
        self.Name = name
        self.ID = uuid.uuid4()
        self.Components = []
        self.Systems = []

class EntityMap:
    def __init__(self):
        self.Entities = []

#-------------------------------------------------------------------------

class MapEncoder(JSONEncoder):
    def default(self, obj):
        if isinstance(obj, EntityMap):
            return obj.Entities
        if isinstance(obj, Entity):
            return obj.__dict__
        if isinstance(obj, StaticMeshComponent):
            return obj.__dict__
        if isinstance(obj, PhysicsMeshComponent):
            return obj.__dict__
        if isinstance(obj, NavmeshComponent):
            return obj.__dict__
        if isinstance(obj, Property):
            return { obj.Path: obj.Value }
        if isinstance(obj, uuid.UUID):
            return str(obj)
        else:
            return json.JSONEncoder.default(self, obj)

#-------------------------------------------------------------------------

D = bpy.data
C = bpy.context
sce = C.scene
ob_list = sce.objects

# Now copy the coordinate of the mouse as center of rotation
try:

    outFileName = bpy.path.basename(bpy.context.blend_data.filepath)
    outFileName = os.path.splitext(outFileName)[0]+'.map'
    outFilePath = 'D:\\Kruger\\Data\\maps\\' + outFileName
    print ( outFilePath )
    outfile = open(outFilePath, 'w')

    # Create map
    entityMap = EntityMap()

    # Add navmesh
    navmeshEntity = Entity( "NavmeshEntity" )
    navmeshEntity.Components.append( NavmeshComponent() )
    entityMap.Entities.append( navmeshEntity )

    # Create scene entities
    for ob in ob_list:

        if ob.name != "Camera" and ob.name != "Lamp" and ob.type == 'MESH':

            mesh = ob.data

            if "container_door" in mesh.name.lower() or "portaloo_01_door" in mesh.name.lower() or "bld_door" in mesh.name.lower() or "_glass" in mesh.name.lower() or "_lid" in mesh.name.lower():
                continue

            loc, rot, sca = ob.matrix_world.decompose()
            tx, ty, tz = loc # unpack the ob.loc tuple for printing
            rx, ry, rz = tuple(map(degrees, ob.matrix_world.to_euler())) # unpack the ob.rot tuple for printing
            sx, sy, sz = sca * 100 # unpack the ob.rot tuple for printing

            meshDataPath = "data://Packs/BR/Environment/{}.msh".format( mesh.name )
            physicsGeoDataPath = 'data://Packs/BR/Environment/{}.pmsh'.format( mesh.name )

            if "sm_veh" in mesh.name.lower():
                meshDataPath = "data://Packs/BR/Vehicles/{}.msh".format( mesh.name )
                physicsGeoDataPath = 'data://Packs/BR/Vehicles/{}.pmsh'.format( mesh.name )

            if "01_wires" in mesh.name.lower() or "02_wires" in mesh.name.lower():
                meshDataPath = meshDataPath.replace("_Wires", "")
                physicsGeoDataPath = physicsGeoDataPath.replace("_Wires", "")

            transformString = "{:.4f}, {:.4f}, {:.4f}, {:.4f}, {:.4f}, {:.4f}, {:.4f}, {:.4f}, {:.4f}".format( rx, ry, rz, tx, ty, tz, sx, sy, sz )

            entity = Entity( ob.name )

            # Create mesh component
            meshComponent = StaticMeshComponent("mesh_" + ob.name )
            meshComponent.TypeData[ "m_pMesh" ] = meshDataPath
            meshComponent.TypeData[ "m_transform"] = transformString

            if "road" in mesh.name.lower():
                meshComponent.TypeData["m_materials/0"] = "data://Packs/BR/Materials/PolygonBattleRoyale_Road_01.mtrl"
            elif "bridge" in mesh.name.lower():
                meshComponent.TypeData["m_materials/0"] = "data://Packs/BR/Materials/PolygonBattleRoyale_Road_01.mtrl"
            elif "destroyed" in mesh.name.lower():
                meshComponent.TypeData["m_materials/0"] = "data://Packs/BR/Materials/PolygonBattleRoyale_Vehicles_01_Damaged.mtrl"
            elif "vehicle" in mesh.name.lower():
                meshComponent.TypeData["m_materials/0"] = "data://Packs/BR/Materials/PolygonBattleRoyale_Vehicles_01.mtrl"
            else:
                meshComponent.TypeData["m_materials/0"] = "data://Packs/BR/Materials/PolygonBattleRoyale_Texture_01_A.mtrl"

            entity.Components.append( meshComponent )

            # Create physics component
            physicsComponent = PhysicsMeshComponent( "phys_" + ob.name )
            physicsComponent.SpatialParent = meshComponent.ID
            physicsComponent.TypeData[ "m_pPhysicsMesh"] = physicsGeoDataPath
            entity.Components.append( physicsComponent )

            entityMap.Entities.append( entity )

    # Write to file
    json.dump( entityMap.__dict__, outfile, indent=4, cls=MapEncoder)

except Exception as e:
    print ("Oh no! something went wrong:", e)

else:
    if outfile: outfile.close()
    print("done writing")