#######################################################################
#
# CPSC489 Game Development Project
# Copyright (c) 2018  Steven Emory <mobysteve@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#######################################################################

#
# PYTHON IMPORTS
#
import os
import re

#
# BLENDER IMPORTS
#
import bpy
import mathutils

##
#  @brief   Required for all plugins.
#  @details A descriptor dictionary that is used by Blender to display information about the plugin,
#  such as the name, author, and version of the plugin.
#
bl_info = {
'name': 'Export World UTF',
'author': 'Steven Emory',
'version': (1, 0),
'blender': (2, 79, 0),
'location': 'File > Export > World UTF (.txt)',
'description': 'Exports a World UTF file.',
'warning': '',
'category': 'Import-Export',
}

class StaticModel:
    # group     - string
    # link_path - string
    # link_file - string
    pass
class DynamicModel:
    # group     - string
    # link_path - string
    # link_file - string
    pass
class ModelInstance:
    # model
    # position
    # rotation
    pass
class SoundItem:
    # link_path - string
    # link_file - string
    pass
class BoundingBox:
    # position - vector3
    # rotation - vector4
    # halfdims - vector3
    pass
class CameraMarker:
    # position          - vector3
    # rotation          - matrix4
    # euler_angle       - vector3
    # index             - uint16
    # speed             - real32
    # interpolate_speed - bool
    # fovy              - real32
    # interpolate_fovy  - bool
    pass
class CameraAnimation:
    # name        - string
    # position    - vector3
    # rotation    - matrix4
    # start       - uint16
    # markers     - CameraMarker[]
    pass
class DoorController:
    # name         - string
    # bbox         - BoundingBox
    # door         - string
    # anim_default - string
    # anim_enter   - string
    # anim_leave   - string
    # sound_enter  - string
    # sound_leave  - string
    pass

def IsMeshObject(obj):
    return (True if obj.type == 'MESH' else False)
def IsMeshGroup(obj):
    if obj.type != 'EMPTY': return False
    if obj.dupli_type != 'GROUP': return False
    if obj.dupli_group is None: return False
    return True
def IsMeshGroup(obj, n):
    if obj.type != 'EMPTY': return False
    if obj.dupli_type != 'GROUP': return False
    if obj.dupli_group is None: return False
    if len(obj.dupli_group.objects) != n: return False
    return True
def GetMeshGroupObjects(obj):
    if IsMeshGroup(obj) == False: return None
    return obj.dupli_group.objects

##
#  @brief   Gets rid of annoying small values.
#  @details 
def ClearVector(v):
    for i in range(len(v)):
        if abs(v[i]) < 1.0e-5: v[i] = 0.0
def ClearMatrix(m):
    for r in range(4):
        for c in range(4):
            if abs(m[r][c]) < 1.0e-5: m[r][c] = 0.0

#region DOOR CONTROLLER FUNCTIONS

##
#  @brief   Queries Blender object.
#  @details 
def IsDoorController(obj):

    # get mesh object from group
    if IsMeshGroup(obj, 1) == False: return False
    mesh_object = obj.dupli_group.objects[0]

    # 1st try - check custom property
    if mesh_object is None: return False
    if 'entity_type' in mesh_object:
        entity_type = mesh_object['entity_type']
        return entity_type == 'DOOR_CONTROLLER'

    # 2nd try - check object name
    list = mesh_object.name.split('_')
    if (len(list) and list[0] == 'DC'): return True

    return False

#endregion DOOR CONTROLLER FUNCTIONS

