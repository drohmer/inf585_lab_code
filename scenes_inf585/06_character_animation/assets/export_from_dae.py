import xmltodict #[pip3] install xmltodict
import json
import pprint
import numpy
import os
import argparse

numpy.set_printoptions(precision=3,suppress=True)

def reshape_array(array_input, element_size):
    assert len(array_input)%element_size==0
    N_element = int(len(array_input)/element_size)
    return numpy.reshape(array_input, (N_element,element_size))

def reshape_mat4(L):
    assert len(L)==16
    v = [float(e) for e in L]
    return numpy.array([[v[0],v[1],v[2],v[3]], [v[4],v[5],v[6],v[7]], [v[8],v[9],v[10],v[11]], [v[12],v[13],v[14],v[15]]])

def collada_parse_single_mesh(entry, name):
    mesh = {}
    for source in entry['source']:
        source_name = source['@name']
        if source_name == 'position':
            mesh['position'] = reshape_array(source['float_array']['#text'].split(), 3)
        if source_name == 'normal':
            mesh['normal'] = reshape_array(source['float_array']['#text'].split(), 3)
        if source_name == 'map1':
            mesh['map1'] = reshape_array(source['float_array']['#text'].split(), 2)

    connectivity = entry['polylist']['p'].split()
    mesh['connectivity'] = reshape_array(connectivity, 9) # assume we read: position, normal, uv

    return mesh

def collada_parse_meshes(geometries):

    meshes = {}
    if type(geometries) is list:
        for geometry in geometries:
            name = geometry['@id']
            meshes[name] = collada_parse_single_mesh(geometry['mesh'],name)
    else:
        name = geometries['@id']
        meshes[name] = collada_parse_single_mesh(geometries['mesh'],name)
    
    return meshes
        

def display_mesh_info(meshes):
    print(f'\nFound {len(meshes)} mesh')
    for name in meshes:
        N_position = len(meshes[name]['position'])
        N_normal = len(meshes[name]['normal'])
        N_uv = len(meshes[name]['map1'])
        print(f'  - {name}: [{N_position} vertex] [{N_normal} normal] [{N_uv} uv]')

def name_mesh_to_directory(mesh_name):
    return 'mesh-'+mesh_name.replace('-lib','').lower()

def export_mesh(meshes, dir_out):
    for name in meshes:
        mesh_name = name_mesh_to_directory(name)
        if not os.path.isdir(dir_out+mesh_name):
            os.mkdir(dir_out+mesh_name)
        
        with open(dir_out+mesh_name+'/mesh.obj', 'w') as fid:
            for p in meshes[name]['position']:
                fid.write(f'v {p[0]} {p[1]} {p[2]}\n')
            
            for n in meshes[name]['normal']:
                fid.write(f'vn {n[0]} {n[1]} {n[2]}\n')

            for uv in meshes[name]['map1']:
                fid.write(f'vt {uv[0]} {uv[1]}\n')

            for t in meshes[name]['connectivity']:
                t = [int(e)+1 for e in t]
                txt = "f {}/{}/{} {}/{}/{} {}/{}/{}\n".format(t[0],t[2],t[1],t[3],t[5],t[4],t[6],t[8],t[7])
                fid.write(txt)

def collada_parse_skeleton_hierarchy(collada_node):

    root_node = collada_node[0]
    visited_node = []
    node_to_visit = [(root_node,'-1')]

    joint_name = []
    joint_parent_name = []
    joint_matrix = []

    while len(node_to_visit)>0:
        node,current_parent_name = node_to_visit.pop(0)
        current_name = node['@name']
        joint_name.append(current_name)
        joint_parent_name.append(current_parent_name)

        M =  reshape_mat4(node['matrix']["#text"].split())
        joint_matrix.append(M)

        if 'node' in node:
            if isinstance(node['node'],list):
                for n in node['node']:
                    node_to_visit.append( (n,current_name) )
            else:
                node_to_visit.append( (node['node'],current_name) )
        visited_node.append(node)

    skeleton_structure = {'joint_name':joint_name,'joint_parent_name':joint_parent_name,'joint_matrix':joint_matrix}
    joint_name_to_index = {}
    joint_name_to_index['-1'] = None
    for k,name in enumerate(skeleton_structure['joint_name']):
        joint_name_to_index[name] = k
    skeleton_structure['joint_name_to_index'] = joint_name_to_index

    return skeleton_structure

