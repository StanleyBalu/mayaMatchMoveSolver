/*
 * Copyright (C) 2018, 2019, 2021 David Cattermole.
 *
 * This file is part of mmSolver.
 *
 * mmSolver is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * mmSolver is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with mmSolver.  If not, see <https://www.gnu.org/licenses/>.
 * ====================================================================
 *
 * Miscellaneous Maya utilities, related to objects and selection, etc.
 */


#ifndef MAYA_UTILS_H
#define MAYA_UTILS_H

#include <vector>

// Utils
#include <utilities/debugUtils.h>

// Maya
#include <maya/MStatus.h>
#include <maya/MColor.h>
#include <maya/MDistance.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MObject.h>
#include <maya/MObjectArray.h>
#include <maya/MDagPath.h>
#include <maya/MPlug.h>
#include <maya/MDataHandle.h>
#include <maya/MSelectionList.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPxNode.h>


#define OBJECT_TYPE_UNKNOWN      (0)
#define OBJECT_TYPE_ATTRIBUTE    (1)
#define OBJECT_TYPE_MARKER       (2)
#define OBJECT_TYPE_BUNDLE       (3)
#define OBJECT_TYPE_CAMERA       (4)
#define OBJECT_TYPE_IMAGE_PLANE  (5)
#define OBJECT_TYPE_MARKER_GROUP (6)
#define OBJECT_TYPE_COLLECTION   (7)


static inline
MStatus getAsSelectionList(MStringArray nodeNames, MSelectionList &selList) {
    MStatus status;
    for (unsigned int i = 0; i < nodeNames.length(); ++i) {
        status = selList.add(nodeNames[i]);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }
    if (selList.length() != nodeNames.length()) {
        status = MStatus::kFailure;
        status.perror("getAsSelectionList failed");
    }
    return status;
}


static inline
MStatus getAsSelectionList(MString nodeName, MSelectionList &selList) {
    MStringArray nodeNames;
    nodeNames.append(nodeName);
    return getAsSelectionList(nodeNames, selList);
}


static inline
MStatus nodeExistsAndIsType(MString nodeName, MFn::Type nodeType) {
    MStatus status;
    MSelectionList selList;
    selList.clear();

    status = getAsSelectionList(nodeName, selList);
    if (status != MS::kSuccess) {
        ERR("Node does not exist; " << nodeName);
    }
    CHECK_MSTATUS_AND_RETURN_IT(status);

    MObject nodeObj;
    status = selList.getDependNode(0, nodeObj);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    if (nodeObj.apiType() != nodeType) {
        ERR("Node type is not correct; node=" << nodeName << " type=" << nodeType);
        status = MS::kFailure;
        status.perror("Node Type is not correct");
        return status;
    }
    return status;
}


static inline
MStatus getAsObject(MString nodeName, MObject &object) {
    MStatus status;
    MSelectionList selList;
    status = getAsSelectionList(nodeName, selList);
    CHECK_MSTATUS(status);
    if (selList.length() == 1) {
        status = selList.getDependNode(0, object);
        CHECK_MSTATUS(status);
    }
    return status;
}


static inline
MStatus getAsDagPath(MString nodeName, MDagPath &nodeDagPath) {
    MStatus status;
    MSelectionList selList;
    status = getAsSelectionList(nodeName, selList);
    CHECK_MSTATUS(status);
    if (selList.length() == 1) {
        status = selList.getDagPath(0, nodeDagPath);
        CHECK_MSTATUS(status);
    }
    return status;
}


inline
bool hasAttrName(MFnDependencyNode &dependFn, MString &attrName) {
    MStatus status = MStatus::kSuccess;
    auto network_plug = true;
    MPlug plug = dependFn.findPlug(attrName, network_plug, &status);
    return !plug.isNull();
}


// Analogous to the Python function "mmSolver.api.get_object_type()"
inline
unsigned int computeObjectType(MObject &node_obj, MDagPath &nodeDagPath) {
    MStatus status = MStatus::kSuccess;

    MFn::Type node_tid = nodeDagPath.apiType();
    bool hasTransformNode = (node_tid == MFn::kTransform)
        || (node_tid == MFn::kPluginTransformNode);

    bool hasLocatorShape = false;
    bool hasCameraShape = false;
    bool hasImagePlaneShape = false;
    auto childNodeDagPath = MDagPath();
    unsigned int num_children = nodeDagPath.childCount();
    for (unsigned int i = 0; i < num_children; ++i) {
        MObject child_obj = nodeDagPath.child(i);
        status = MDagPath::getAPathTo(child_obj, childNodeDagPath);
        CHECK_MSTATUS(status);
        MFn::Type shape_tid = childNodeDagPath.apiType();
        if (shape_tid == MFn::kLocator || shape_tid == MFn::kPluginLocatorNode) {
            hasLocatorShape = true;
        } else if (shape_tid == MFn::kCamera) {
            hasCameraShape = true;
        } else if (shape_tid == MFn::kImagePlane) {
            hasImagePlaneShape = true;
        }
    }

    MFnDependencyNode dependFn(node_obj, &status);
    CHECK_MSTATUS(status);

    unsigned int objectType = OBJECT_TYPE_UNKNOWN;
    auto attrNameEnable = MString("enable");
    auto attrNameWeight = MString("weight");
    auto attrNameBundle = MString("bundle");
    auto attrNameSolverList = MString("solverList");
    bool hasAttrEnable = hasAttrName(dependFn, attrNameEnable);
    bool hasAttrWeight = hasAttrName(dependFn, attrNameWeight);
    bool hasAttrBundle = hasAttrName(dependFn, attrNameBundle);
    bool hasAttrSolverList = hasAttrName(dependFn, attrNameSolverList);
    if (hasTransformNode
        && hasLocatorShape
        && hasAttrEnable
        && hasAttrWeight
        && hasAttrBundle) {
        objectType = OBJECT_TYPE_MARKER;
    } else if (hasTransformNode && hasLocatorShape) {
        objectType = OBJECT_TYPE_BUNDLE;
    } else if (hasTransformNode && hasCameraShape) {
        objectType = OBJECT_TYPE_CAMERA;
    } else if (node_tid == MFn::kCamera) {
        objectType = OBJECT_TYPE_CAMERA;
    } else if (hasTransformNode && hasImagePlaneShape) {
        objectType = OBJECT_TYPE_IMAGE_PLANE;
    } else if (hasImagePlaneShape) {
        objectType = OBJECT_TYPE_IMAGE_PLANE;
    } else if (node_tid == MFn::kPluginDependNode) {
        // TODO: Check specifically for 'mmMarkerGroupTransform' node type.
        objectType = OBJECT_TYPE_MARKER_GROUP;
    } else if (node_tid == MFn::kSet && hasAttrSolverList) {
        objectType = OBJECT_TYPE_COLLECTION;
    }
    return objectType;
}


