/*
 * Copyright (C) 2022 David Cattermole.
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
 * Command for running mmReadImage.
 */

#include "MMReadImageCmd.h"

// Maya
#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <maya/MDagPath.h>
#include <maya/MFileObject.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MImage.h>
#include <maya/MMatrix.h>
#include <maya/MMatrixArray.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MSyntax.h>

// MM Solver
#include "mmSolver/image/image_io.h"
#include "mmSolver/utilities/debug_utils.h"

// Command arguments and command name:
#define FILE_PATH_FLAG "-fp"
#define FILE_PATH_FLAG_LONG "-filePath"

#define WIDTH_HEIGHT_FLAG "-wh"
#define WIDTH_HEIGHT_FLAG_LONG "-widthHeight"

// TODO: Add flags to:
// - Return the number of channels in the image.
// - Return the number of bytes per-channel in the image.
// - Return the number of total raw bytes in the image.
//
// NOTE: We do not want to have to call mmReadImage multiple times. We
// want to get as much data as possible in a single call, because
// subsequent calls will need to re-read the image.

namespace mmsolver {

MMReadImageCmd::~MMReadImageCmd() {}

void *MMReadImageCmd::creator() { return new MMReadImageCmd(); }

MString MMReadImageCmd::cmdName() { return MString("mmReadImage"); }

/*
 * Tell Maya we have a syntax function.
 */
bool MMReadImageCmd::hasSyntax() const { return true; }

bool MMReadImageCmd::isUndoable() const { return false; }

/*
 * Add flags to the command syntax
 */
MSyntax MMReadImageCmd::newSyntax() {
    MSyntax syntax;
    syntax.enableQuery(true);
    syntax.enableEdit(false);

    auto minNumObjects = 1;
    auto maxNumObjects = 1;
    syntax.setObjectType(MSyntax::kStringObjects, minNumObjects, maxNumObjects);

    syntax.addFlag(WIDTH_HEIGHT_FLAG, WIDTH_HEIGHT_FLAG_LONG,
                   MSyntax::kBoolean);
    return syntax;
}

/*
 * Parse command line arguments
 */
MStatus MMReadImageCmd::parseArgs(const MArgList &args) {
    MStatus status = MStatus::kSuccess;

    MArgDatabase argData(syntax(), args, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    MStringArray objects;
    argData.getObjects(objects);
    if (objects.length() == 0) {
        status = MStatus::kFailure;
        status.perror(
            "mmReadImage: No file path was was not given, but is required!");
        return status;
    }
    if (objects.length() > 1) {
        status = MStatus::kFailure;
        status.perror(
            "mmReadImage: "
            "More than one file path was was not given,"
            " only one file path should be given.");
        return status;
    }
    m_file_path = objects[0];

    // Query Flag
    const bool query = argData.isQuery(&status);
    CHECK_MSTATUS(status);
    if (status != MStatus::kSuccess) {
        status.perror("mmReadImage: Could not get the query flag");
        return status;
    }
    if (!query) {
        status = MStatus::kFailure;
        status.perror("mmReadImage command must query using the 'query' flag");
        return status;
    }

    m_query_width_height = argData.isFlagSet(WIDTH_HEIGHT_FLAG, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    return status;
}

MStatus MMReadImageCmd::doIt(const MArgList &args) {
    const bool verbose = false;
    MStatus status = MStatus::kSuccess;

    // Read all the flag arguments.
    status = parseArgs(args);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    auto file_object = MFileObject();
    file_object.setRawFullName(m_file_path);
    file_object.setResolveMethod(MFileObject::kInputFile);

    bool path_exists = file_object.exists();
    if (!path_exists) {
        MString resolved_file_path = file_object.resolvedFullName();
        status = MS::kFailure;
        MMSOLVER_MAYA_WRN("mmReadImage: Could not find file path "
                          << "\"" << m_file_path.asChar()
                          << "\", resolved path "
                          << "\"" << resolved_file_path.asChar() << "\".");
        return status;
    }
    MString resolved_file_path = file_object.resolvedFullName();
    if (resolved_file_path.length() > 0) {
        MMSOLVER_MAYA_VRB("mmReadImage: resolved file path "
                          << "\"" << resolved_file_path.asChar() << "\".");
        m_file_path = file_object.resolvedFullName();
    }

    if (m_query_width_height) {
        auto image = MImage();

        // kUnknown attempts to load the native pixel type.
        auto pixel_type = MImage::kUnknown;

        // TODO: Can we read just the file header to get the image
        // size? This would remove the need to read the entire image
        // for this command's usage.
        uint32_t image_width;
        uint32_t image_height;
        uint8_t num_channels;
        uint8_t bytes_per_channel;
        MHWRender::MRasterFormat texture_format;
        image::PixelDataType pixel_data_type;
        void *pixel_data = nullptr;
        status = image::read_image_file(image, m_file_path, pixel_type,
                                        image_width, image_height, num_channels,
                                        bytes_per_channel, texture_format,
                                        pixel_data_type, pixel_data);
        if (status != MS::kSuccess) {
            status = MS::kSuccess;
            MMSOLVER_MAYA_WRN("mmReadImage: "
                              << "Image file path could not be read: "
                              << m_file_path.asChar());
            return status;
        }

        MIntArray outResult;
        outResult.append(image_width);
        outResult.append(image_height);
        MMReadImageCmd::setResult(outResult);
    }
    return status;
}

}  // namespace mmsolver
