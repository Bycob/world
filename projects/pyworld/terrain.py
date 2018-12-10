
from __future__ import absolute_import

#système
import sys
import math
#libs externes
import numpy as np
from PIL import Image


DEBUG = False

"""Configuration des paterns. Contients toute l'information nécessaire pour
générer un pattern de terrain."""
class Config :

    def __init__(self, size = 513, slope_max = 0.05) :
        self.size = size

        self.slope_max = slope_max
#end

"""Pattern de terrain : ce pattern contient les valeurs des altitudes qui décrivent
un terrain. Le terrain est une surface du monde plus ou moins accidentée. Elle ne
contient pas de grotte ou de cavité horizontale.
La taille d'un pattern doit être de la forme 2^n + 1."""
class Pattern :
    def __init__(self, config) :
        self.config = config
        
        size = config.size
        self.array = np.zeros(size * size).reshape(size, size)

    def _set_config(self, config) :
        self.config = config
        
        size = config.size
        self.array = np.zeros(size * size).reshape(size, size)

    def __str__(self) :
        return str(self.array)
#end

"""Algorithme générant du relief très allongé.
Cet algorithme a été élaboré pendant une phase de recherche, il est peu performant
et n'est pas forcément compatible avec tous les paramètres de l'API."""
def generate_diagonal_relief(config) :
    terrain = Pattern(config)
    
    data = terrain.array
    size = config.size
    
    filled = np.zeros(data.size).reshape(data.shape)
    filled[0, 1:size].fill(1)
    filled[0:size, 0].fill(1)

    data[0, 1:size].fill(0.5)
    data[0:size, 0].fill(0.5)
    
    #coef de pente par pixel
    c = config.slope_max
    
    for y in range(1, size) :
        for x in range(1, size) :
            #calculs des bornes pour respecter les paramètres
            hmin = 0
            hmax = 1

            if x > 60 and y > 60 and DEBUG :
                print("at", x, y)
            
            for x0 in range(x - 1, min(x + 2, size)) :
                for y0 in range(y - 1, y + 1) :
                    if filled[x0, y0] != 0 :
                        dist = math.sqrt((x-x0) **2 + (y-y0) **2)
                        locmin = data[x0, y0] - dist * c
                        locmax = data[x0, y0] + dist * c

                        if x > 60 and y > 60 and DEBUG :
                            print(x0, y0, ":", locmin, locmax)

                        if locmin > hmin :
                            hmin = locmin
                        if locmax < hmax :
                            hmax = locmax

            if x > 60 and y > 60 and DEBUG :
                print("valeurs choisies :", hmin, hmax, "\n")
            
            hmin = max(0.5 - (size - x) * c, hmin)
            hmax = min(0.5 + (size - x) * c, hmax)
            
            #calcul du nouveau point
            data[x, y] = hmin + np.random.random() * (hmax - hmin)
            filled[x, y] = 1
    
    return terrain
#end

"""Permet d'écrire le décor passé en paramètres sous la forme d'une image, au
chemin indiqué."""
def writeImage(pattern, path) :
    canvas = pattern.array
    img = Image.new("L", pattern.array.shape)

    for i in range(0, canvas.shape[0]) :
        for j in range(0, canvas.shape[1]) :
            v = int(canvas[i][j] * 256)
            img.putpixel((i, j), (v))

    img.save(path)
    img.close()
#end

def loadFromImage(path) :
    img = None
    try :
        img = Image.open(path, 'r')
    except Exception :
        print("échec de la lecture")
        return None

    size = max(img.size)
    terrain = Pattern(Config(size=size))
    array = terrain.array

    for x in range(0, size) :
        for y in range(0, size) :
            
            if x > img.size[0] or y > img.size[1] :
                array[x, y] = 1
            else :
                pix = img.getpixel((x, y))
                
                #image en noir et blanc
                if isinstance(pix, int) :
                    array[x, y] = pix / 255

                #image en couleur -> conversion
                elif isinstance(pix, tuple) :
                    pixmoy = 0
                    l = len(pix)
                    for i in pix :
                        pixmoy += i / 255 / l
                    array[x, y] = pixmoy
                else :
                    print("echec de la lecture : type d'image inconnu")

    return terrain
#end

    
def test() :
    pattern0 = generate_diagonal_relief(Config())
    writeImage(pattern0, "../tests/terrain.png")
    pattern1 = loadFromImage("../tests/terrain.png")
    print(pattern1)

#test()
