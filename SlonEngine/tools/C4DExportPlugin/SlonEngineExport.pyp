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
    
def convertMatrixP(m, scale = 1.0):
    v1 = m.v1.GetNormalized()
    v2 = m.v2.GetNormalized()
    v3 = m.v3.GetNormalized()
    
    mat = math.Matrix4f()
    mat[0] = math.VectorRow4f(v1.x, v2.x, v3.x, m.off.x * scale)
    mat[1] = math.VectorRow4f(v1.y, v2.y, v3.y, m.off.y * scale)
    mat[2] = math.VectorRow4f(v1.z, v2.z, v3.z, m.off.z * scale)
    mat[3] = math.VectorRow4f(   0,    0,    0,               1)
    
    return mat
    
class SlonExporter(plugins.SceneSaverData):
        
    def __init__(self):
        self.library = database.Library()
        self.documentScale = 0.01
        self.rigidBodies = []
        
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
        
        if (c4dNode.GetType() == 180000011):
            current.addChild( self.convertConnectorNode(c4dNode) )
            
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
               
    def computeBoxShape(self, c4dNode, transform):
        mp = c4dNode.GetMp() * self.documentScale
        transform[0][3] += mp.x
        transform[1][3] += mp.y
        transform[2][3] += mp.z
        return physics.BoxShape( convertVector3(c4dNode.GetRad(), self.documentScale) )
        
    def computeCylinderXShape(self, c4dNode, transform):
        mp = c4dNode.GetMp() * self.documentScale
        transform[0][3] += mp.x
        transform[1][3] += mp.y
        transform[2][3] += mp.z
        return physics.CylinderXShape( convertVector3(c4dNode.GetRad(), self.documentScale) )
        
    def computeCylinderYShape(self, c4dNode, transform):
        mp = c4dNode.GetMp() * self.documentScale
        transform[0][3] += mp.x
        transform[1][3] += mp.y
        transform[2][3] += mp.z
        return physics.CylinderYShape( convertVector3(c4dNode.GetRad(), self.documentScale) )
        
    def computeCylinderZShape(self, c4dNode, transform):
        mp = c4dNode.GetMp() * self.documentScale
        transform[0][3] += mp.x
        transform[1][3] += mp.y
        transform[2][3] += mp.z
        return physics.CylinderZShape( convertVector3(c4dNode.GetRad(), self.documentScale) )
        
    def computeConvexHullShape(self, c4dNode, transform):
        pointsArr = slon.Vector3fArray()    
        for point in c4dNode.GetAllPoints():
            pointsArr.append( convertVector3(point, self.documentScale) )
        
        shape = physics.ConvexShape()
        shape.buildConvexHull(pointsArr)
        return shape
        
    def getNodeRigidBody(self, c4dNode):
        try:
            rbElem = c4d.DescID(c4d.DescLevel(c4d.ID_USERDATA), c4d.DescLevel(1))
            rbID = c4dNode[rbElem]
            return self.rigidBodies[rbID]
        except:
            rbData = c4d.GetCustomDatatypeDefault(c4d.DTYPE_LONG)
            rbElem = c4dNode.AddUserData(rbData)
            c4dNode[rbElem] = len(self.rigidBodies)
            rigidBody = physics.RigidBody()
            self.rigidBodies.append(rigidBody)
            return rigidBody
        
    def convertConnectorNode(self, c4dNode):
        objA = c4dNode[c4d.FORCE_OBJECT_A]
        objB = c4dNode[c4d.FORCE_OBJECT_B]
        
        desc = physics.Constraint.DESC()
        desc.rigidBodyA = self.getNodeRigidBody(objA)
        desc.frameA     = math.invert( convertMatrixP(c4dNode.GetMg(), self.documentScale) ) * convertMatrixP(objA.GetMg(), self.documentScale)
        desc.rigidBodyB = self.getNodeRigidBody(objB)
        desc.frameB     = math.invert( convertMatrixP(c4dNode.GetMg(), self.documentScale) ) * convertMatrixP(objB.GetMg(), self.documentScale)
       
        type = c4dNode[c4d.FORCE_TYPE]
        if type == c4d.CONSTRAINT_JOINT_TYPE_CARDAN:
            if c4dNode[c4d.CONSTRAINT_ROT1_LIMIT]:
                desc.angularLimitMin.x = c4dNode[c4d.CONSTRAINT_ROT1_LIMIT_MIN]
                desc.angularLimitMax.x = c4dNode[c4d.CONSTRAINT_ROT1_LIMIT_MAX]
            else:
                desc.angularLimitMin.x =  1
                desc.angularLimitMax.x = -1
                
            if c4dNode[c4d.CONSTRAINT_ROT2_LIMIT]:
                desc.angularLimitMin.y = c4dNode[c4d.CONSTRAINT_ROT2_LIMIT_MIN]
                desc.angularLimitMax.y = c4dNode[c4d.CONSTRAINT_ROT2_LIMIT_MAX]
            else:
                desc.angularLimitMin.y =  1
                desc.angularLimitMax.y = -1
        elif type == c4d.CONSTRAINT_JOINT_TYPE_HINGE:
            print c4dNode[c4d.CONSTRAINT_ROT1_LIMIT]
            print c4dNode[c4d.CONSTRAINT_ROT1_LIMIT_MIN]
            print c4dNode[c4d.CONSTRAINT_ROT1_LIMIT_MAX]
            if c4dNode[c4d.CONSTRAINT_ROT1_LIMIT]:
                desc.angularLimitMin.z = c4dNode[c4d.CONSTRAINT_ROT1_LIMIT_MIN]
                desc.angularLimitMax.z = c4dNode[c4d.CONSTRAINT_ROT1_LIMIT_MAX]
            else:
                desc.angularLimitMin.z =  1
                desc.angularLimitMax.z = -1
        elif type == c4d.CONSTRAINT_JOINT_TYPE_RAGDOLL:
            desc.angularLimitMin.x = -c4dNode[c4d.CONSTRAINT_CONE_LIMIT_RADIUS]
            desc.angularLimitMax.x =  c4dNode[c4d.CONSTRAINT_CONE_LIMIT_RADIUS]
            desc.angularLimitMin.y = -c4dNode[c4d.CONSTRAINT_CONE_LIMIT_RADIUS]
            desc.angularLimitMax.y =  c4dNode[c4d.CONSTRAINT_CONE_LIMIT_RADIUS]
            if c4dNode[c4d.CONSTRAINT_ROT1_LIMIT]:
                desc.angularLimitMin.z = c4dNode[c4d.CONSTRAINT_ROT1_LIMIT_MIN]
                desc.angularLimitMax.z = c4dNode[c4d.CONSTRAINT_ROT1_LIMIT_MAX]
            else:
                desc.angularLimitMin.z =  1
                desc.angularLimitMax.z = -1
        
        desc.angularLimitMin.y = 1
        desc.angularLimitMax = math.Vector3f(-1, -1, -1)
        constraint = physics.Constraint(desc)
        return physics.ConstraintNode(constraint)
            
    def convertPhysicsNode(self, c4dNode):       
        dynTag = c4dNode.GetTag(180000102)

        desc = physics.RigidBody.DESC()
        desc.transform      = convertMatrixP(c4dNode.GetMg(), self.documentScale)
        desc.type           = physics.RigidBody.DYNAMICS_TYPE.DYNAMIC
        desc.mass           = dynTag[c4d.RIGID_BODY_MASS]
        desc.relativeMargin = 0.0
        desc.margin         = dynTag[c4d.RIGID_BODY_MARGIN] * self.documentScale
        
        shape = dynTag[c4d.RIGID_BODY_SHAPE]
        if shape == c4d.RIGID_BODY_SHAPE_BOX:
            desc.collisionShape = self.computeBoxShape(c4dNode, desc.transform)
        elif shape == c4d.RIGID_BODY_SHAPE_CYLINDER_X:
            desc.collisionShape = self.computeCylinderXShape(c4dNode, desc.transform)
        elif shape == c4d.RIGID_BODY_SHAPE_CYLINDER_Y:
            desc.collisionShape = self.computeCylinderYShape(c4dNode, desc.transform)
        elif shape == c4d.RIGID_BODY_SHAPE_CYLINDER_Z:
            desc.collisionShape = self.computeCylinderZShape(c4dNode, desc.transform)
        elif shape == c4d.RIGID_BODY_SHAPE_CONVEX_HULL:
            desc.collisionShape = self.computeConvexHullShape(c4dNode, desc.transform)
        else:
            print c4dNode.GetName(), " : physics shape not converted"
            desc.collisionShape = physics.SphereShape(1.0)
            
        rigidBody = self.getNodeRigidBody(c4dNode)
        rigidBody.reset(desc)

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
