import bpy
import os

def export_custom_fbx(preset):
    filename = "_".join(s.lower() for s in preset.split())
    preset_path = bpy.utils.preset_paths('operator\\export_scene.fbx\\')

    if preset_path:
        filepath = os.path.join(preset_path[0], preset)
        if filepath:
            class Container(object):
                __slots__ = ('__dict__',)

            op = Container()
            file = open(filepath, 'r')

            # storing the values from the preset on the class
            for line in file.readlines()[3::]:
                exec(line, globals(), locals())

            # pass class dictionary to the operator
            kwargs = op.__dict__
            bpy.ops.export_scene.fbx(**kwargs)

        else:
            raise FileNotFoundError(preset_path[0] + filename + '.py')

export_custom_fbx('Amplify')