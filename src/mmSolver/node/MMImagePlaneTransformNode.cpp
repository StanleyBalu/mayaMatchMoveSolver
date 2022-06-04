/*
 * Copyright (C) 2021 David Cattermole.
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
 * A dummy transform node that acts exactly the same as Maya's normal
 * transform node, but with a different icon.
 */

#include "MMImagePlaneTransformNode.h"

// Maya
#include <maya/MPxTransformationMatrix.h>
#include <maya/MTypeId.h>

// MM Solver
#include "mmSolver/mayahelper/maya_camera.h"
#include "mmSolver/nodeTypeIds.h"

namespace mmsolver {

MTypeId MMImagePlaneTransformNode::m_id(MM_IMAGE_PLANE_TRANSFORM_TYPE_ID);

MString MMImagePlaneTransformNode::nodeName() {
    return MString(MM_IMAGE_PLANE_TRANSFORM_TYPE_NAME);
}

MMImagePlaneTransformNode::MMImagePlaneTransformNode() : MPxTransform() {}

// Maya 2020+ will manage the creation of MPxTransformationMatrix on
// demand for us.
#if MAYA_API_VERSION < 20200000
MMImagePlaneTransformNode::MMImagePlaneTransformNode(
    MPxTransformationMatrix *tm)
    : MPxTransform(tm) {}
#endif

void MMImagePlaneTransformNode::postConstructor() {
    MPxTransform::postConstructor();
#if MAYA_API_VERSION < 20190000
    if (baseTransformationMatrix == nullptr) {
        baseTransformationMatrix = new MPxTransformationMatrix();
    }
#endif
}

MMImagePlaneTransformNode::~MMImagePlaneTransformNode(){
    // Empty function body rather than '= default', to allow compiling
    // under Visual Studio 2012.
};

void *MMImagePlaneTransformNode::creator() {
    return (new MMImagePlaneTransformNode());
}

MStatus MMImagePlaneTransformNode::initialize() { return MS::kSuccess; }

}  // namespace mmsolver
