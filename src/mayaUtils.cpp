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
 */

#include <mayaUtils.h>

// Maya
#include <maya/MStatus.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MObject.h>
#include <maya/MObjectArray.h>
#include <maya/MDagPath.h>
#include <maya/MPlug.h>
#include <maya/MSelectionList.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPxNode.h>
#include <maya/MStreamUtils.h>

MStatus
MMNodeInitUtils::attributeAffectsMulti(
        const MObjectArray &inputAttrs,
        const MObjectArray &outputAttrs
) {
    MStatus status = MS::kSuccess;
    for (unsigned int i = 0; i < inputAttrs.length(); i++) {
        for (unsigned int j = 0; j < outputAttrs.length(); j++) {
            MObject inputAttr = inputAttrs[i];
            MObject outputAttr = outputAttrs[j];
            status = MPxNode::attributeAffects(inputAttr, outputAttr);
            if (status != MS::kSuccess) {
                MStreamUtils::stdErrorStream()
                    << "ERROR: attributeAffects failed at "
                    << "input_index=" << i
                    << " output_index=" << j
                    << '\n';
                CHECK_MSTATUS(status);
            }
        }
    }
    return status;
}


MStatus getAsSelectionList(
    const MStringArray &nodeNames,
    MSelectionList &selList
) {
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


MStatus getAsSelectionList(
    const MString nodeName,
    MSelectionList &selList
) {
    MStringArray nodeNames;
    nodeNames.append(nodeName);
    return getAsSelectionList(nodeNames, selList);
}


MStatus nodeExistsAndIsType(
    const MString nodeName,
    const MFn::Type nodeType
) {
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
        ERR("Node type is not correct;"
            << " node=" << nodeName
            << " type=" << nodeType);
        status = MS::kFailure;
        status.perror("Node Type is not correct");
        return status;
    }
    return status;
}


MStatus getAsObject(
    const MString nodeName,
    MObject &object
) {
    MStatus status;
    MSelectionList selList;
    status = getAsSelectionList(nodeName, selList);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    if (selList.length() == 1) {
        status = selList.getDependNode(0, object);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }
    return status;
}


MStatus getAsDagPath(
    const MString nodeName,
    MDagPath &nodeDagPath
) {
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


bool hasAttrName(MFnDependencyNode &dependFn,
                 const MString &attrName) {
    MStatus status = MStatus::kSuccess;
    auto network_plug = true;
    MPlug plug = dependFn.findPlug(attrName, network_plug, &status);
    return !plug.isNull();
}


// Analogous to the Python function "mmSolver.api.get_object_type()"
ObjectType computeObjectType(
    const MObject &node_obj,
    MDagPath &nodeDagPath
) {
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

    ObjectType objectType = ObjectType::kUnknown;
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
        objectType = ObjectType::kMarker;
    } else if (hasTransformNode && hasLocatorShape) {
        objectType = ObjectType::kBundle;
    } else if (hasTransformNode && hasCameraShape) {
        objectType = ObjectType::kCamera;
    } else if (node_tid == MFn::kCamera) {
        objectType = ObjectType::kCamera;
    } else if (hasTransformNode && hasImagePlaneShape) {
        objectType = ObjectType::kImagePlane;
    } else if (hasImagePlaneShape) {
        objectType = ObjectType::kImagePlane;
    } else if (node_tid == MFn::kPluginDependNode) {
        // TODO: Check specifically for 'mmMarkerGroupTransform' node type.
        objectType = ObjectType::kMarkerGroup;
    } else if (node_tid == MFn::kSet && hasAttrSolverList) {
        objectType = ObjectType::kCollection;
    }
    return objectType;
}


MStatus constructAttrAffectsName(
    const MString attrName,
    const MString attrUuidStr,
    MString &outAttrName
) {
    MStatus status = MStatus::kSuccess;

    const MString attrNamePrefix = "node_";
    const MString attrNameSuffix = "_attr_";

    MString attrSubstitue(attrName);
    status = attrSubstitue.substitute(".", "_");
    CHECK_MSTATUS_AND_RETURN_IT(status);

    outAttrName = attrNamePrefix + attrUuidStr + attrNameSuffix + attrSubstitue;
    status = outAttrName.substitute("-", "_");
    CHECK_MSTATUS_AND_RETURN_IT(status);
    return status;
}
