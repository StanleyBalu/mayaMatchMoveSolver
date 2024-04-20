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

use criterion::measurement::WallTime;
use criterion::{black_box, criterion_group, criterion_main, Criterion};

use rand::distributions::Uniform;
use rand::thread_rng;
use rand::Rng;

use mmscenegraph_rust::attr::datablock::AttrDataBlock;
use mmscenegraph_rust::constant::Matrix44;
use mmscenegraph_rust::constant::Real;
use mmscenegraph_rust::math::camera::get_projection_matrix;
use mmscenegraph_rust::math::camera::FilmFit;
use mmscenegraph_rust::math::reprojection::reproject_as_normalised_coord;
use mmscenegraph_rust::math::rotate::euler::RotateOrder;
use mmscenegraph_rust::math::transform::calculate_matrix;
use mmscenegraph_rust::math::transform::calculate_matrix_with_values;
use mmscenegraph_rust::math::transform::Transform;
use mmscenegraph_rust::node::traits::NodeHasId;
use mmscenegraph_rust::node::NodeId;
use mmscenegraph_rust::scene::bake::bake_scene_graph;
use mmscenegraph_rust::scene::evaluationobjects::EvaluationObjects;
use mmscenegraph_rust::scene::graph::SceneGraph;
use mmscenegraph_rust::scene::helper::create_static_bundle;
use mmscenegraph_rust::scene::helper::create_static_camera;
use mmscenegraph_rust::scene::helper::create_static_marker;
use mmscenegraph_rust::scene::helper::create_static_transform;

fn bench_transform_calculate_matrix(c: &mut Criterion) {
    let tx = -2.0;
    let ty = 2.0;
    let tz = 5.0;
    let rx = 10.0;
    let ry = -10.0;
    let rz = -10.0;
    let sx = 1.0;
    let sy = 1.0;
    let sz = 1.0;

    let camera_transform = Transform::from_txyz_rxyz_sxyz(
        tx,
        ty,
        tz,
        rx,
        ry,
        rz,
        RotateOrder::XYZ,
        sx,
        sy,
        sz,
    );

    c.bench_function("transform::calculate_matrix", |b| {
        b.iter(|| calculate_matrix(black_box(&camera_transform)))
    });
}

fn bench_transform_calculate_matrix_with_values(c: &mut Criterion) {
    let tx = -2.0;
    let ty = 2.0;
    let tz = 5.0;
    let rx = 10.0;
    let ry = -10.0;
    let rz = -10.0;
    let sx = 1.0;
    let sy = 1.0;
    let sz = 1.0;
    let roo = RotateOrder::XYZ;

    c.bench_function("transform::calculate_matrix_with_values", |b| {
        b.iter(|| {
            calculate_matrix_with_values(
                black_box(tx),
                black_box(ty),
                black_box(tz),
                black_box(rx),
                black_box(ry),
                black_box(rz),
                black_box(sx),
                black_box(sy),
                black_box(sz),
                black_box(roo),
            )
        })
    });
}

fn bench_camera_get_projection_matrix(c: &mut Criterion) {
    let focal_length = 35.0;
    let film_back_width = 36.0 / 25.4;
    let film_back_height = 24.0 / 25.4;
    let film_offset_x = 0.0;
    let film_offset_y = 0.0;
    let image_width = 2048.0; // 3600.0; // 960.0;
    let image_height = 1556.0; // 2400.0; // 540.0;
    let film_fit = FilmFit::Horizontal;
    let near_clip_plane = 0.1;
    let far_clip_plane = 10000.0;
    let camera_scale = 1.0;

    c.bench_function("camera::get_projection_matrix", |b| {
        b.iter(|| {
            get_projection_matrix(
                black_box(focal_length),
                black_box(film_back_width),
                black_box(film_back_height),
                black_box(film_offset_x),
                black_box(film_offset_y),
                black_box(image_width),
                black_box(image_height),
                black_box(film_fit),
                black_box(near_clip_plane),
                black_box(far_clip_plane),
                black_box(camera_scale),
            );
        })
    });
}

