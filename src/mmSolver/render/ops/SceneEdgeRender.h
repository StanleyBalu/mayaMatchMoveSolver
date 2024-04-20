/*
 * Copyright (C) 2021, 2023 David Cattermole.
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

#ifndef MM_SOLVER_RENDER_OPS_SCENE_EDGE_RENDER_H
#define MM_SOLVER_RENDER_OPS_SCENE_EDGE_RENDER_H

// Maya
#include <maya/MString.h>
#include <maya/MViewport2Renderer.h>

// MM Solver
#include "mmSolver/render/data/constants.h"
#include "mmSolver/render/ops/SceneRenderBase.h"
#include "mmSolver/render/ops/scene_utils.h"

namespace mmsolver {
namespace render {

class SceneEdgeRender : public SceneRenderBase {
public:
    SceneEdgeRender(const MString &name);
    ~SceneEdgeRender() override;

    const MHWRender::MShaderInstance *shaderOverride() override;

    void setEdgeColor(const float r, const float g, const float b) {
        m_edge_color.r = r;
        m_edge_color.g = g;
        m_edge_color.b = b;
    }
    void setEdgeAlpha(const float value) { m_edge_alpha = value; }
    void setEdgeThickness(const float value) { m_edge_thickness = value; }

protected:
    // Shader override for surfaces
    MHWRender::MShaderInstance *m_shader_override;

    MColor m_edge_color;
    float m_edge_alpha;
    float m_edge_thickness;
};

}  // namespace render
}  // namespace mmsolver

#endif  // MM_SOLVER_RENDER_OPS_SCENE_EDGE_RENDER_H
