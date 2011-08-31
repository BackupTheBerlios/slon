import c4d
import datetime
import inspect
import os
import math as pmath
import struct
import sys
import time

from c4d import plugins, documents, utils, gui
from c4d.plugins import GeLoadString

folder = os.path.dirname(__file__)
if folder not in sys.path:
    sys.path.insert(0, folder)
    sys.path.insert(0, folder+"/slon")

import slon
from slon import math
from slon import scene
from slon import graphics
from slon import physics
from slon import database
    
#be sure to use a unique ID obtained from 'plugincafe.com'
PLUGIN_ID = 1025282
    
def convertVector3(v, scale = 1.0):
    return math.Vector3f(v.x * scale, v.y * scale, v.z * scale)
    
def convertMatrix(m, scale = 1.0):
    mat = math.Matrix4f()
    mat[0] = math.VectorRow4f(m.v1.x, m.v2.x, m.v3.x, m.off.x * scale)
    mat[1] = math.VectorRow4f(m.v1.y, m.v2.y, m.v3.y, m.off.y * scale)
    mat[2] = math.VectorRow4f(m.v1.z, m.v2.z, m.v3.z, m.off.z * scale)
    mat[3] = math.VectorRow4f(     0,      0,      0,               1)
    
    return mat
    
def convertPhysicsTransform(rot, trans, scale = 1.0):
    rotMat = utils.HPBToMatrix(rot)
    
    mat = math.Matrix4f()
    mat[0] = math.VectorRow4f(rotMat.v1.x, rotMat.v2.x, rotMat.v3.x, trans.x * scale)
    mat[1] = math.VectorRow4f(rotMat.v1.y, rotMat.v2.y, rotMat.v3.y, trans.y * scale)
    mat[2] = math.VectorRow4f(rotMat.v1.z, rotMat.v2.z, rotMat.v3.z, trans.z * scale)
    mat[3] = math.VectorRow4f(          0,           0,           0,               1)
    
    return mat
    