fn bench_reprojection_reproject_as_normalised_coord(c: &mut Criterion) {
    // Test with both translation and rotation of the camera matrix.

    // 3D Point to be reprojected
    let px = -0.5;
    let py = 2.7;
    let pz = 0.0;

    // Camera Transform Values
    let tx = -2.0;
    let ty = 2.0;
    let tz = 5.0;
    let rx = 10.0;
    let ry = -10.0;
    let rz = -10.0;

    // Camera Projection Values
    let focal_length = 35.0;
    let film_back_width = 36.0 / 25.4;
    let film_back_height = 24.0 / 25.4;
    let film_offset_x = 0.0;
    let film_offset_y = 0.0;
    let image_width = 2048.0; // 3600.0; // 960.0;
    let image_height = 1556.0; // 2400.0; // 540.0;
    let film_fit = FilmFit::Horizontal;
    let near_clip_plane = 0.1;
    let far_clip_plane = 10000.0;
    let camera_scale = 1.0;

    let camera_transform = Transform {
        sx: 1.0,
        sy: 1.0,
        sz: 1.0,
        spx: 0.0,
        spy: 0.0,
        spz: 0.0,
        sptx: 0.0,
        spty: 0.0,
        sptz: 0.0,
        shxy: 0.0,
        shxz: 0.0,
        shyz: 0.0,
        roo: RotateOrder::XYZ,
        rx: rx,
        ry: ry,
        rz: rz,
        rax: rx,
        ray: ry,
        raz: rz,
        rpx: 0.0,
        rpy: 0.0,
        rpz: 0.0,
        rptx: 0.0,
        rpty: 0.0,
        rptz: 0.0,
        tx: tx,
        ty: ty,
        tz: tz,
    };
    let camera_transform_matrix = calculate_matrix(&camera_transform);
    let camera_projection_matrix = get_projection_matrix(
        focal_length,
        film_back_width,
        film_back_height,
        film_offset_x,
        film_offset_y,
        image_width,
        image_height,
        film_fit,
        near_clip_plane,
        far_clip_plane,
        camera_scale,
    );

    let point = Matrix44::new(
        1.0, 0.0, 0.0, px, //
        0.0, 1.0, 0.0, py, //
        0.0, 0.0, 1.0, pz, //
        0.0, 0.0, 0.0, 1.0, //
    );

    c.bench_function("reprojection::reproject_as_normalised_coord", |b| {
        b.iter(|| {
            let screen_point = reproject_as_normalised_coord(
                black_box(camera_transform_matrix),
                black_box(camera_projection_matrix),
                black_box(point),
            );
            screen_point
        })
    });
}

fn bench_reprojection(c: &mut Criterion) {
    // Test with both translation and rotation of the camera matrix.

    // 3D Point to be reprojected
    let px = -0.5;
    let py = 2.7;
    let pz = 0.0;

    // Camera Transform Values
    let tx = -2.0;
    let ty = 2.0;
    let tz = 5.0;
    let rx = 10.0;
    let ry = -10.0;
    let rz = -10.0;

    // Camera Projection Values
    let focal_length = 35.0;
    let film_back_width = 36.0 / 25.4;
    let film_back_height = 24.0 / 25.4;
    let film_offset_x = 0.0;
    let film_offset_y = 0.0;
    let image_width = 2048.0; // 3600.0; // 960.0;
    let image_height = 1556.0; // 2400.0; // 540.0;
    let film_fit = FilmFit::Horizontal;
    let near_clip_plane = 0.1;
    let far_clip_plane = 10000.0;
    let camera_scale = 1.0;

    c.bench_function("reprojection", |b| {
        b.iter(|| {
            let camera_transform = Transform {
                sx: 1.0,
                sy: 1.0,
                sz: 1.0,
                spx: 0.0,
                spy: 0.0,
                spz: 0.0,
                sptx: 0.0,
                spty: 0.0,
                sptz: 0.0,
                shxy: 0.0,
                shxz: 0.0,
                shyz: 0.0,
                roo: RotateOrder::XYZ,
                rx: black_box(rx),
                ry: black_box(ry),
                rz: black_box(rz),
                rax: 0.0,
                ray: 0.0,
                raz: 0.0,
                rpx: 0.0,
                rpy: 0.0,
                rpz: 0.0,
                rptx: 0.0,
                rpty: 0.0,
                rptz: 0.0,
                tx: black_box(tx),
                ty: black_box(ty),
                tz: black_box(tz),
            };
            let camera_transform_matrix = calculate_matrix(&camera_transform);
            let camera_projection_matrix = get_projection_matrix(
                black_box(focal_length),
                black_box(film_back_width),
                black_box(film_back_height),
                black_box(film_offset_x),
                black_box(film_offset_y),
                black_box(image_width),
                black_box(image_height),
                black_box(film_fit),
                black_box(near_clip_plane),
                black_box(far_clip_plane),
                black_box(camera_scale),
            );
            let point = Matrix44::new(
                1.0,
                0.0,
                0.0,
                black_box(px), //
                0.0,
                1.0,
                0.0,
                black_box(py), //
                0.0,
                0.0,
                1.0,
                black_box(pz), //
                0.0,
                0.0,
                0.0,
                1.0, //
            );
            let screen_point = reproject_as_normalised_coord(
                black_box(camera_transform_matrix),
                black_box(camera_projection_matrix),
                black_box(point),
            );
            screen_point
        })
    });
}

