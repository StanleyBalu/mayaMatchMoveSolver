//
// Copyright (C) 2020, 2021 David Cattermole.
//
// This file is part of mmSolver.
//
// mmSolver is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// mmSolver is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with mmSolver.  If not, see <https://www.gnu.org/licenses/>.
// ====================================================================
//

use petgraph::algo::toposort as PGtoposort;
use petgraph::graph::NodeIndex as PGNodeIndex;
use petgraph::visit::EdgeRef as PGEdgeRef;
use petgraph::Direction as PGDirection;

use std::collections::hash_set::HashSet;

use crate::attr::AttrCameraIds;
use crate::attr::AttrMarkerIds;
use crate::attr::AttrTransformIds;
use crate::node::traits::NodeCanTranslate2D;
use crate::node::traits::NodeCanViewScene;
use crate::node::traits::NodeHasId;
use crate::node::traits::NodeHasWeight;
use crate::node::NodeId;
use crate::scene::evaluationobjects::EvaluationObjects;
use crate::scene::flat::FlatScene;
use crate::scene::graph::SceneGraph;
use crate::scene::graphiter::UpstreamDepthFirstSearch;

/// Get all upstream node_indices from node_index.
///
/// All returned node_indices are required in the hierarchy for the
/// given node_ids. For example all parents will be present in the
/// returned indices.
fn upstream_node_indices_set(
    sg: &SceneGraph,
    node_ids: &[NodeId],
) -> HashSet<PGNodeIndex> {
    let graph = &sg.get_hierarchy_graph();
    let mut node_indices_set = HashSet::new();
    for node_id in node_ids {
        let node_index = match sg.get_node_index_from_node_id(*node_id) {
            Some(value) => value,
            None => continue,
        };

        let mut walker = UpstreamDepthFirstSearch::new(graph, node_index);
        while let Some((node, _depth)) = walker.next(graph) {
            // println!("node: {:?}", node);
            // println!("depth: {:?}", _depth);
            node_indices_set.insert(node);
        }
    }
    node_indices_set
}

/// Flatten the scene graph into a list of nodes, filtered to only the
/// nodes needed for the input node_ids, and sort the nodes so that
/// parents must appear first in the list, followed by children.
fn flatten_filter_and_sort_graph_nodes(
    sg: &SceneGraph,
    node_ids: Vec<NodeId>,
) -> Option<(Vec<PGNodeIndex>, Vec<NodeId>)> {
    // let debug_string = sg.hierarchy_graph_debug_string();
    // println!("{}", debug_string);

    // Get the node indices, including all parents required.
    let node_indices_set = upstream_node_indices_set(&sg, &node_ids);
    // println!("Node Indices to keep: {:#?}", node_indices_set);

    // Filter-topo-sort.
    let graph = &sg.get_hierarchy_graph();
    match PGtoposort(graph, None) {
        Ok(nodes) => {
            // println!("Toposort nodes: {:#?}", nodes);

            let filtered_nodes: Vec<_> = nodes
                .iter()
                .filter(|node_index| node_indices_set.contains(&node_index))
                .map(|node_index| *node_index)
                .collect();
            // println!("Toposort filtered nodes: {:#?}", filtered_nodes);

            let filtered_node_ids: Vec<_> = filtered_nodes
                .iter()
                .map(|node_index| *graph.node_weight(*node_index).unwrap())
                .collect();

            Some((filtered_nodes, filtered_node_ids))
        }
        Err(_cycle) => {
            // println!("Cycle: {:#?}", _cycle);
            None
        }
    }
}

