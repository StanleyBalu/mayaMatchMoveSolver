/*
 * Copyright (C) 2020, 2021 David Cattermole.
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

#include <unordered_map>

// Maya
#include <maya/MStreamUtils.h>
#include <maya/MDagPath.h>
#include <maya/MObject.h>
#include <maya/MTime.h>
#include <maya/MTimeArray.h>
#include <maya/MComputation.h>
#include <maya/MString.h>

// MM SceneGraph
#include <mmscenegraph/mmscenegraph.h>

// Internal
#include "Camera.h"
#include "Attr.h"
#include "Marker.h"
#include "Bundle.h"
#include "mayaUtils.h"
#include "mayaSceneGraph.h"

namespace mmsg = mmscenegraph;

using StringToAttrIdMap = std::unordered_map<std::string, mmscenegraph::AttrId>;

MStatus
add_attribute(
    Attr &mayaAttr,
    const MString &attr_name,
    const MTimeArray &frameList,
    const int timeEvalMode,
    const double scaleFactor,
    mmsg::AttrDataBlock &attrDataBlock,
    mmsg::AttrId &out_attrId,
    StringToAttrIdMap &out_attrNameToAttrIdMap
) {
    MStatus status = MS::kSuccess;
    mayaAttr.setAttrName(attr_name);

    // TODO: If the attribute is keyed, but there's only one frame in
    // the frame list, then we can consider the attribute static, not
    // animated.

    // TODO: If the attribute is connected directly to another
    // attribute we can use the connected attribute's value. If the
    // source attribute is computed by another node, the scene graph
    // construction should fail - we do not support that yet.

    auto animated = mayaAttr.isAnimated();
    // auto locked = !mayaAttr.isFreeToChange();
    // auto connected = mayaAttr.isConnected();

    assert(frameList.length() > 0);
    auto start_time = frameList[0];
    auto start_frame_num = static_cast<mmsg::FrameValue>(
        frameList[0].as(MTime::uiUnit()));

    double value = 0.0;
    if (animated) {
        auto values = rust::Vec <mmsg::Real>();
        for (uint32_t i = 0; i < frameList.length(); ++i) {
            MTime frame = frameList[i];
            status = mayaAttr.getValue(value, frame, timeEvalMode);
            CHECK_MSTATUS_AND_RETURN_IT(status);
            values.push_back(value * scaleFactor);
        }
        out_attrId = attrDataBlock.create_attr_anim_dense(values, start_frame_num);
    } else {
        status = mayaAttr.getValue(value, timeEvalMode);
        out_attrId = attrDataBlock.create_attr_static(value * scaleFactor);
    }

    MString nodeAttrName = mayaAttr.getLongName();
    auto nodeAttrNameStr = std::string(nodeAttrName.asChar());
    out_attrNameToAttrIdMap.insert({nodeAttrNameStr, out_attrId});

    return status;
}


MStatus
get_translate_attrs(
    Attr &mayaAttr,
    const MTimeArray &frameList,
    const int timeEvalMode,
    mmsg::AttrDataBlock &out_attrDataBlock,
    mmsg::Translate3DAttrIds &out_attrIds,
    StringToAttrIdMap &out_attrNameToAttrIdMap
) {
    MStatus status = MS::kSuccess;
    double scaleFactor = 1.0;  // No conversion.

    add_attribute(
        mayaAttr,
        MString("translateX"),
        frameList,
        timeEvalMode,
        scaleFactor,
        out_attrDataBlock,
        out_attrIds.tx,
        out_attrNameToAttrIdMap);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    add_attribute(
        mayaAttr,
        MString("translateY"),
        frameList,
        timeEvalMode,
        scaleFactor,
        out_attrDataBlock,
        out_attrIds.ty,
        out_attrNameToAttrIdMap);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    add_attribute(
        mayaAttr,
        MString("translateZ"),
        frameList,
        timeEvalMode,
        scaleFactor,
        out_attrDataBlock,
        out_attrIds.tz,
        out_attrNameToAttrIdMap);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    return status;
}

MStatus
get_rotate_attrs(
    Attr &mayaAttr,
    const MTimeArray &frameList,
    const int timeEvalMode,
    mmsg::AttrDataBlock &out_attrDataBlock,
    mmsg::Rotate3DAttrIds &out_attrIds,
    StringToAttrIdMap &out_attrNameToAttrIdMap
) {
    MStatus status = MS::kSuccess;
    double scaleFactor = 1.0;  // No conversion.

    add_attribute(
        mayaAttr,
        MString("rotateX"),
        frameList,
        timeEvalMode,
        scaleFactor,
        out_attrDataBlock,
        out_attrIds.rx,
        out_attrNameToAttrIdMap);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    add_attribute(
        mayaAttr,
        MString("rotateY"),
        frameList,
        timeEvalMode,
        scaleFactor,
        out_attrDataBlock,
        out_attrIds.ry,
        out_attrNameToAttrIdMap);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    add_attribute(
        mayaAttr,
        MString("rotateZ"),
        frameList,
        timeEvalMode,
        scaleFactor,
        out_attrDataBlock,
        out_attrIds.rz,
        out_attrNameToAttrIdMap);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    return status;
}

MStatus
get_scale_attrs(
    Attr &mayaAttr,
    const MTimeArray &frameList,
    const int timeEvalMode,
    mmsg::AttrDataBlock &out_attrDataBlock,
    mmsg::Scale3DAttrIds &out_attrIds,
    StringToAttrIdMap &out_attrNameToAttrIdMap
) {
    MStatus status = MS::kSuccess;
    double scaleFactor = 1.0;  // No conversion.

    add_attribute(
        mayaAttr,
        MString("scaleX"),
        frameList,
        timeEvalMode,
        scaleFactor,
        out_attrDataBlock,
        out_attrIds.sx,
        out_attrNameToAttrIdMap);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    add_attribute(
        mayaAttr,
        MString("scaleY"),
        frameList,
        timeEvalMode,
        scaleFactor,
        out_attrDataBlock,
        out_attrIds.sy,
        out_attrNameToAttrIdMap);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    add_attribute(
        mayaAttr,
        MString("scaleZ"),
        frameList,
        timeEvalMode,
        scaleFactor,
        out_attrDataBlock,
        out_attrIds.sz,
        out_attrNameToAttrIdMap);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    return status;
}

MStatus
get_camera_attrs(
    Attr &mayaAttr,
    const MTimeArray &frameList,
    const int timeEvalMode,
    mmsg::AttrDataBlock &out_attrDataBlock,
    mmsg::CameraAttrIds &out_attrIds,
    StringToAttrIdMap &out_attrNameToAttrIdMap
) {
    MStatus status = MS::kSuccess;
    double scaleFactor = 1.0;  // No conversion.
    double inch_to_mm = 25.4;

    add_attribute(
        mayaAttr,
        MString("horizontalFilmAperture"),
        frameList,
        timeEvalMode,
        inch_to_mm,
        out_attrDataBlock,
        out_attrIds.sensor_width,
        out_attrNameToAttrIdMap);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    add_attribute(
        mayaAttr,
        MString("verticalFilmAperture"),
        frameList,
        timeEvalMode,
        inch_to_mm,
        out_attrDataBlock,
        out_attrIds.sensor_height,
        out_attrNameToAttrIdMap);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    add_attribute(
        mayaAttr,
        MString("focalLength"),
        frameList,
        timeEvalMode,
        scaleFactor,
        out_attrDataBlock,
        out_attrIds.focal_length,
        out_attrNameToAttrIdMap);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    return status;
}

MStatus
get_rotate_order_attr(
    Attr &mayaAttr,
    const int timeEvalMode,
    mmsg::RotateOrder &out_rotateOrder
) {
    MStatus status = MS::kSuccess;

    status = mayaAttr.setAttrName(MString("rotateOrder"));
    CHECK_MSTATUS_AND_RETURN_IT(status);

    short value = 0;
    mayaAttr.getValue(value, timeEvalMode);
    switch (value) {
        break; case 0: out_rotateOrder = mmsg::RotateOrder::kXYZ;
        break; case 1: out_rotateOrder = mmsg::RotateOrder::kYZX;
        break; case 2: out_rotateOrder = mmsg::RotateOrder::kZXY;
        break; case 3: out_rotateOrder = mmsg::RotateOrder::kXZY;
        break; case 4: out_rotateOrder = mmsg::RotateOrder::kYXZ;
        break; case 5: out_rotateOrder = mmsg::RotateOrder::kZYX;
        break; default: out_rotateOrder = mmsg::RotateOrder::kUnknown;
    }
    return status;
}


MStatus
get_transform_attrs(
    Attr &mayaAttr,
    const MTimeArray &frameList,
    const int timeEvalMode,
    mmsg::AttrDataBlock &out_attrDataBlock,
    mmsg::Translate3DAttrIds &out_translateAttrIds,
    mmsg::Rotate3DAttrIds &out_rotateAttrIds,
    mmsg::Scale3DAttrIds &out_scaleAttrIds,
    mmsg::RotateOrder &out_rotateOrder,
    StringToAttrIdMap &out_attrNameToAttrIdMap
) {
    MStatus status = MS::kSuccess;

    get_translate_attrs(
        mayaAttr,
        frameList, timeEvalMode,
        out_attrDataBlock, out_translateAttrIds,
        out_attrNameToAttrIdMap);
    get_rotate_attrs(
        mayaAttr,
        frameList, timeEvalMode,
        out_attrDataBlock, out_rotateAttrIds,
        out_attrNameToAttrIdMap);
    get_scale_attrs(
        mayaAttr,
        frameList, timeEvalMode,
        out_attrDataBlock, out_scaleAttrIds,
        out_attrNameToAttrIdMap);
    get_rotate_order_attr(
        mayaAttr, timeEvalMode, out_rotateOrder);

    return status;
}

MStatus
get_marker_attrs(
    Attr &mayaAttr,
    const MTimeArray &frameList,
    const int timeEvalMode,
    mmsg::AttrDataBlock &out_attrDataBlock,
    mmsg::MarkerAttrIds &out_attrIds,
    StringToAttrIdMap &out_attrNameToAttrIdMap
) {
    MStatus status = MS::kSuccess;
    double scaleFactor = 1.0;  // No conversion.

    add_attribute(
        mayaAttr,
        MString("translateX"),
        frameList,
        timeEvalMode,
        scaleFactor,
        out_attrDataBlock,
        out_attrIds.tx,
        out_attrNameToAttrIdMap);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    add_attribute(
        mayaAttr,
        MString("translateY"),
        frameList,
        timeEvalMode,
        scaleFactor,
        out_attrDataBlock,
        out_attrIds.ty,
        out_attrNameToAttrIdMap);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    // TODO: The marker weight is more complicated because the user
    // can also disable the marker using the 'enable' attribute.
    //
    // TODO: Do we actually need the weight? It's not used inside
    // mmscenegraph currently.
    add_attribute(
        mayaAttr,
        MString("weight"),
        frameList,
        timeEvalMode,
        scaleFactor,
        out_attrDataBlock,
        out_attrIds.weight,
        out_attrNameToAttrIdMap);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    return status;
}

MStatus
add_cameras(
    const CameraPtrList &cameraList,
    const MTimeArray &frameList,
    const int timeEvalMode,
    std::vector<mmsg::CameraNode> &out_cameraNodes,
    mmsg::EvaluationObjects &out_evalObjects,
    mmsg::SceneGraph &out_sceneGraph,
    mmsg::AttrDataBlock &out_attrDataBlock,
    StringToAttrIdMap &out_attrNameToAttrIdMap
) {
    MStatus status = MS::kSuccess;

    // Create a single attribute that will be re-used.
    auto mayaAttr = Attr();

    auto translate_attr_ids = mmsg::Translate3DAttrIds();
    auto rotate_attr_ids = mmsg::Rotate3DAttrIds();
    auto scale_attr_ids = mmsg::Scale3DAttrIds();
    auto camera_attr_ids = mmsg::CameraAttrIds();
    auto rotate_order = mmsg::RotateOrder::kUnknown;

    out_cameraNodes.clear();
    out_cameraNodes.reserve(cameraList.size());

    CameraPtrList::const_iterator cit;
    for (auto cit = cameraList.cbegin(); cit != cameraList.cend(); ++cit) {
        auto cam_ptr = *cit;
        auto cam_tfm_name = cam_ptr->getTransformNodeName();
        auto cam_shp_name = cam_ptr->getShapeNodeName();
        auto cam_tfm_obj = cam_ptr->getTransformObject();
        auto cam_shp_obj = cam_ptr->getShapeObject();
        auto dag_path = MDagPath();
        status = MDagPath::getAPathTo(cam_tfm_obj, dag_path);
        CHECK_MSTATUS_AND_RETURN_IT(status);

        MString transform_name = dag_path.fullPathName(&status);
        CHECK_MSTATUS_AND_RETURN_IT(status);
        status = mayaAttr.setNodeName(transform_name);
        CHECK_MSTATUS_AND_RETURN_IT(status);

        get_transform_attrs(
            mayaAttr,
            frameList,
            timeEvalMode,
            out_attrDataBlock,
            translate_attr_ids,
            rotate_attr_ids,
            scale_attr_ids,
            rotate_order,
            out_attrNameToAttrIdMap
        );

        status = MDagPath::getAPathTo(cam_shp_obj, dag_path);
        CHECK_MSTATUS_AND_RETURN_IT(status);
        MString shape_name = dag_path.fullPathName(&status);
        CHECK_MSTATUS_AND_RETURN_IT(status);
        status = mayaAttr.setNodeName(shape_name);
        CHECK_MSTATUS_AND_RETURN_IT(status);

        get_camera_attrs(
            mayaAttr,
            frameList,
            timeEvalMode,
            out_attrDataBlock,
            camera_attr_ids,
            out_attrNameToAttrIdMap
        );

        auto cam_node = out_sceneGraph.create_camera_node(
            translate_attr_ids,
            rotate_attr_ids,
            scale_attr_ids,
            camera_attr_ids,
            rotate_order);
        out_cameraNodes.push_back(cam_node);
        out_evalObjects.add_camera(cam_node);
    }
    return status;
}

MStatus
add_bundles(
    const BundlePtrList &bundleList,
    const MTimeArray &frameList,
    const int timeEvalMode,
    std::vector<mmsg::BundleNode> &out_bundleNodes,
    mmsg::EvaluationObjects &out_evalObjects,
    mmsg::SceneGraph &out_sceneGraph,
    mmsg::AttrDataBlock &out_attrDataBlock,
    StringToAttrIdMap &out_attrNameToAttrIdMap
) {
    MStatus status = MS::kSuccess;

    // Create a single attribute that will be re-used.
    auto mayaAttr = Attr();

    auto translate_attr_ids = mmsg::Translate3DAttrIds();
    auto rotate_attr_ids = mmsg::Rotate3DAttrIds();
    auto scale_attr_ids = mmsg::Scale3DAttrIds();
    auto rotate_order = mmsg::RotateOrder::kUnknown;

    out_bundleNodes.clear();
    out_bundleNodes.reserve(bundleList.size());

    BundlePtrList::const_iterator cit;
    for (auto cit = bundleList.cbegin(); cit != bundleList.cend(); ++cit) {
        auto bnd_ptr = *cit;
        auto bnd_tfm_name = bnd_ptr->getNodeName();
        auto bnd_tfm_obj = bnd_ptr->getObject();

        auto dag_path = MDagPath();
        status = MDagPath::getAPathTo(bnd_tfm_obj, dag_path);
        CHECK_MSTATUS_AND_RETURN_IT(status);

        MString transform_name = dag_path.fullPathName(&status);
        CHECK_MSTATUS_AND_RETURN_IT(status);
        status = mayaAttr.setNodeName(transform_name);
        CHECK_MSTATUS_AND_RETURN_IT(status);

        get_transform_attrs(
            mayaAttr,
            frameList,
            timeEvalMode,
            out_attrDataBlock,
            translate_attr_ids,
            rotate_attr_ids,
            scale_attr_ids,
            rotate_order,
            out_attrNameToAttrIdMap
        );

        auto bnd_node = out_sceneGraph.create_bundle_node(
            translate_attr_ids,
            rotate_attr_ids,
            scale_attr_ids,
            rotate_order);
        out_bundleNodes.push_back(bnd_node);
        out_evalObjects.add_bundle(bnd_node);
    }
    return status;
}

MStatus
add_markers(
    const MarkerPtrList &markerList,
    const CameraPtrList &cameraList,
    const BundlePtrList &bundleList,
    const MTimeArray &frameList,
    const int timeEvalMode,
    const std::vector<mmsg::CameraNode> &cameraNodes,
    const std::vector<mmsg::BundleNode> &bundleNodes,
    std::vector<mmsg::MarkerNode> &out_markerNodes,
    mmsg::EvaluationObjects &out_evalObjects,
    mmsg::SceneGraph &out_sceneGraph,
    mmsg::AttrDataBlock &out_attrDataBlock,
    StringToAttrIdMap &out_attrNameToAttrIdMap
) {
    MStatus status = MS::kSuccess;
    assert(cameraList.size() == cameraNodes.size());
    assert(bundleList.size() == bundleNodes.size());

    // Create a single attribute that will be re-used.
    auto mayaAttr = Attr();

    auto mkr_attr_ids = mmsg::MarkerAttrIds();

    out_markerNodes.clear();
    out_markerNodes.reserve(markerList.size());

    for (uint32_t i = 0; i < markerList.size(); ++i) {
        auto mkr_ptr = markerList[i];
        auto mkr_tfm_name = mkr_ptr->getNodeName();
        auto mkr_tfm_obj = mkr_ptr->getObject();

        // Get camera name.
        auto mkr_cam_ptr = mkr_ptr->getCamera();
        auto mkr_cam_shp_name = mkr_cam_ptr->getShapeNodeName();

        mmsg::NodeId cam_node_id = mmsg::NodeId();
        for (unsigned int j = 0; j < cameraList.size(); ++j) {
            if (cameraList[j]->getShapeNodeName() == mkr_cam_shp_name) {
                cam_node_id = cameraNodes[j].id;
                break;
            }
        }

        // Get bundle name.
        auto mkr_bnd_ptr = mkr_ptr->getBundle();
        auto mkr_bnd_tfm_name = mkr_bnd_ptr->getNodeName();

        mmsg::NodeId bnd_node_id = mmsg::NodeId();
        for (unsigned int j = 0; j < bundleList.size(); ++j) {
            if (bundleList[j]->getNodeName() == mkr_bnd_tfm_name) {
                bnd_node_id = bundleNodes[j].id;
                break;
            }
        }

        auto dag_path = MDagPath();
        status = MDagPath::getAPathTo(mkr_tfm_obj, dag_path);
        CHECK_MSTATUS_AND_RETURN_IT(status);

        MString transform_name = dag_path.fullPathName(&status);
        CHECK_MSTATUS_AND_RETURN_IT(status);
        status = mayaAttr.setNodeName(transform_name);
        CHECK_MSTATUS_AND_RETURN_IT(status);

        get_marker_attrs(
            mayaAttr,
            frameList,
            timeEvalMode,
            out_attrDataBlock,
            mkr_attr_ids,
            out_attrNameToAttrIdMap);

        auto mkr_node = out_sceneGraph.create_marker_node(mkr_attr_ids);
        out_markerNodes.push_back(mkr_node);
        out_evalObjects.add_marker(mkr_node);

        // Create Marker to Bundle and Camera relationships.
        out_sceneGraph.link_marker_to_camera(mkr_node.id, cam_node_id);
        out_sceneGraph.link_marker_to_bundle(mkr_node.id, bnd_node_id);
    }
    return status;
}

MStatus
convert_attributes_to_attr_ids(
    const AttrPtrList &attrList,
    const StringToAttrIdMap &attrNameToAttrIdMap,
    mmsg::AttrDataBlock &out_attrDataBlock,
    std::vector<mmscenegraph::AttrId> &out_attrIdList
) {
    MStatus status = MS::kSuccess;

    out_attrIdList.clear();
    out_attrIdList.reserve(attrList.size());

    for (AttrPtrListCIt ait = attrList.cbegin();
         ait != attrList.cend();
         ++ait) {
        AttrPtr attr = *ait;
        MString attrName = attr->getLongName();
        std::string key = std::string(attrName.asChar());

        // Find mmSceneGraph AttrId from the full (long) attribute
        // name.
        auto search = attrNameToAttrIdMap.find(key);
        if (search != attrNameToAttrIdMap.end()) {
            out_attrIdList.push_back(search->second);
        } else {
            ERR("key was not found: " << key);
            return MStatus::kFailure;
        }
    }
    assert(out_attrIdList.size() == attrList.size());
    return status;
}


MStatus construct_scene_graph(
    CameraPtrList &cameraList,
    MarkerPtrList &markerList,
    BundlePtrList &bundleList,
    AttrPtrList &attrList,
    const MTimeArray &frameList,
    const int timeEvalMode,
    mmsg::SceneGraph &out_sceneGraph,
    mmsg::AttrDataBlock &out_attrDataBlock,
    mmsg::FlatScene &out_flatScene,
    std::vector<mmsg::FrameValue> &out_frameList,
    std::vector<mmsg::CameraNode> &out_cameraNodes,
    std::vector<mmsg::BundleNode> &out_bundleNodes,
    std::vector<mmsg::MarkerNode> &out_markerNodes,
    std::vector<mmscenegraph::AttrId> &out_attrIdList
) {
    MStatus status = MS::kSuccess;

    auto evalObjects = mmsg::EvaluationObjects();
    auto attrNameToAttrIdMap = StringToAttrIdMap();

    add_cameras(
        cameraList, frameList, timeEvalMode,
        out_cameraNodes, evalObjects,
        out_sceneGraph, out_attrDataBlock,
        attrNameToAttrIdMap);

    add_bundles(
        bundleList, frameList, timeEvalMode,
        out_bundleNodes, evalObjects,
        out_sceneGraph, out_attrDataBlock,
        attrNameToAttrIdMap);

    add_markers(
        markerList,
        cameraList, bundleList,
        frameList, timeEvalMode,
        out_cameraNodes, out_bundleNodes,
        out_markerNodes, evalObjects,
        out_sceneGraph, out_attrDataBlock,
        attrNameToAttrIdMap);

    // Create parameters to attributes
    convert_attributes_to_attr_ids(
        attrList,
        attrNameToAttrIdMap,
        out_attrDataBlock,
        out_attrIdList
    );

    // Frames
    auto uiUnit = MTime::uiUnit();
    for (uint32_t i = 0; i < frameList.length(); ++i) {
        MTime frame = frameList[i];
        auto frame_num =
            static_cast<mmsg::FrameValue>(frame.as(uiUnit));
        out_frameList.push_back(frame_num);
    }

    // // Print number of nodes in the evaluation objects.
    // INFO("EvaluationObjects num_bundles: "
    //      << evalObjects.num_bundles());
    // INFO("EvaluationObjects num_cameras: "
    //      << evalObjects.num_cameras());
    // INFO("EvaluationObjects num_markers: "
    //      << evalObjects.num_markers());

    // // Print number of nodes in the scene graph.
    // INFO("SceneGraph num_transform_nodes: "
    //      << out_sceneGraph.num_transform_nodes());
    // INFO("SceneGraph num_bundle_nodes: "
    //      << out_sceneGraph.num_bundle_nodes());
    // INFO("SceneGraph num_camera_nodes: "
    //      << out_sceneGraph.num_camera_nodes());
    // INFO("SceneGraph num_marker_nodes: "
    //      << out_sceneGraph.num_marker_nodes());

    // Bake down SceneGraph into FlatScene for fast evaluation.
    out_flatScene = mmsg::bake_scene_graph(
        out_sceneGraph,
        evalObjects
    );

    return status;
}