fn bench_construct_scene_graph_static_transforms(c: &mut Criterion) {
    const MAX_MIN_TRANSLATE_VALUE: Real = 10000.0;
    const MAX_MIN_ROTATE_VALUE: Real = 180.0;
    const MAX_MIN_SCALE_VALUE: Real = 10000.0;

    let mut rng = thread_rng();
    let translate_side =
        Uniform::new(-MAX_MIN_TRANSLATE_VALUE, MAX_MIN_TRANSLATE_VALUE);
    let rotate_side = Uniform::new(-MAX_MIN_ROTATE_VALUE, MAX_MIN_ROTATE_VALUE);
    let scale_side = Uniform::new(-MAX_MIN_SCALE_VALUE, MAX_MIN_SCALE_VALUE);

    c.bench_function("construct_scene_graph_static_transforms", |b| {
        b.iter(|| {
            let mut sg = SceneGraph::new();
            let mut attrdb = AttrDataBlock::new();

            for _ in 0..100 {
                let rotate_order = RotateOrder::ZXY;
                let tx = rng.sample(translate_side);
                let ty = rng.sample(translate_side);
                let tz = rng.sample(translate_side);
                let rx = rng.sample(rotate_side);
                let ry = rng.sample(rotate_side);
                let rz = rng.sample(rotate_side);
                let sx = rng.sample(scale_side);
                let sy = rng.sample(scale_side);
                let sz = rng.sample(scale_side);
                create_static_transform(
                    &mut sg,
                    &mut attrdb,
                    (tx, ty, tz),
                    (rx, ry, rz),
                    (sx, sy, sz),
                    rotate_order,
                );
            }
            black_box(sg.get_hierarchy_graph());
        })
    });
}