class SlonExporter(plugins.SceneSaverData):
        
    def __init__(self):
        self.library = database.Library()
        self.documentScale = 0.01
        
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
        
    def convertPolygonObject(self, c4dPolygonObj):
        cpuSideMesh = graphics.CPUSideTriangleMesh()
        
        # gather points
        points = c4dPolygonObj.GetAllPoints()
        pointsArr = slon.Vector3fArray()
        for point in points:
            pointsArr.append( convertVector3(point, self.documentScale) )
        
        if (len(pointsArr) == 0):
            return None
        cpuSideMesh.setAttributes("position", 0, pointsArr)
                
        # gather polygons
        polygons = c4dPolygonObj.GetAllPolygons()
        polygonsArr = slon.UIntArray()
        indexCount = 0
        for poly in polygons:
            if hasattr(poly, "d"):
                polygonsArr.extend( [poly.a, poly.b, poly.c, poly.a, poly.c, poly.d] )
            else:
                polygonsArr.extend( [poly.a, poly.b, poly.c] )
        
        if (len(polygonsArr) == 0):
            return None
        cpuSideMesh.setIndices(0, polygonsArr)
        
        # gather normals
        normalTag = c4dPolygonObj.GetTag(c4d.Tnormal)
        if (normalTag != None):
            normalsArr = slon.Vector3fArray()
            normalsInds = slon.UIntArray()
            normalData = normalTag.GetAllHighlevelData()
            for i in range(0, GetDataCount() - 1):
                rawNormal = normalData[i]
                normalsArr.append( math.Vector3f(rawNormal.x / 32000.0, rawNormal.y / 32000.0, rawNormal.z / 32000.0) )
                normalsInds.append(i)
                
            if (len(normalsArr) > 0):
                cpuSideMesh.setAttributes("normal", 1, normalsArr)
                cpuSideMesh.setIndices(1, normalsInds)
        else:
            cpuSideMesh.generateSmoothingGroups(pmath.pi / 4, True, True)
            cpuSideMesh.generatePhongNormals()
            
        # create material
        textureTag = c4dPolygonObj.GetTag(c4d.Ttexture)
        if (textureTag == None):
            return None
        
        material = textureTag.GetMaterial()
        diffuse = convertVector3(material.GetAverageColor(c4d.CHANNEL_COLOR))
        shininess = material.GetAverageColor(c4d.CHANNEL_SPECULAR).x
        effect = graphics.createLightingEffect(diffuse, shininess)
        
        # construct mesh
        gpuSideMesh = cpuSideMesh.createGPUSideMesh()
        gpuSideMesh.addIndexedSubset(effect, graphics.PRIMITIVE_TYPE.TRIANGLES, 0, len(polygonsArr))
        return graphics.StaticMesh(gpuSideMesh)
        
    def dumpLightObject(self, parentEl, node):
        return
        
    def convertNode(self, c4dNode):
        if not c4dNode: 
            return None
       
        dynTag = c4dNode.GetTag(180000102)
        if c4dNode.GetTag(180000102):
            parent, current = self.convertPhysicsNode(c4dNode)
        else:
            parent = current = scene.MatrixTransform( c4dNode.GetName(), convertMatrix(c4dNode.GetRelMl(), self.documentScale) )
        
        if (c4dNode.GetType() == c4d.Opolygon):
            polyObj = self.convertPolygonObject(c4dNode)
            if (polyObj != None):
                current.addChild(polyObj)
        #elif (node.GetType() == c4d.Olight): 
        #    self.dumpLightObject(nodeEl, node)
        
        c4dChildObj = c4dNode.GetDown()
        while c4dChildObj != None:
            current.addChild( self.convertNode(c4dChildObj) )
            c4dChildObj = c4dChildObj.GetNext()
            
        return parent
               
    def computeBoxShape(self, c4dNode):
        return physics.BoxShape( convertVector3(c4dNode.GetRad(), self.documentScale) )
        
    def computeCylinderXShape(self, c4dNode):
        return physics.CylinderShape( convertVector3(c4dNode.GetRad(), self.documentScale) )
        
    def computeCylinderYShape(self, c4dNode):
        return physics.CylinderShape( convertVector3(c4dNode.GetRad(), self.documentScale) )
        
    def computeCylinderZShape(self, c4dNode):
        return physics.CylinderShape( convertVector3(c4dNode.GetRad(), self.documentScale) )
        
    def computeConvexHullShape(self, c4dNode):
        pointsArr = slon.Vector3fArray()    
        for point in c4dNode.GetAllPoints():
            pointsArr.append( convertVector3(point, self.documentScale) )
        
        shape = physics.ConvexShape()
        shape.buildConvexHull(pointsArr)
        return shape
            
    def convertPhysicsNode(self, c4dNode):       
        dynTag = c4dNode.GetTag(180000102)

        desc = physics.RigidBody.DESC()
        desc.transform = convertPhysicsTransform(c4dNode.GetAbsRot(), c4dNode.GetAbsPos(), self.documentScale)
        desc.type = physics.RigidBody.DYNAMICS_TYPE.DYNAMIC
        desc.mass = dynTag[c4d.RIGID_BODY_MASS]
        desc.margin = dynTag[c4d.RIGID_BODY_MARGIN]
        shape = dynTag[c4d.RIGID_BODY_SHAPE]
        if shape == c4d.RIGID_BODY_SHAPE_BOX:
            desc.collisionShape = self.computeBoxShape(c4dNode)
        elif shape == c4d.RIGID_BODY_SHAPE_CYLINDER_X:
            desc.collisionShape = self.computeCylinderXShape(c4dNode)
        elif shape == c4d.RIGID_BODY_SHAPE_CYLINDER_Y:
            desc.collisionShape = self.computeCylinderYShape(c4dNode)
        elif shape == c4d.RIGID_BODY_SHAPE_CYLINDER_Z:
            desc.collisionShape = self.computeCylinderZShape(c4dNode)
        elif shape == c4d.RIGID_BODY_SHAPE_CONVEX_HULL:
            desc.collisionShape = self.computeConvexHullShape(c4dNode)
        else:
            print c4dNode.GetName(), " : physics shape not converted"
            desc.collisionShape = physics.SphereShape(1.0)
        rigidBody = physics.RigidBody(desc)

        physicsTransform = physics.PhysicsTransform(rigidBody)
        physicsTransform.absolute = True
        
        # physics to graphics transformation convertion       
        transform = math.invert(desc.transform) * convertMatrix(c4dNode.GetMg(), self.documentScale) 
        physToGraphics = scene.MatrixTransform("PhysicsToGraphics", transform)
        physicsTransform.addChild(physToGraphics)
        
        return physicsTransform, physToGraphics
        #rbEl = etree.SubElement(self.physicsEl, "RigidBody")
        #etree.SubElement(rbEl, "mass").text = str(dynTag[c4d.RIGID_BODY_MASS])
        #etree.SubElement(rbEl, "margin").text = str(dynTag[c4d.RIGID_BODY_MARGIN])
        #etree.SubElement(rbEl, "target").text = node.GetName()
        #etree.SubElement(rbEl, "shape").text = str(dynTag[c4d.RIGID_BODY_SHAPE])
        #etree.SubElement(rbEl, "ref_shape").text = str(dynTag[c4d.RIGID_BODY_REFERENCE_SHAPE])
        
    def writeToFile(self, name):
        database.saveLibrary(name, self.library)
        return c4d.FILEERROR_NONE
        
    def Save(self, c4dNode, fileName, c4dDocument, filterflags):
        # init graphics
        graphics.setVideoMode(640, 480, 32, False, False, 0)
        
        # do save
        root = scene.Group()
        c4dObj = c4dDocument.GetFirstObject()
        while c4dObj != None:
            root.addChild( self.convertNode(c4dObj) )
            c4dObj = c4dObj.GetNext()
        #self.dumpPhysics(doc.GetFirstObject())
        self.library.visualScenes["root"] = root
        
        result = self.writeToFile(fileName)
        graphics.closeWindow()
        return result
        
if __name__=='__main__':
    plugins.RegisterSceneSaverPlugin(id=PLUGIN_ID, str="SLON exporter (*.sxml)", info=0, g=SlonExporter, description="", suffix="sxml")
