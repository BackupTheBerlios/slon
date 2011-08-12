import c4d
import os
import sys
import time
import datetime
import traceback

from c4d import plugins, documents, utils, gui
from c4d.plugins import GeLoadString

folder = os.path.dirname(__file__)
if folder not in sys.path:
    sys.path.insert(0, folder)
    sys.path.insert(0, folder+"/slon")
   
from slon import database
from slon import graphics
from slon import math
from slon import scene
    
#be sure to use a unique ID obtained from 'plugincafe.com'
PLUGIN_ID = 1025282
    
def indent(elem, level=0):
    i = "\n" + level*"\t"
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "\t"
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        for elem in elem:
            indent(elem, level+1)
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i
    return
    
def dumpVector(v):
    return "%f %f %f" % (v.x, v.y, v.z)
    
def convertMatrix(m):
    mat = math.Matrix4f()
    
    mat[0] = math.VectorRow4f(m.v1.x, m.v2.x, m.v3.x, m.off.x)
    mat[1] = math.VectorRow4f(m.v1.y, m.v2.y, m.v3.y, m.off.y)
    mat[2] = math.VectorRow4f(m.v1.z, m.v2.z, m.v3.z, m.off.z)
    mat[3] = math.VectorRow4f(     0,      0,      0,       1)
    
    return mat
    
def dumpVectorArray(vecs):
    text = ""
    for vec in vecs:
        text += "%f %f %f " % (vec.x, vec.y, vec.z)
    return text
            
def dumpArray(arr):
    text = ""
    for v in arr:
        text += str(v) + " "
    return text
    
class SlonExporter(plugins.SceneSaverData):
        
    def __init__(self):
        self.library = database.Library()
        self.materials = set()
        
    def dumpMaterial(self, material):
        if material.GetName() in self.materials:
            return material.GetName()
        
        materialElem = etree.SubElement(self.graphicsEl, "material", name=material.GetName(), effect="phong_lighting")
        etree.SubElement(materialElem, "color", type="vec3f", value=dumpVector(material.GetAverageColor(c4d.CHANNEL_COLOR)))
        etree.SubElement(materialElem, "diffuse", type="vec3f", value=dumpVector(material.GetAverageColor(c4d.CHANNEL_DIFFUSION)))
        
        self.materials.add(material.GetName())
        return material.GetName()
        
    def dumpMesh(self, parentEl, polygonObj):
        # extract vertices and indices
        positions = polygonObj.GetAllPoints()
        indices = []
        for poly in polygonObj.GetAllPolygons():
            if hasattr(poly, "d"):
                indices.extend([poly.a, poly.b, poly.c, poly.a, poly.c, poly.d])
            else:
                indices.extend([poly.a, poly.b, poly.c])
                
        meshEl = etree.SubElement(parentEl, "Mesh", vcount=str(len(positions)), icount=str(len(indices)))
        etree.SubElement(meshEl, "attribute", size="3", semantic="position", type="float").text = dumpVectorArray(positions)
        etree.SubElement(meshEl, "indices", type="uint32").text = dumpArray(indices)
       
        # extract effect
        effectName = "default"
        textureTag = polygonObj.GetTag(c4d.Ttexture)
        if textureTag and textureTag.GetMaterial():
            effectName = self.dumpMaterial(textureTag.GetMaterial())
        subsetEl = etree.SubElement(meshEl, "subsets")
        etree.SubElement(subsetEl, "indexed_subset", primitiveType="TRIANGLES", startIndex="0", numIndices=str(len(indices)), effect=effectName)
        
    def dumpPolygonObject(self, parentEl, polygonObj):
        polyEl = etree.SubElement(parentEl, "StaticMesh")
        self.dumpMesh(polyEl, polygonObj)
        
    def dumpLightObject(self, parentEl, node):
        return
        
    def convertNode(self, c4dNode):
        if not c4dNode: 
            return None
        
        transform = scene.MatrixTransform( c4dNode.GetName(), convertMatrix(c4dNode.GetRelMl()) )
        #if (node.GetType() == c4d.Opolygon): 
        #    self.dumpPolygonObject(nodeEl, node)
        #elif (node.GetType() == c4d.Olight): 
        #    self.dumpLightObject(nodeEl, node)
        
        c4dChildObj = c4dNode.GetDown()
        while c4dChildObj != None:
            transform.addChild( self.convertNode(c4dChildObj) )
            c4dChildObj = c4dChildObj.GetNext()
            
        return transform

    def dumpPhysics(self, node):
        if not node: return
        
        dynTag = node.GetTag(180000102)
        if dynTag:
            rbEl = etree.SubElement(self.physicsEl, "RigidBody")
            etree.SubElement(rbEl, "mass").text = str(dynTag[c4d.RIGID_BODY_MASS])
            etree.SubElement(rbEl, "margin").text = str(dynTag[c4d.RIGID_BODY_MARGIN])
            etree.SubElement(rbEl, "target").text = node.GetName()
            etree.SubElement(rbEl, "shape").text = str(dynTag[c4d.RIGID_BODY_SHAPE])
            etree.SubElement(rbEl, "ref_shape").text = str(dynTag[c4d.RIGID_BODY_REFERENCE_SHAPE])
        
        self.dumpPhysics(node.GetDown())
        self.dumpPhysics(node.GetNext())

    def writeToFile(self, name):
        database.saveLibrary(name, self.library)
        return c4d.FILEERROR_NONE
        
    def Save(self, c4dNode, fileName, c4dDocument, filterflags):
        root = scene.Group()
        c4dObj = c4dDocument.GetFirstObject()
        while c4dObj != None:
            root.addChild( self.convertNode(c4dObj) )
            c4dObj = c4dObj.GetNext()
        #self.dumpPhysics(doc.GetFirstObject())
        self.library.visualScenes["root"] = root
        
        return self.writeToFile(fileName)

if __name__=='__main__':
    plugins.RegisterSceneSaverPlugin(id=PLUGIN_ID, str="SLON exporter (*.sxml)", info=0, g=SlonExporter, description="", suffix="sxml")