fn bench_construct_scene_graph_hierarchy_transforms(c: &mut Criterion) {
    const MAX_MIN_TRANSLATE_VALUE: Real = 10000.0;
    const MAX_MIN_ROTATE_VALUE: Real = 180.0;
    const MAX_MIN_SCALE_VALUE: Real = 10000.0;

    let mut rng = thread_rng();
    let translate_side =
        Uniform::new(-MAX_MIN_TRANSLATE_VALUE, MAX_MIN_TRANSLATE_VALUE);
    let rotate_side = Uniform::new(-MAX_MIN_ROTATE_VALUE, MAX_MIN_ROTATE_VALUE);
    let scale_side = Uniform::new(-MAX_MIN_SCALE_VALUE, MAX_MIN_SCALE_VALUE);

    c.bench_function("construct_scene_graph_hierarchy_transforms", |b| {
        b.iter(|| {
            let mut sg = SceneGraph::new();
            let mut attrdb = AttrDataBlock::new();

            // Transform A (parented under to root)
            let rotate_order = RotateOrder::ZXY;
            let tx = rng.sample(translate_side);
            let ty = rng.sample(translate_side);
            let tz = rng.sample(translate_side);
            let rx = rng.sample(rotate_side);
            let ry = rng.sample(rotate_side);
            let rz = rng.sample(rotate_side);
            let sx = rng.sample(scale_side);
            let sy = rng.sample(scale_side);
            let sz = rng.sample(scale_side);
            let tfm_a = create_static_transform(
                &mut sg,
                &mut attrdb,
                (tx, ty, tz),
                (rx, ry, rz),
                (sx, sy, sz),
                rotate_order,
            );
            sg.set_node_parent(tfm_a.get_id(), NodeId::Root);

            // Transform B (parented under Transform A)
            let tx = rng.sample(translate_side);
            let ty = rng.sample(translate_side);
            let tz = rng.sample(translate_side);
            let rx = rng.sample(rotate_side);
            let ry = rng.sample(rotate_side);
            let rz = rng.sample(rotate_side);
            let sx = rng.sample(scale_side);
            let sy = rng.sample(scale_side);
            let sz = rng.sample(scale_side);
            let tfm_b = create_static_transform(
                &mut sg,
                &mut attrdb,
                (tx, ty, tz),
                (rx, ry, rz),
                (sx, sy, sz),
                rotate_order,
            );
            sg.set_node_parent(tfm_b.get_id(), tfm_a.get_id());

            // Transform C (parented under root)
            let tx = rng.sample(translate_side);
            let ty = rng.sample(translate_side);
            let tz = rng.sample(translate_side);
            let rx = rng.sample(rotate_side);
            let ry = rng.sample(rotate_side);
            let rz = rng.sample(rotate_side);
            let sx = rng.sample(scale_side);
            let sy = rng.sample(scale_side);
            let sz = rng.sample(scale_side);
            let tfm_c = create_static_transform(
                &mut sg,
                &mut attrdb,
                (tx, ty, tz),
                (rx, ry, rz),
                (sx, sy, sz),
                rotate_order,
            );
            sg.set_node_parent(tfm_c.get_id(), NodeId::Root);

            // Transform D (parented under Transform B)
            let tx = rng.sample(translate_side);
            let ty = rng.sample(translate_side);
            let tz = rng.sample(translate_side);
            let rx = rng.sample(rotate_side);
            let ry = rng.sample(rotate_side);
            let rz = rng.sample(rotate_side);
            let sx = rng.sample(scale_side);
            let sy = rng.sample(scale_side);
            let sz = rng.sample(scale_side);
            let tfm_d = create_static_transform(
                &mut sg,
                &mut attrdb,
                (tx, ty, tz),
                (rx, ry, rz),
                (sx, sy, sz),
                rotate_order,
            );
            sg.set_node_parent(tfm_d.get_id(), tfm_b.get_id());

            // Transform E (parented under Transform D)
            let tx = rng.sample(translate_side);
            let ty = rng.sample(translate_side);
            let tz = rng.sample(translate_side);
            let rx = rng.sample(rotate_side);
            let ry = rng.sample(rotate_side);
            let rz = rng.sample(rotate_side);
            let sx = rng.sample(scale_side);
            let sy = rng.sample(scale_side);
            let sz = rng.sample(scale_side);
            let tfm_e = create_static_transform(
                &mut sg,
                &mut attrdb,
                (tx, ty, tz),
                (rx, ry, rz),
                (sx, sy, sz),
                rotate_order,
            );
            sg.set_node_parent(tfm_e.get_id(), tfm_d.get_id());

            // Transform F (parented under Transform E)
            let tx = rng.sample(translate_side);
            let ty = rng.sample(translate_side);
            let tz = rng.sample(translate_side);
            let rx = rng.sample(rotate_side);
            let ry = rng.sample(rotate_side);
            let rz = rng.sample(rotate_side);
            let sx = rng.sample(scale_side);
            let sy = rng.sample(scale_side);
            let sz = rng.sample(scale_side);
            let tfm_f = create_static_transform(
                &mut sg,
                &mut attrdb,
                (tx, ty, tz),
                (rx, ry, rz),
                (sx, sy, sz),
                rotate_order,
            );
            sg.set_node_parent(tfm_f.get_id(), tfm_e.get_id());

            // Transform G (parented under Transform F)
            let tx = rng.sample(translate_side);
            let ty = rng.sample(translate_side);
            let tz = rng.sample(translate_side);
            let rx = rng.sample(rotate_side);
            let ry = rng.sample(rotate_side);
            let rz = rng.sample(rotate_side);
            let sx = rng.sample(scale_side);
            let sy = rng.sample(scale_side);
            let sz = rng.sample(scale_side);
            let tfm_g = create_static_transform(
                &mut sg,
                &mut attrdb,
                (tx, ty, tz),
                (rx, ry, rz),
                (sx, sy, sz),
                rotate_order,
            );
            sg.set_node_parent(tfm_g.get_id(), tfm_f.get_id());

            black_box(sg.get_hierarchy_graph());
        })
    });
}