##
#  @brief 
#  @details
class WorldUTFExporter:

    mapname     = None
    export_path = None
    export_name = None
    export_fext = None
    filename = None
    file = None

    # data lists
    static_models = []
    dynamic_models = []
    soundlist = []

    # instance lists
    smi_list = []
    dmi_list = []

    # entity lists
    cam_list = []
    door_controllers  = []

    ##
    #  @brief
    #  @details 
    def __init__(self):
        pass

    ##
    #  @brief
    #  @details 
    def WriteInt(self, v): self.file.write('{}\n'.format(int(v)))

    ##
    #  @brief
    #  @details 
    def WriteFloat(self, v): self.file.write('{}\n'.format(float(v)))

    ##
    #  @brief
    #  @details 
    def WriteString(self, s): self.file.write('{}\n'.format(str(s)))

    ##
    #  @brief
    #  @details 
    def WriteVector3(self, v): self.file.write('{} {} {}\n'.format(v[0], v[1], v[2]))

    ##
    #  @brief
    #  @details 
    def WriteVector4(self, v): self.file.write('{} {} {} {}\n'.format(v[0], v[1], v[2], v[3]))

    ##
    #  @brief
    #  @details 
    def WriteMatrix4(self, m):
        self.file.write('{} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}\n'.format(
            m[0][0], m[0][1], m[0][2], m[0][3],
            m[1][0], m[1][1], m[1][2], m[1][3],
            m[2][0], m[2][1], m[2][2], m[2][3],
            m[3][0], m[3][1], m[3][2], m[3][3]))

    ##
    #  @brief
    #  @details 
    def execute(self):

        # build export path
        if 'export_path' in bpy.context.scene: self.export_path = bpy.context.scene['export_path']
        else: self.export_path = os.path.dirname(bpy.data.filepath) + '\\'

        # build export name
        if 'export_name' in bpy.context.scene: self.export_name = bpy.context.scene['export_name']
        else: self.export_name = os.path.splitext(os.path.basename(bpy.data.filepath))[0]

        # build export file extension
        if 'export_fext' in bpy.context.scene: self.export_fext = bpy.context.scene['export_fext']
        else: self.export_fext = 'txt'

        # create file for writing
        self.filename = self.export_path + self.export_name + '.' + self.export_fext
        print(self.filename)
        self.file = open(self.filename, 'w')

        # save map name
        if 'mapname' in bpy.context.scene: self.mapname = bpy.context.scene['mapname']
        else: self.export_name = 'default'
        self.WriteString(self.mapname)

        # examine objects
        prev_mode = bpy.context.mode
        if bpy.context.mode != 'OBJECT': bpy.ops.object.mode_set(mode='OBJECT')
        self.ExamineObjects()
        if prev_mode != bpy.context.mode: bpy.ops.object.mode_set(mode=prev_mode)

        # export entities
        self.ExportDoorControllers()

    ##
    #  @brief
    #  @details 
    def ExamineObjects(self):

        # process static models
        for object in bpy.data.objects:
            if 'entity_type' not in object: continue
            entity_type = object['entity_type']
            if entity_type == 'STATIC_MODEL_LIST': self.ProcessStaticModelList(object)

        # process dynamic models
        for object in bpy.data.objects:
            if 'entity_type' not in object: continue
            entity_type = object['entity_type']
            if entity_type == 'DYNAMIC_MODEL_LIST': self.ProcessDynamicModelList(object)

        # process sound and music files
        for object in bpy.data.objects:
            if 'entity_type' not in object: continue
            entity_type = object['entity_type']
            if entity_type == 'SOUND_LIST': self.ProcessSoundList(object)

        # process sound and music files
        for object in bpy.data.objects:
            if 'entity_type' not in object: continue
            entity_type = object['entity_type']
            if entity_type == 'CAMERA_ANIMATION': self.ProcessCameraAnimation(object)

        # process static model instances
        for object in bpy.data.objects:
            if 'entity_type' not in object: continue
            entity_type = object['entity_type']
            if entity_type == 'STATIC_MODEL_INSTANCES': self.ProcessStaticModelInstances(object)

        # process dynamic model instances
        for object in bpy.data.objects:
            if 'entity_type' not in object: continue
            entity_type = object['entity_type']
            if entity_type == 'DYNAMIC_MODEL_INSTANCES': self.ProcessDynamicModelInstances(object)

        # save static models
        self.WriteString('###')
        self.WriteString('### STATIC MODELS')
        self.WriteString('###')
        n = len(self.static_models)
        self.WriteString('{} # number of static models'.format(n))
        for item in self.static_models: self.WriteString(item.link_path + '\\' + item.link_file)

        # save dynamic models
        self.WriteString('###')
        self.WriteString('### DYNAMIC MODELS')
        self.WriteString('###')
        n = len(self.dynamic_models)
        self.WriteString('{} # number of dynamic models'.format(n))
        for item in self.dynamic_models: self.WriteString(item.link_path + '\\' + item.link_file)

        # save dynamic models
        self.WriteString('###')
        self.WriteString('### SOUNDS')
        self.WriteString('###')
        n = len(self.soundlist)
        self.WriteString('{} # number of sounds'.format(n))
        for item in self.soundlist: self.WriteString(item.link_path + '\\' + item.link_file)

        # save static model instances
        self.WriteString('###')
        self.WriteString('### STATIC MODEL INSTANCESS')
        self.WriteString('###')
        n = len(self.smi_list)
        self.WriteString('{} # number of static model instances'.format(n))
        for item in self.smi_list:
            self.WriteInt(item.model)
            self.WriteVector3(item.position)
            self.WriteMatrix4(item.rotation)

        # save dynamic model instances
        self.WriteString('###')
        self.WriteString('### DYNAMIC MODEL INSTANCESS')
        self.WriteString('###')
        n = len(self.dmi_list)
        self.WriteString('{} # number of dynamic model instances'.format(n))
        for item in self.dmi_list:
            self.WriteInt(item.model)
            self.WriteVector3(item.position)
            self.WriteMatrix4(item.rotation)

        # save camera animations
        self.file.write('###\n')
        self.file.write('### CAMERA ANIMATIONS\n')
        self.file.write('###\n')
        n_anim = len(self.cam_list)
        self.file.write('{} # number of camera animations\n'.format(n_anim))
        for cao in self.cam_list:
            self.WriteString(cao.name)
            self.WriteVector3(cao.position)
            self.WriteMatrix4(cao.rotation)
            self.WriteInt(cao.start)
            self.WriteString('{} # of camera markers'.format(len(cao.markers)))
            for marker in cao.markers:
                self.WriteVector3(marker.position)
                self.WriteMatrix4(marker.rotation)
                self.WriteVector3(marker.euler_angle)
                self.WriteInt(marker.index)
                self.WriteFloat(marker.speed)
                self.WriteInt(marker.interpolate_speed)
                self.WriteFloat(marker.fovy)
                self.WriteInt(marker.interpolate_fovy)

        #     if IsDoorController(object): self.ProcessDoorController(object)

    ##
    #  @brief
    #  @details 
    def ProcessStaticModelList(self, object):

        # must be an EMPTY object type
        self.static_models = []
        if object.type != 'EMPTY': raise Exception('Static model lists must be EMPTY Blender objects.')

        # nothing to do
        if len(object.children) == 0: return

        for mdlobj in object.children:
        
            # nothing to do
            if mdlobj.type != 'EMPTY': raise Exception('Static model references must be EMPTY Blender objects.')

            # must have path and file defined
            msg = 'Static model list \"{}\" contains an object \"{}\" with missing \"{}\" declaration.'
            if 'group' not in mdlobj: raise Exception(msg.format(object.name, mdlobj.name, 'group'))
            if 'link_path' not in mdlobj: raise Exception(msg.format(object.name, mdlobj.name, 'link_path'))
            if 'link_file' not in mdlobj: raise Exception(msg.format(object.name, mdlobj.name, 'link_file'))
        
            # otherwise static model is valid
            sm = StaticModel()
            sm.group = mdlobj['group']
            sm.link_path = mdlobj['link_path']
            sm.link_file = mdlobj['link_file']
            self.static_models.append(sm)
        
    ##
    #  @brief
    #  @details 
    def ProcessStaticModelInstances(self, object):

        # must be an EMPTY object type
        self.smi_list = []
        if object.type != 'EMPTY': raise Exception('The static model instance list \"{}\" must be an EMPTY Blender object.'.format(object.name))

        # nothing to do
        if len(object.children) == 0: return

        # create a group map to map names to indices
        gmap = {}
        for i, item in enumerate(self.static_models): gmap[item.group] = i

        # for each child
        for item in object.children:

            # child must be a MESH GROUP
            pf = '{}[{}] '.format(object.name, item.name)
            if item.type != 'EMPTY': raise Exception(pf + 'must be an EMPTY Blender object.')
            if item.dupli_type != 'GROUP': raise Exception(pf + 'must reference a Blender mesh group object.')

            # child must be a validate MESH GROUP object
            group = item.dupli_group
            if group is None: raise Exception(pf + 'must reference a valid Blender mesh group object.')
            if len(group.objects) == 0: raise Exception(pf + 'is a Blender mesh group that contains no meshes.')

            # child must index into the STATIC_MODEL_LIST
            if group.name not in gmap:
                raise Exception(pf + 'must reference a model from a static models list.')

            # must be a STATIC_MODEL
            if 'entity_type' in item:
                if item['entity_type'] != 'STATIC_MODEL':
                    raise Exception(pf + 'is not a static model.')
            else:
                print('Warning: ' + pf + 'is missing the entity_type declaration. Assuming STATIC_MODEL.')

            # otherwise static model is valid
            mi = ModelInstance()
            mi.model = gmap[group.name]
            mi.position = item.matrix_world * item.location
            mi.rotation = item.matrix_world * item.matrix_local
            ClearVector(mi.position)
            ClearMatrix(mi.rotation)
            self.smi_list.append(mi)

    ##
    #  @brief
    #  @details 
    def ProcessDynamicModelList(self, object):

        # must be an EMPTY object type
        self.dynamic_models = []
        if object.type != 'EMPTY': raise Exception('Dynamic model lists must be EMPTY Blender objects.')

        # nothing to do
        if len(object.children) == 0: return

        for mdlobj in object.children:
        
            # nothing to do
            if mdlobj.type != 'EMPTY': raise Exception('Dynamic model references must be EMPTY Blender objects.')

            # must have path and file defined
            msg = 'Dynamic model list \"{}\" contains an object \"{}\" with missing \"{}\" declaration.'
            if 'group' not in mdlobj: raise Exception(msg.format(object.name, mdlobj.name, 'group'))
            if 'link_path' not in mdlobj: raise Exception(msg.format(object.name, mdlobj.name, 'link_path'))
            if 'link_file' not in mdlobj: raise Exception(msg.format(object.name, mdlobj.name, 'link_file'))
        
            # otherwise static model is valid
            dm = DynamicModel()
            dm.group = mdlobj['group']
            dm.link_path = mdlobj['link_path']
            dm.link_file = mdlobj['link_file']
            self.dynamic_models.append(dm)
            print('adding ' + dm.group)
        
    ##
    #  @brief
    #  @details 
    def ProcessDynamicModelInstances(self, object):

        # must be an EMPTY object type
        self.dmi_list = []
        if object.type != 'EMPTY': raise Exception('The dynamic model instance list \"{}\" must be an EMPTY Blender object.'.format(object.name))

        # nothing to do
        if len(object.children) == 0: return

        # create a group map to map names to indices
        gmap = {}
        for i, item in enumerate(self.dynamic_models): gmap[item.group] = i

        # for each child
        for item in object.children:

            # child must be a MESH GROUP
            pf = '{}[{}] '.format(object.name, item.name)
            if item.type != 'EMPTY': raise Exception(pf + 'must be an EMPTY Blender object.')
            if item.dupli_type != 'GROUP': raise Exception(pf + 'must reference a Blender mesh group object.')

            # child must be a validate MESH GROUP object
            group = item.dupli_group
            if group is None: raise Exception(pf + 'must reference a valid Blender mesh group object.')
            if len(group.objects) == 0: raise Exception(pf + 'is a Blender mesh group that contains no meshes.')

            # child must index into the DYNAMIC_MODEL_LIST
            if group.name not in gmap:
                raise Exception(pf + 'must reference a model from a dynamic models list.')

            # must be a DYNAMIC_MODEL
            if 'entity_type' in item:
                if item['entity_type'] != 'DYNAMIC_MODEL':
                    raise Exception(pf + 'is not a dynamic model.')
            else:
                print('Warning: ' + pf + 'is missing the entity_type declaration. Assuming DYNAMIC_MODEL.')

            # otherwise static model is valid
            mi = ModelInstance()
            mi.model = gmap[group.name]
            mi.position = item.matrix_world * item.location
            mi.rotation = item.matrix_world * item.matrix_local
            ClearVector(mi.position)
            ClearMatrix(mi.rotation)
            self.dmi_list.append(mi)

    ##
    #  @brief
    #  @details 
    def ProcessSoundList(self, object):

        # must be an EMPTY object type
        self.soundlist = []
        if object.type != 'EMPTY': raise Exception('Sound lists must be EMPTY Blender objects.')

        # nothing to do
        if len(object.children) == 0:
            print("SOUNDLIST HAS NO CHILDREN")
            return
        else:
            print("SOUNDLIST HAS CHILDREN")

        for child in object.children:
        
            # nothing to do
            if child.type != 'EMPTY': raise Exception('Sound items must be EMPTY Blender objects.')

            # must have path and file defined
            msg = 'Sound list \"{}\" contains an item \"{}\" with missing \"{}\" declaration.'
            if 'link_path' not in child: raise Exception(msg.format(object.name, mdlobj.name, 'link_path'))
            if 'link_file' not in child: raise Exception(msg.format(object.name, mdlobj.name, 'link_file'))
        
            # otherwise static model is valid
            item = SoundItem()
            item.link_path = child['link_path']
            item.link_file = child['link_file']
            self.soundlist.append(item)
 

    ##
    #  @brief
    #  @details 
    def ProcessCameraAnimation(self, object):

        # must be a Plain Axes or Mesh Group object
        self.cam_list = []
        if object.type != 'EMPTY': raise Exception('Camera animation object {} must be an EMPTY Blender object.'.format(object.name))

        # nothing to do
        if len(object.children) == 0: return

        # initialize a camera animation object
        cao = CameraAnimation()
        cao.name = object.name
        cao.position = object.matrix_world * object.location
        cao.rotation = object.matrix_world * object.matrix_local
        cao.start = 0
        cao.markers = []
        ClearVector(cao.position)
        ClearMatrix(cao.rotation)

        # process children
        index = 0
        for item in object.children:

            # child must be an EMPTY GROUP
            pf = '{}[{}] '.format(object.name, item.name)
            if item.type != 'EMPTY': raise Exception(pf + 'must be an EMPTY Blender object.')
            if item.dupli_type != 'GROUP': raise Exception(pf + 'must reference a Blender mesh group object.')

            # child must be a valid GROUP object
            group = item.dupli_group
            if group is None: raise Exception(pf + 'must reference a valid Blender group object.')
            if len(group.objects) == 0: raise Exception(pf + 'is a Blender mesh group that contains no meshes.')

            # must be a DYNAMIC_MODEL
            if 'entity_type' in item:
                if item['entity_type'] != 'CAMERA_MARKER':
                    raise Exception(pf + 'is not a CAMERA_MARKER.')
            else:
                print('Warning: ' + pf + 'is missing the entity_type declaration. Assuming CAMERA_MARKER.')
 
            # initialize a camera animation object
            cmo = CameraMarker()
            cmo.position = item.matrix_world * item.location
            cmo.rotation = item.matrix_world * item.matrix_local
            cmo.euler_angle       = item.rotation_euler
            cmo.index             = index
            cmo.speed             = 1.0
            cmo.interpolate_speed = True
            cmo.fovy              = 60.0
            cmo.interpolate_fovy  = True
            ClearVector(cmo.position)
            ClearMatrix(cmo.rotation)
            ClearVector(cmo.euler_angle)

            # read properties (if present)
            if 'index' in item: cmo.index = int(item['index'])
            if 'speed' in item: cmo.speed = float(item['speed'])
            if 'interpolate_speed' in item: cmo.interpolate_speed = bool(item['interpolate_speed'])
            if 'fovy' in item: cmo.fovy = item['fovy']
            if 'interpolate_fovy' in item: cmo.interpolate_fovy = item['interpolate_fovy']

            # validate properties
            # TODO: finish this

            # append marker object and increment index
            cao.markers.append(cmo)
            index = index + 1

        # append camera animation object only if there are markers
        if len(cao.markers): self.cam_list.append(cao)

    ##
    #  @brief
    #  @details 
    def ProcessDoorController(self, object):

        # must be a Group object
        if IsMeshGroup(object, 1) == False:
            raise Exception('The door controller is a Blender mesh group object.')

        # compute entity bounding box
        min_v = [ object.bound_box[0][0], object.bound_box[0][1], object.bound_box[0][2] ]
        max_v = [ object.bound_box[0][0], object.bound_box[0][1], object.bound_box[0][2] ]
        for p in object.bound_box:
            v = object.matrix_world*mathutils.Vector(p)
            if v[0] < min_v[0]: min_v[0] = v[0]
            if v[1] < min_v[1]: min_v[1] = v[1]
            if v[2] < min_v[2]: min_v[2] = v[2]
            if max_v[0] < v[0]: max_v[0] = v[0]
            if max_v[1] < v[1]: max_v[1] = v[1]
            if max_v[2] < v[2]: max_v[2] = v[2]
        rv = BoundingBox()
        rv.position = object.location
        rv.rotation = object.rotation_quaternion
        rv.halfdims = [(max_v[0] - min_v[0])/2.0, (max_v[1] - min_v[1])/2.0, (max_v[2] - min_v[2])/2.0]

        # initialize entity
        dc = DoorController()
        dc.name         = object.name
        dc.bbox         = rv
        dc.door         = 'door_model'
        dc.anim_default = 'none'
        dc.anim_enter   = 'none'
        dc.anim_leave   = 'none'
        dc.sound_enter  = 'none'
        dc.sound_leave  = 'none'

        # insert entity
        self.door_controllers.append(dc)

    ##
    #  @brief
    #  @details 
    def ExportDoorControllers(self):

        self.file.write('###\n')
        self.file.write('### DOOR CONTROLLERS\n')
        self.file.write('###\n')
        
        # save number of camera animations
        n = len(self.door_controllers)
        self.file.write('{} # number of door controllers\n'.format(n))

        # save camera animations
        for dc in self.door_controllers:

            # save camera animation properties
            self.WriteString(dc.name)
            self.WriteVector3(dc.bbox.position)
            self.WriteVector4(dc.bbox.rotation)
            self.WriteVector3(dc.bbox.halfdims)
            self.WriteString(dc.anim_default)
            self.WriteString(dc.anim_enter)
            self.WriteString(dc.anim_leave)
            self.WriteString(dc.sound_enter)
            self.WriteString(dc.sound_leave)