// Generate attribute name used to set and look up 'attribute affects'
// on nodes.
static inline
MStatus constructAttrAffectsName(MString attrName,
                                 MString attrUuidStr,
                                 MString &outAttrName){
    MStatus status = MStatus::kSuccess;

    const MString attrNamePrefix = "node_";
    const MString attrNameSuffix = "_attr_";

    status = attrName.substitute(".", "_");
    CHECK_MSTATUS_AND_RETURN_IT(status);

    outAttrName = attrNamePrefix + attrUuidStr + attrNameSuffix + attrName;
    status = outAttrName.substitute("-", "_");
    CHECK_MSTATUS_AND_RETURN_IT(status);
    return status;
}

namespace mmsolver {

static inline
MStatus getNodeAttr(const MDagPath &objPath,
                    const MObject &attr,
                    MDistance &value) {
    MStatus status;
    MObject node = objPath.node(&status);
    if (status) {
        MPlug plug(node, attr);
        if (!plug.isNull()) {
            status = plug.getValue(value);
            return status;
        }
    }
    return status;
}

static inline
MStatus getNodeAttr(const MDagPath &objPath,
                    const MObject &attr,
                    bool &value) {
    MStatus status;
    MObject node = objPath.node(&status);
    if (status) {
        MPlug plug(node, attr);
        if (!plug.isNull()) {
            value = plug.asBool();
            return status;
        }
    }
    return status;
}

static inline
MStatus getNodeAttr(const MDagPath &objPath,
                    const MObject &attr,
                    int32_t &value) {
    MStatus status;
    MObject node = objPath.node(&status);
    if (status) {
        MPlug plug(node, attr);
        if (!plug.isNull()) {
            value = plug.asInt();
            return status;
        }
    }
    return status;
}

static inline
MStatus getNodeAttr(const MDagPath &objPath,
                    const MObject &attr,
                    uint32_t &value) {
    MStatus status;
    MObject node = objPath.node(&status);
    if (status) {
        MPlug plug(node, attr);
        if (!plug.isNull()) {
            value = static_cast<uint32_t>(plug.asInt());
            return status;
        }
    }
    return status;
}

static inline
MStatus getNodeAttr(const MDagPath &objPath,
                    const MObject &attr,
                    short &value) {
    MStatus status;
    MObject node = objPath.node(&status);
    if (status) {
        MPlug plug(node, attr);
        if (!plug.isNull()) {
            value = plug.asShort();
            return status;
        }
    }
    return status;
}

static inline
MStatus getNodeAttr(const MDagPath &objPath,
                    const MObject &attr,
                    float &value) {
    MStatus status;
    MObject node = objPath.node(&status);
    if (status) {
        MPlug plug(node, attr);
        if (!plug.isNull()) {
            value = plug.asFloat();
            return status;
        }
    }
    return status;
}

static inline
MStatus getNodeAttr(const MDagPath &objPath,
                    const MObject &attr,
                    double &value) {
    MStatus status;
    MObject node = objPath.node(&status);
    if (status) {
        MPlug plug(node, attr);
        if (!plug.isNull()) {
            value = plug.asDouble();
            return status;
        }
    }
    return status;
}

static inline
MStatus getNodeAttr(const MDagPath &objPath,
                    const MObject &attr,
                    MColor &value) {
    MStatus status;
    MObject node = objPath.node(&status);
    if (status) {
        MPlug plug(node, attr);
        if (!plug.isNull()) {
            MDataHandle data_handle = plug.asMDataHandle(&status);
            CHECK_MSTATUS_AND_RETURN_IT(status);
            auto data = data_handle.asFloat3();
            value.r = data[0];
            value.g = data[1];
            value.b = data[2];
            return status;
        }
    }
    return status;
}

static inline
MStatus getNodeAttr(const MDagPath &objPath,
                    const MObject &attr,
                    MMatrix &value) {
    MStatus status;
    MObject node = objPath.node(&status);
    if (status) {
        MPlug plug(node, attr);
        if (!plug.isNull()) {
            MDataHandle data_handle = plug.asMDataHandle(&status);
            CHECK_MSTATUS_AND_RETURN_IT(status);
            value = data_handle.asMatrix();
            return status;
        }
    }
    return status;
}

} // namespace mmsolver

// Static attributes to help with Maya Node initialization.
class MMNodeInitUtils {
  public:

    // Create 'MPxNode::attributeAffects' relationships for nodes, using
    // lists of attributes, rather than each relationship taking up one
    // line.
    static MStatus attributeAffectsMulti(
            const MObjectArray &inputAttrs,
            const MObjectArray &outputAttrs);
};

#endif // MAYA_UTILS_H