fn bench_construct_scene_graph_depth_transforms(c: &mut Criterion) {
    const MAX_MIN_TRANSLATE_VALUE: Real = 10000.0;
    const MAX_MIN_ROTATE_VALUE: Real = 180.0;
    const MAX_MIN_SCALE_VALUE: Real = 10000.0;

    let mut rng = thread_rng();
    let translate_side =
        Uniform::new(-MAX_MIN_TRANSLATE_VALUE, MAX_MIN_TRANSLATE_VALUE);
    let rotate_side = Uniform::new(-MAX_MIN_ROTATE_VALUE, MAX_MIN_ROTATE_VALUE);
    let scale_side = Uniform::new(-MAX_MIN_SCALE_VALUE, MAX_MIN_SCALE_VALUE);

    c.bench_function("construct_scene_graph_depth_transforms", |b| {
        b.iter(|| {
            let mut sg = SceneGraph::new();
            let mut attrdb = AttrDataBlock::new();

            let mut parent_index = NodeId::Root;

            for _ in 0..100 {
                // Transform (parented under previous transform)
                let rotate_order = RotateOrder::ZXY;
                let tx = rng.sample(translate_side);
                let ty = rng.sample(translate_side);
                let tz = rng.sample(translate_side);
                let rx = rng.sample(rotate_side);
                let ry = rng.sample(rotate_side);
                let rz = rng.sample(rotate_side);
                let sx = rng.sample(scale_side);
                let sy = rng.sample(scale_side);
                let sz = rng.sample(scale_side);
                let tfm = create_static_transform(
                    &mut sg,
                    &mut attrdb,
                    (tx, ty, tz),
                    (rx, ry, rz),
                    (sx, sy, sz),
                    rotate_order,
                );
                let tfm_node_id = tfm.get_id();
                sg.set_node_parent(tfm_node_id, parent_index);
                parent_index = tfm_node_id;
            }

            black_box(sg.get_hierarchy_graph());
        })
    });
}

