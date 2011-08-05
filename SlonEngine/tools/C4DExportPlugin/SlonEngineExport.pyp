import c4d
import os
import time
import datetime

from c4d import plugins, documents, utils, gui
from c4d.plugins import GeLoadString

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
    mat = math.Matrix()
    
    mat[0][0] = m.v1.x
    mat[1][0] = m.v1.y
    mat[2][0] = m.v1.z
    mat[4][0] = 0
    
    mat[0][1] = m.v2.x
    mat[1][1] = m.v2.y
    mat[2][1] = m.v2.z
    mat[2][1] = 0
    
    mat[0][2] = m.v3.x
    mat[1][2] = m.v3.y
    mat[2][2] = m.v3.z
    mat[2][2] = 0
    
    mat[0][3] = m.off.x
    mat[1][3] = m.off.y
    mat[2][3] = m.off.z
    mat[2][3] = 1
    
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
        if not node: 
            return None
        
        transform = scene.MatrixTransform( c4dNode.GetName(), convertMatrix(c4Node.GetRelMl()) )
        #if (node.GetType() == c4d.Opolygon): 
        #    self.dumpPolygonObject(nodeEl, node)
        #elif (node.GetType() == c4d.Olight): 
        #    self.dumpLightObject(nodeEl, node)
        
        c4dChildObj = node.GetDown()
        while c4dChildObj != None:
            transform.addChild( self.convertNode(nodeEl, c4dChildObj) )
            c4dChildObj = c4dChildObj.GetNext()

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

    def writeToFile(self, name, root):
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
        
        return self.writeToFile(name)

if __name__=='__main__':
    plugins.RegisterSceneSaverPlugin(id=PLUGIN_ID, str="SLON exporter (*.xml)", info=0, g=SlonExporter, description="", suffix="xml")