/// Get the parent index for each node index given.
fn get_parent_index_list(
    sg: &SceneGraph,
    node_indices: &[PGNodeIndex],
) -> Vec<Option<usize>> {
    let mut list = Vec::<Option<usize>>::new();
    let dir = PGDirection::Incoming;
    let graph = &sg.get_hierarchy_graph();
    for (i, node_index) in (0..).zip(node_indices) {
        let edges: Vec<_> = graph.edges_directed(*node_index, dir).collect();
        assert!(edges.len() < 2);
        if edges.len() == 0 {
            list.push(None);
            continue;
        }
        let parent_node_index = edges[0].source();

        let mut parent_index = None;
        let previous_node_indices_iter = node_indices[0..i].iter();
        for (j, previous_node_index) in (0..).zip(previous_node_indices_iter) {
            if *previous_node_index == parent_node_index {
                parent_index = Some(j);
                break;
            }
        }
        list.push(parent_index);
    }
    list
}

/// Bake down graph into a more efficient representation that has an
/// un-editable hierarchy.
pub fn bake_scene_graph(
    sg: &SceneGraph,
    eval_objects: &EvaluationObjects,
) -> FlatScene {
    let bnd_nodes = eval_objects.get_bundles();
    let cam_nodes = eval_objects.get_cameras();
    let mkr_nodes = eval_objects.get_markers();
    let bnd_node_ids: Vec<NodeId> = bnd_nodes
        .iter()
        .map(|x| x.get_id())
        .filter(|x| match x {
            NodeId::Bundle(_) => true,
            _ => false,
        })
        .collect();
    let cam_node_ids: Vec<NodeId> = cam_nodes
        .iter()
        .map(|x| x.get_id())
        .filter(|x| match x {
            NodeId::Camera(_) => true,
            _ => false,
        })
        .collect();
    let mkr_ids: Vec<NodeId> = mkr_nodes
        .iter()
        .map(|x| x.get_id())
        .filter(|x| match x {
            NodeId::Marker(_) => true,
            _ => false,
        })
        .collect();
    assert!(mkr_nodes.len() == mkr_ids.len());
    let bnd_ids = bnd_node_ids.clone();
    let cam_ids = cam_node_ids.clone();
    let tfm_node_ids = bnd_node_ids
        .into_iter()
        .chain(cam_node_ids.into_iter())
        .collect();

    // Organize the transform hierarchy data.
    let (tfm_node_indices, tfm_node_ids) =
        flatten_filter_and_sort_graph_nodes(&sg, tfm_node_ids).unwrap();
    // println!("tfm_node_indices: {:#?}", tfm_node_indices.len());
    // println!("tfm_node_ids: {:#?}", tfm_node_ids.len());

    let tfm_nodes = sg.get_transformable_nodes(&tfm_node_ids).unwrap();
    // println!("tfm_nodes: {:#?}", tfm_nodes.len());

    let tfm_node_parent_indices = get_parent_index_list(&sg, &tfm_node_indices);
    // println!(
    //     "tfm_node_parent_indices: {}",
    //     tfm_node_parent_indices.len()
    // );

    // Get transform attributes.
    let num_transforms = tfm_nodes.len();
    let mut tfm_attr_list = Vec::new();
    let mut rotate_order_list = Vec::new();
    tfm_attr_list.reserve(num_transforms);
    rotate_order_list.reserve(num_transforms);
    for node in tfm_nodes.iter() {
        let attr_tx = node.get_attr_tx();
        let attr_ty = node.get_attr_ty();
        let attr_tz = node.get_attr_tz();

        let attr_rx = node.get_attr_rx();
        let attr_ry = node.get_attr_ry();
        let attr_rz = node.get_attr_rz();

        let attr_sx = node.get_attr_sx();
        let attr_sy = node.get_attr_sy();
        let attr_sz = node.get_attr_sz();

        let attr_tfm = AttrTransformIds {
            tx: attr_tx,
            ty: attr_ty,
            tz: attr_tz,
            //
            rx: attr_rx,
            ry: attr_ry,
            rz: attr_rz,
            //
            sx: attr_sx,
            sy: attr_sy,
            sz: attr_sz,
        };
        tfm_attr_list.push(attr_tfm);

        let rotate_order = node.get_rotate_order();
        rotate_order_list.push(rotate_order);
    }

    // Camera attributes.
    let mut cam_attr_list = Vec::new();
    let mut cam_film_fit_list = Vec::new();
    let mut cam_render_res_list = Vec::new();
    cam_attr_list.reserve(cam_nodes.len());
    cam_film_fit_list.reserve(cam_nodes.len());
    cam_render_res_list.reserve(cam_nodes.len());
    for cam_node in cam_nodes.iter() {
        let attr_sensor_width = cam_node.get_attr_sensor_width();
        let attr_sensor_height = cam_node.get_attr_sensor_height();
        let attr_focal_length = cam_node.get_attr_focal_length();
        let attr_lens_offset_x = cam_node.get_attr_lens_offset_x();
        let attr_lens_offset_y = cam_node.get_attr_lens_offset_y();
        let attr_near_clip_plane = cam_node.get_attr_near_clip_plane();
        let attr_far_clip_plane = cam_node.get_attr_far_clip_plane();
        let attr_camera_scale = cam_node.get_attr_camera_scale();

        let cam_attrs = AttrCameraIds {
            sensor_width: attr_sensor_width,
            sensor_height: attr_sensor_height,
            focal_length: attr_focal_length,
            lens_offset_x: attr_lens_offset_x,
            lens_offset_y: attr_lens_offset_y,
            near_clip_plane: attr_near_clip_plane,
            far_clip_plane: attr_far_clip_plane,
            camera_scale: attr_camera_scale,
        };
        cam_attr_list.push(cam_attrs);

        let film_fit = cam_node.get_film_fit();
        cam_film_fit_list.push(film_fit);

        let render_image_width = cam_node.get_render_image_width();
        let render_image_height = cam_node.get_render_image_height();
        cam_render_res_list.push((render_image_width, render_image_height));
    }

    // Marker attributes
    let mut mkr_attr_list = Vec::new();
    mkr_attr_list.reserve(mkr_nodes.len());
    for mkr_node in mkr_nodes.iter() {
        let attr_tx = mkr_node.get_attr_tx();
        let attr_ty = mkr_node.get_attr_ty();
        let attr_weight = mkr_node.get_attr_weight();

        let mkr_attrs = AttrMarkerIds {
            tx: attr_tx,
            ty: attr_ty,
            weight: attr_weight,
        };

        mkr_attr_list.push(mkr_attrs);
    }

    // Marker to bundle indices.
    let mut mkr_bnd_indices = Vec::new();
    mkr_bnd_indices.reserve(mkr_ids.len());
    for mkr_id in &mkr_ids {
        let bnd_id =
            sg.get_bundle_node_id_from_marker_node_id(*mkr_id).unwrap();
        for (bnd_index, bnd_node_id) in (0..).zip(bnd_ids.iter()) {
            if *bnd_node_id == bnd_id {
                mkr_bnd_indices.push(bnd_index);
                break;
            }
        }
    }

    // Marker to camera indices.
    let mut mkr_cam_indices = Vec::new();
    mkr_cam_indices.reserve(mkr_ids.len());
    for mkr_id in &mkr_ids {
        let cam_id =
            sg.get_camera_node_id_from_marker_node_id(*mkr_id).unwrap();
        for (cam_index, cam_node_id) in (0..).zip(cam_ids.iter()) {
            if *cam_node_id == cam_id {
                mkr_cam_indices.push(cam_index);
                break;
            }
        }
    }

    FlatScene::new(
        // Node ids.
        bnd_ids,
        cam_ids,
        mkr_ids,
        // Marker to camera and marker to bundle relationships.
        mkr_cam_indices,
        mkr_bnd_indices,
        // Attributes.
        tfm_attr_list,
        rotate_order_list,
        cam_attr_list,
        cam_film_fit_list,
        cam_render_res_list,
        mkr_attr_list,
        // Transform hierarchy struture and metadata.
        tfm_node_ids,
        tfm_node_indices,
        tfm_node_parent_indices,
    )
}

// #[cfg(test)]
// mod tests {
//     use super::*;
//     #[test]
//     fn test_bake_scene_graph() {}
// }