fn bench_construct_and_evaluate_scene_graph(c: &mut Criterion) {
    const MAX_MIN_TRANSLATE_VALUE: Real = 10000.0;
    const MAX_MIN_ROTATE_VALUE: Real = 180.0;
    const MAX_MIN_SCALE_VALUE: Real = 10000.0;
    const MAX_MIN_MARKER_VALUE: Real = 0.5;

    let mut rng = thread_rng();
    let translate_side =
        Uniform::new(-MAX_MIN_TRANSLATE_VALUE, MAX_MIN_TRANSLATE_VALUE);
    let rotate_side = Uniform::new(-MAX_MIN_ROTATE_VALUE, MAX_MIN_ROTATE_VALUE);
    let scale_side = Uniform::new(-MAX_MIN_SCALE_VALUE, MAX_MIN_SCALE_VALUE);
    let marker_side = Uniform::new(-MAX_MIN_MARKER_VALUE, MAX_MIN_MARKER_VALUE);

    c.bench_function("construct_and_evaluate_scene_graph", |b| {
        b.iter(|| {
            let mut sg = SceneGraph::new();
            let mut attrdb = AttrDataBlock::new();
            let mut eval_objects = EvaluationObjects::new();

            let tx = rng.sample(translate_side);
            let ty = rng.sample(translate_side);
            let tz = rng.sample(translate_side);
            let rx = rng.sample(rotate_side);
            let ry = rng.sample(rotate_side);
            let rz = rng.sample(rotate_side);
            let cam = create_static_camera(
                &mut sg,
                &mut attrdb,
                (tx, ty, tz),
                (rx, ry, rz),
                // Cameras should always have no scale values.
                (1.0, 1.0, 1.0),
                (36.0, 24.0),
                35.0,
                (0.0, 0.0),
                1.0,
                10000.0,
                1.0,
                RotateOrder::ZXY,
                FilmFit::Horizontal,
                2048,
                2048,
            );
            eval_objects.add_camera(cam);

            let rotate_order = RotateOrder::ZXY;
            for _ in 0..100 {
                let tx = rng.sample(translate_side);
                let ty = rng.sample(translate_side);
                let tz = rng.sample(translate_side);
                let rx = rng.sample(rotate_side);
                let ry = rng.sample(rotate_side);
                let rz = rng.sample(rotate_side);
                let sx = rng.sample(scale_side);
                let sy = rng.sample(scale_side);
                let sz = rng.sample(scale_side);
                let tfm = create_static_transform(
                    &mut sg,
                    &mut attrdb,
                    (tx, ty, tz),
                    (rx, ry, rz),
                    (sx, sy, sz),
                    rotate_order,
                );

                let tx = rng.sample(translate_side);
                let ty = rng.sample(translate_side);
                let tz = rng.sample(translate_side);
                let rx = rng.sample(rotate_side);
                let ry = rng.sample(rotate_side);
                let rz = rng.sample(rotate_side);
                let sx = rng.sample(scale_side);
                let sy = rng.sample(scale_side);
                let sz = rng.sample(scale_side);
                let bnd = create_static_bundle(
                    &mut sg,
                    &mut attrdb,
                    (tx, ty, tz),
                    (rx, ry, rz),
                    (sx, sy, sz),
                    rotate_order,
                );

                let mkr_tx = rng.sample(marker_side);
                let mkr_ty = rng.sample(marker_side);
                let mkr = create_static_marker(
                    &mut sg,
                    &mut attrdb,
                    (mkr_tx, mkr_ty),
                    1.0,
                );

                sg.set_node_parent(bnd.get_id(), tfm.get_id());
                sg.link_marker_to_camera(mkr.get_id(), cam.get_id());
                sg.link_marker_to_bundle(mkr.get_id(), bnd.get_id());

                eval_objects.add_marker(mkr);
                eval_objects.add_bundle(bnd);
            }

            let mut flat_scene = bake_scene_graph(&sg, &eval_objects);

            let mut frame_list = Vec::new();
            for frame in 1001..1121 {
                frame_list.push(frame);
            }

            flat_scene.evaluate(&attrdb, &frame_list);

            let out_point_list = flat_scene.points();
            black_box(out_point_list);
            let points_iter = out_point_list.chunks_exact(2);
            for (i, point) in (0..).zip(points_iter) {
                black_box(i);
                black_box(point);
            }
        })
    });
}

// fn bench_compute_dag_matrices_deep(c: &mut Criterion) {
//     let mut group = c.benchmark_group("dag::compute_matrices (deep graph)");
//     for size in [1, 2, 10, 20, 100, 200, 1000, 2000].iter() {
//         // Set up clone of data structures to be passed to function.
//         let depth = 100;
//         let scene = create_scene_depth(*size, depth);
//         let expect_count = scene.transforms.len() + 1;
//         let world_matrix_list = vec![Matrix44::identity(); expect_count];
//         let local_matrix_list = vec![Matrix44::identity(); expect_count];
//         let parent_world_matrix_list = vec![Matrix44::identity(); expect_count];