def display_skeleton_structure_info(skeleton_structure):
    N_joint = len(skeleton_structure['joint_name'])
    print(f'\nFound skeleton with {N_joint} joints')
    for name in skeleton_structure['joint_name']:
        idx = skeleton_structure['joint_name_to_index'][name]
        parent_name = skeleton_structure['joint_parent_name'][idx]
        idx_parent = skeleton_structure['joint_name_to_index'][parent_name]
        print(f'  - Joint "{name}" ({idx}) is child of "{parent_name}" ({idx_parent})')

def export_skeleton_structure(skeleton_structure, dir_out):
    
    with open(dir_out+'skeleton_joint_name.txt','w') as fid :
        for name in skeleton_structure['joint_name']:
            fid.write(name+'\n')
    with open(dir_out+'skeleton_parent_index.txt','w') as fid :
        for parent_name in skeleton_structure['joint_parent_name']:
            idx = skeleton_structure['joint_name_to_index'][parent_name]
            if idx==None:
                idx = -1
            fid.write(f'{idx} ')
    with open(dir_out+'skeleton_joint_matrix.txt','w') as fid :
        for m in skeleton_structure['joint_matrix']:
            for k1 in range(4):
                for k2 in range(4):
                    fid.write(f'{m[k1][k2]} ')
            fid.write('\n')

def collada_parse_single_controller_skinning(controller, skeleton_structure):
    
    skinning = {}

    bind_matrix = controller['skin']['bind_shape_matrix'].split()
    skinning['bind_matrix'] = bind_matrix

    for source in controller['skin']['source']:
        if source['@id'].endswith('-Joints'):
            skinning_joint_name_vector = source['Name_array']['#text'].split()
        if source['@id'].endswith('-Weights'):
            skinning_weights_vector = [float(v) for v in source['float_array']['#text'].split()]
        if source['@id'].endswith('-Matrices'):
            skinning_inverse_bind_matrix = reshape_array(source['float_array']['#text'].split(), 16) # these are the inverse bind matrix in global frame
    skinning['joint_rig_index_to_skeleton_index'] = [skeleton_structure['joint_name_to_index'][name] for name in skinning_joint_name_vector]
    skinning['joint_name'] = skinning_joint_name_vector
    skinning['inverse_bind_matrix'] = skinning_inverse_bind_matrix
    

    skinning_weight_organization = controller['skin']['vertex_weights']
    dependence_size = [int(k) for k in skinning_weight_organization['vcount'].split()]
    dependence_local_index = [int(k) for k in skinning_weight_organization['v'].split()]

    counter = 0
    skinning['joint'] = []
    skinning['weight'] = []
    for N_dependence in dependence_size:
        joint = []
        weight = []
        for k in range(N_dependence):
            joint.append( dependence_local_index[2*counter+0] )
            weight.append( skinning_weights_vector[dependence_local_index[2*counter+1]])
            counter = counter + 1

        skinning['joint'].append(joint)
        skinning['weight'].append(weight)

    return skinning
    

def collada_parse_controller_skinning(controllers, skeleton_structure):
    controller_skinning = {}
    if type(controllers) is list:
        for controller in controllers:
            geometry_id = controller['skin']['@source'][1:]
            controller_skinning[geometry_id] = collada_parse_single_controller_skinning(controller, skeleton_structure)
    else:
        geometry_id = controllers['skin']['@source'][1:]
        controller_skinning[geometry_id] = collada_parse_single_controller_skinning(controllers, skeleton_structure)
    
    return controller_skinning
   

