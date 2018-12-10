import math
import numpy as np

"""Agrandit un tableau 2D en rajoutant des zeros à droite et en bas"""
def grow(array, growX = 0, growY = 0) :

    if (array.ndim != 2) :
        raise ValueError("Le tableau doit être de dimension 2")

    sx, sy = array.shape
    
    hgrow = np.zeros(sx * growY).reshape(sx, growY)
    vgrow = np.zeros(growX * (sy + growY)).reshape(growX, sy + growY)

    array = np.hstack((array, hgrow))
    array = np.vstack((array, vgrow))

    return array