//         group.throughput(Throughput::Elements(expect_count as u64));
//         group.bench_with_input(BenchmarkId::from_parameter(size), size, move |b, &_size| {
//             b.iter_batched(
//                 || {
//                     let data = (
//                         scene.transforms.clone(),
//                         scene.out_transforms.clone(),
//                         scene.transform_parents.clone(),
//                         scene.params.values.clone(),
//                         local_matrix_list.clone(),
//                         world_matrix_list.clone(),
//                         parent_world_matrix_list.clone(),
//                     );
//                     data
//                 },
//                 |mut data| {
//                     mmsg::dag::compute_matrices(
//                         black_box(data.0),
//                         black_box(&mut data.1),
//                         black_box(&mut data.2),
//                         black_box(&mut data.3),
//                         black_box(&mut data.4),
//                         black_box(&mut data.5),
//                         black_box(&mut data.6),
//                     );
//                 },
//                 BatchSize::LargeInput,
//             )
//         });
//     }
//     group.finish();
// }

// fn create_scene_wide(size: usize, wide: usize) -> mmsg::scene::SceneGraph {
//     let mut scene = mmsg::scene::SceneGraph::new();
//     let tfm_root = mmsg::scene::ROOT_INDEX;

//     const MAX_MIN_TRANSLATE_VALUE: Real = 10000.0;
//     const MAX_MIN_ROTATE_VALUE: Real = 180.0;
//     const MAX_MIN_SCALE_VALUE: Real = 10000.0;

//     // Transform A (parented under to root)
//     let mut rng = thread_rng();
//     let translate_side = Uniform::new(-MAX_MIN_TRANSLATE_VALUE, MAX_MIN_TRANSLATE_VALUE);
//     let rotate_side = Uniform::new(-MAX_MIN_ROTATE_VALUE, MAX_MIN_ROTATE_VALUE);
//     let scale_side = Uniform::new(-MAX_MIN_SCALE_VALUE, MAX_MIN_SCALE_VALUE);
//     for _ in 0..size {
//         for _j in 0..wide {
//             // Transform (unparented)
//             let tx = rng.sample(translate_side);
//             let ty = rng.sample(translate_side);
//             let tz = rng.sample(translate_side);
//             let rx = rng.sample(rotate_side);
//             let ry = rng.sample(rotate_side);
//             let rz = rng.sample(rotate_side);
//             let sx = rng.sample(scale_side);
//             let sy = rng.sample(scale_side);
//             let sz = rng.sample(scale_side);
//             let tfm = mmsg::scene::add_transform_to_scene(
//                 tx,
//                 ty,
//                 tz,
//                 rx,
//                 ry,
//                 rz,
//                 sx,
//                 sy,
//                 sz,
//                 RotateOrder::ZXY,
//                 tfm_root,
//                 &mut scene,
//             );
//         }
//     }

//     scene
// }

// fn bench_compute_dag_matrices_wide(c: &mut Criterion) {
//     let mut group = c.benchmark_group("dag::compute_matrices (wide graph)");
//     for size in [1, 2, 10, 20, 100, 200, 1000, 2000].iter() {
//         // Set up clone of data structures to be passed to function.
//         let wide = 100;
//         let scene = create_scene_wide(*size, wide);
//         let expect_count = scene.transforms.len() + 1;
//         let world_matrix_list = vec![Matrix44::identity(); expect_count];
//         let local_matrix_list = vec![Matrix44::identity(); expect_count];
//         let parent_world_matrix_list = vec![Matrix44::identity(); expect_count];

//         group.throughput(Throughput::Elements(expect_count as u64));
//         group.bench_with_input(BenchmarkId::from_parameter(size), size, move |b, &_size| {
//             b.iter_batched(
//                 || {
//                     let data = (
//                         scene.transforms.clone(),
//                         scene.out_transforms.clone(),
//                         scene.transform_parents.clone(),
//                         scene.params.values.clone(),
//                         local_matrix_list.clone(),
//                         world_matrix_list.clone(),
//                         parent_world_matrix_list.clone(),
//                     );
//                     data
//                 },
//                 |mut data| {
//                     mmsg::dag::compute_matrices(
//                         black_box(data.0),
//                         black_box(&mut data.1),
//                         black_box(&mut data.2),
//                         black_box(&mut data.3),
//                         black_box(&mut data.4),
//                         black_box(&mut data.5),
//                         black_box(&mut data.6),
//                     );
//                 },
//                 BatchSize::LargeInput,
//             )
//         });
//     }
//     group.finish();
// }

