# -*- coding: utf-8 -*-

from __future__ import absolute_import

WORLD_LOCATION = "d:\\Utilisateurs\\Louis\\Documents\\Louis JEAN\\programmation\\Python\\world"

import sys
sys.path.append(WORLD_LOCATION)
from core import terrain

import bpy

"""crée un mesh blender correspondant au terrain passé en
paramètres et l'ajoute à la scène"""
def createMeshFast(pattern, size = 1) :
    array = pattern.array
    sizeX, sizeY = array.shape
    
    mode_set = bpy.ops.object.mode_set
    if mode_set.poll() :
        mode_set(mode = 'OBJECT')
    
    bpy.context.scene.cursor_location = (0.0, 0.0, 0.0)
    bpy.ops.object.select_all(action = 'DESELECT')

    # L'objet ajouté dans un futur proche s'appelle Grid. On supprime donc
    # l'objet Grid préexistant dans la scène, s'il y en a un.
    if 'Grid' in bpy.data.objects.keys() :
        bpy.data.objects['Grid'].select = True
        bpy.ops.object.delete()

    # Création du nouveau mesh.
    bpy.ops.mesh.primitive_grid_add(x_subdivisions=sizeX, y_subdivisions=sizeY)
    
    grid = bpy.data.objects['Grid']
    gridmesh = grid.data
    
    for x in range(0, sizeX) :
        for y in range(0, sizeY) :
            ptX, ptY = (x / sizeX - 0.5) * size, (y / sizeY - 0.5) * size
            ptZ = array[x, y] / 2
            
            gridmesh.vertices[x * sizeY + y].co[2] = ptZ

    # Recalculate normals
    mode_set(mode = 'EDIT')
    bpy.ops.mesh.select_all(action = 'TOGGLE')
    bpy.ops.mesh.normals_make_consistent()

    # Dernière touche
    mode_set(mode = 'OBJECT')
    grid.select = True
    bpy.ops.object.shade_smooth()

def exportSelection(path) :
    bpy.ops.export_scene.obj(use_selection=True, filepath=path)

    ##### TESTS #####

def test() :
    pattern = terrain.generate_diagonal_relief(terrain.Config(size=129))
    createMeshFast(pattern)
    exportSelection(WORLD_LOCATION + "\\tests\\terrain.obj")
    
def test2() :
    pattern = terrain.loadFromImage(WORLD_LOCATION + "\\tests\\test2.png")
    createMeshFast(pattern)
    
test2()