def export_controller_skinning(controller_skinning, dir_out):
    for mesh_name in controller_skinning:
        dir_name = dir_out+name_mesh_to_directory(mesh_name)+'/'
        entry = controller_skinning[mesh_name]
        with open(dir_name+'skinning_global_bind_matrix.txt','w') as fid:
            B = entry['bind_matrix']
            for k in range(16):
                fid.write(f'{B[k]} ')
        with open(dir_name+'skinning_joint_name_dependence.txt','w') as fid:
            for name in entry['joint_name']:
                fid.write(f'{name}\n')
        with open(dir_name+'skinning_joint_rig_index_to_skeleton_index.txt','w') as fid:
            for k in entry['joint_rig_index_to_skeleton_index']:
                fid.write(f'{k} ')
        with open(dir_name+'skinning_inverse_bind_matrix.txt','w') as fid:
            for M_inv in entry['inverse_bind_matrix']:
                for km in range(16):
                    fid.write(f'{M_inv[km]} ')
                fid.write('\n')
        with open(dir_name+'skinning_joint.txt','w') as fid:
            for dependance_joint in entry['joint']:
                for j in dependance_joint:
                    fid.write(f'{j} ')
                fid.write('\n')
        with open(dir_name+'skinning_weight.txt','w') as fid:
            for dependance_weight in entry['weight']:
                for w in dependance_weight:
                    fid.write(f'{w} ')
                fid.write('\n')


def collada_parse_animation(collada_animation, skeleton_structure):

    animation_data = {}
    for k,joint_animation in enumerate(collada_animation):
        joint_name = joint_animation['@name']
        joint_index = skeleton_structure['joint_name_to_index'][joint_name]
 
        for entry in joint_animation['source']:
            if entry['@id'].endswith('-input'):
                animation_time = entry['float_array']['#text'].split()
            if entry['@id'].endswith('-output-transform'):
                animation_matrix = reshape_array(entry['float_array']['#text'].split(), 16)

        animation_data[joint_index]={'time_input':animation_time, 'matrix':animation_matrix}
    return animation_data

def export_skeleton_animation(skeleton_animation, dir_out):
    dir_anim = dir_out+'animation/'
    if not os.path.isdir(dir_anim):
        os.mkdir(dir_anim)
    
    with open(dir_anim+'skeleton_animation_joint_index.txt', 'w') as fid:
        for idx in skeleton_animation:
            fid.write(f'{idx} ')
    with open(dir_anim+'skeleton_animation_timing.txt', 'w') as fid:
        for idx in skeleton_animation:
            for t in skeleton_animation[idx]['time_input']:
                fid.write(f'{t} ')
            fid.write('\n')
    with open(dir_anim+'skeleton_animation_matrix.txt', 'w') as fid:
        for idx in skeleton_animation:
            for M in skeleton_animation[idx]['matrix']:
                for km in range(16):
                    fid.write(f'{M[km]} ')
            fid.write('\n')


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description='Export dae animation into ascii files')
    parser.add_argument('filename', help='The path to the input .dae file.')
    parser.add_argument('-o','--output', help='Output directory. Default=out/')
    args = parser.parse_args()


    filename_in = args.filename
    dir_out = 'out/'
    if args.output != None:
        dir_out = args.output
    
    if not os.path.isfile(filename_in):
        print('Error, cannot find input file',filename_in)
        exit(0)

    if not os.path.isdir(dir_out):
        os.mkdir(dir_out)
   
    with open(filename_in) as fd:
        collada = xmltodict.parse(fd.read())
    collada = collada['COLLADA']

    meshes = collada_parse_meshes(collada['library_geometries']['geometry'])
    display_mesh_info(meshes)


    skeleton_structure = collada_parse_skeleton_hierarchy(collada['library_visual_scenes']['visual_scene']['node'])
    display_skeleton_structure_info(skeleton_structure)

    controller_skinning = collada_parse_controller_skinning(collada['library_controllers']['controller'], skeleton_structure)

    skeleton_animation = collada_parse_animation(collada['library_animations']['animation'], skeleton_structure)


    export_mesh(meshes, dir_out)
    export_skeleton_structure(skeleton_structure, dir_out)
    export_controller_skinning(controller_skinning, dir_out)
    export_skeleton_animation(skeleton_animation, dir_out)