// // fn bench_topological_sort(c: &mut Criterion) {
// //     let mut group = c.benchmark_group("dag::compute_matrices (wide graph)");
// //     for size in [1, 2, 10, 20, 100, 200, 1000, 2000].iter() {
// //         // Set up clone of data structures to be passed to function.
// //         let wide = 100;
// //         let scene = create_scene_wide(*size, wide);
// //         let expect_count = scene.transforms.len() + 1;
// //         let world_matrix_list = vec![Matrix44::identity(); expect_count];
// //         let local_matrix_list = vec![Matrix44::identity(); expect_count];
// //         let parent_world_matrix_list = vec![Matrix44::identity(); expect_count];

// //         group.throughput(Throughput::Elements(expect_count as u64));
// //         group.bench_with_input(BenchmarkId::from_parameter(size), size, move |b, &_size| {
// //             b.iter_batched(
// //                 || {
// //                     let data = (
// //                         scene.transforms.clone(),
// //                         scene.out_transforms.clone(),
// //                         scene.transform_parents.clone(),
// //                         scene.params.values.clone(),
// //                         local_matrix_list.clone(),
// //                         world_matrix_list.clone(),
// //                         parent_world_matrix_list.clone(),
// //                     );
// //                     data
// //                 },
// //                 |mut data| {
// //                     mmsg::dag::topological_sort(
// //                         black_box(data.0),
// //                         black_box(&mut data.1),
// //                         black_box(&mut data.2),
// //                         black_box(&mut data.3),
// //                         black_box(&mut data.4),
// //                         black_box(&mut data.5),
// //                         black_box(&mut data.6),
// //                     );
// //                 },
// //                 BatchSize::LargeInput,
// //             )
// //         });
// //     }
// //     group.finish();
// // }

// fn bench_compute_dag_matrices(c: &mut Criterion) {
//     let mut group = c.benchmark_group("dag::compute_matrices (normal)");
//     for size in [1, 2, 10, 20, 100, 200, 1000, 2000].iter() {
//         // Set up clone of data structures to be passed to function.
//         let scene = create_scene_normal(*size);

//         let expect_count = scene.transforms.len() + 1;
//         let world_matrix_list = vec![Matrix44::identity(); expect_count];
//         let local_matrix_list = vec![Matrix44::identity(); expect_count];
//         let parent_world_matrix_list = vec![Matrix44::identity(); expect_count];

//         group.throughput(Throughput::Elements(expect_count as u64));
//         group.bench_with_input(BenchmarkId::from_parameter(size), size, move |b, &_size| {
//             b.iter_batched(
//                 || {
//                     let data = (
//                         scene.transforms.clone(),
//                         scene.out_transforms.clone(),
//                         scene.transform_parents.clone(),
//                         scene.params.values.clone(),
//                         local_matrix_list.clone(),
//                         world_matrix_list.clone(),
//                         parent_world_matrix_list.clone(),
//                     );
//                     data
//                 },
//                 |mut data| {
//                     mmsg::dag::compute_matrices(
//                         black_box(data.0),
//                         black_box(&mut data.1),
//                         black_box(&mut data.2),
//                         black_box(&mut data.3),
//                         black_box(&mut data.4),
//                         black_box(&mut data.5),
//                         black_box(&mut data.6),
//                     );
//                 },
//                 BatchSize::LargeInput,
//             )
//         });
//     }
//     group.finish();
// }

criterion_group!(
    name = benches;
    config = Criterion::default().with_measurement(WallTime);
    targets =
        bench_transform_calculate_matrix,
        bench_transform_calculate_matrix_with_values,
        bench_camera_get_projection_matrix,
        bench_reprojection_reproject_as_normalised_coord,
        bench_reprojection,
        bench_construct_scene_graph_static_transforms,
        bench_construct_scene_graph_hierarchy_transforms,
        bench_construct_scene_graph_depth_transforms,
        bench_construct_and_evaluate_scene_graph,
        // bench_compute_dag_matrices,
        // bench_compute_dag_matrices_deep,
        // bench_compute_dag_matrices_wide
);
criterion_main!(benches);