##
#  @brief   ExportWorldUTF Blender operator.
#  @details Defines the operation(s) to be performed when menu item in Blender is chosen. This
#  operator invokes the file manager and executes an export operation on the chosen filename.
#
class ExportWorldUTFOperator(bpy.types.Operator):

    ##
    #  @brief   bpy.ops.bl_idname identfier.
    #  @details The name of the bpy.ops identifier that is used to invoke this script plugin from
    #  within Blender.
    bl_idname = 'export_world.world_utf'
    
    ##
    #  @brief   Selection dialog label.
    #  @details Required member for fileselect_add to label button in selection dialog.
    bl_label = 'Export'
    
    ##
    #  @brief   Menu item enabler/disabler.
    #  @details Defines conditions for which menu item should be enabled. True = always enabled.
    @classmethod
    def poll(cls, context): return True
    
    ##
    #  @brief   Invoke plugin.
    #  @details Defines what happens immediately after menu item is selected and plugin enters the
    #  invoked state. Simply just calls execute.
    def invoke(self, context, event):
        return self.execute(context)

    ##
    #  @brief   Execute export code.
    #  @details Called by invoke to execute export code.
    def execute(self, context):
        obj = WorldUTFExporter()
        obj.execute()
        return {'FINISHED'}

##
#  @brief   Menu command function.
#  @details Defines what happens when File > Export > World UTF is selected from the main menu.
def ExportWorldUTF_MenuFunction(self, context):
    self.layout.operator_context = 'INVOKE_DEFAULT'
    self.layout.operator(ExportWorldUTFOperator.bl_idname, text='World UTF (.txt)')

# UNCOMMENT REGION IF PLUGIN
# region

# ##
# def register():
#     bpy.utils.register_module(__name__)
#     bpy.types.INFO_MT_file_export.append(ExportWorldUTF_MenuFunction)
# 
# ##
# def unregister():
#     bpy.utils.unregister_module(__name__)
#     bpy.types.INFO_MT_file_export.remove(ExportWorldUTF_MenuFunction)
#     
# ##
# if __name__ == '__main__':
#     register()

# endregion

# UNCOMMENT REGION IF OPERATOR
# region

# # register operator and add to the main menu
# bpy.utils.register_class(ExportWorldUTFOperator)
# bpy.types.INFO_MT_file_export.append(ExportWorldUTF_MenuFunction)

# endregion
    
# UNCOMMENT REGION IF IN PANEL
# region

# register operator and test call
bpy.utils.register_class(ExportWorldUTFOperator)
bpy.ops.export_world.world_utf('INVOKE_DEFAULT')

# endregion