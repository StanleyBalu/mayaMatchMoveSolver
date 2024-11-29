//
// Copyright (C) 2024 David Cattermole.
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

#![allow(unused_imports)]
#![allow(unused_variables)]
#![allow(dead_code)]

mod common;

use anyhow::Result;
use approx::assert_relative_eq;
use std::ffi::OsString;

use crate::common::chan_data_filter_only_x;
use crate::common::chan_data_filter_only_y;
use crate::common::construct_input_file_path;
use crate::common::construct_output_file_path;
use crate::common::find_data_dir;
use crate::common::print_actual_pops;
use crate::common::print_chan_data;
use crate::common::read_chan_file;
use crate::common::save_chart_linear_n3_regression_pop;
use crate::common::save_chart_linear_n_points_regression_pop;
use crate::common::save_chart_linear_regression_pop;
use crate::common::CHART_RESOLUTION;

use mmscenegraph_rust::constant::Real;
use mmscenegraph_rust::curve::detect::pops::detect_curve_pops;
use mmscenegraph_rust::curve::detect::pops::filter_curve_pops;
use mmscenegraph_rust::curve::detect::pops::PopPoint;
use mmscenegraph_rust::curve::resample::resample_uniform_xy;
use mmscenegraph_rust::math::curve_fit::linear_regression;
use mmscenegraph_rust::math::curve_fit::nonlinear_line_n3;
use mmscenegraph_rust::math::curve_fit::nonlinear_line_n_points;
use mmscenegraph_rust::math::curve_fit::AngleRadian;
use mmscenegraph_rust::math::curve_fit::Point2;
use mmscenegraph_rust::math::interpolate::InterpolationMethod;

fn compare_detected_vs_actual_pops(
    actual_pop_times: &[Real],
    pops: &[PopPoint],
) -> usize {
    let pop_times: Vec<Real> = pops.iter().map(|x| x.time).collect();

    let mut not_found_count = 0;
    for actual_pop_time in actual_pop_times {
        let found =
            pop_times.iter().find(|&&x| x == *actual_pop_time).is_some();
        if !found {
            println!("actual_pop_time={actual_pop_time} not found");
            not_found_count += 1;
        }
    }

    not_found_count
}

fn pops_common(
    chart_title: &str,
    in_pop_file_name: &str,
    in_raw_file_name: &str,
    out_file_name: &str,
    threshold: Real,
    resample_start: Real,
    resample_end: Real,
    resample_by: Real,
) -> Result<(
    OsString,
    Vec<Real>,
    Vec<PopPoint>,
    Vec<(Real, Real)>,
    Vec<(Real, Real)>,
    Vec<(Real, Real)>,
)> {
    let data_dir = find_data_dir()?;
    let in_file_path_pop =
        construct_input_file_path(&data_dir, in_pop_file_name)?;
    let in_file_path = construct_input_file_path(&data_dir, in_raw_file_name)?;
    let out_file_path = construct_output_file_path(&data_dir, out_file_name)?;

    let data_pop = read_chan_file(&in_file_path_pop.as_os_str())?;
    let data_raw = read_chan_file(&in_file_path.as_os_str())?;
    // print_chan_data(&data);
    let x_values = chan_data_filter_only_x(&data_pop);
    let y_values = chan_data_filter_only_y(&data_pop);
    let y_values_raw = chan_data_filter_only_y(&data_raw);

    let actual_pops =
        print_actual_pops(&x_values, &y_values, &y_values_raw, 1.0e-9);

    let pops = detect_curve_pops(&x_values, &y_values, threshold)?;
    for pop in &pops {
        println!("pop: {pop:?}");
    }

    let interpolation = InterpolationMethod::Linear;
    let data_filtered = filter_curve_pops(&x_values, &y_values, threshold)?;
    let data_filtered = resample_uniform_xy(
        &data_filtered,
        resample_start,
        resample_end,
        resample_by,
        interpolation,
    )?;

    Ok((
        out_file_path.into(),
        actual_pops,
        pops,
        data_raw,
        data_pop,
        data_filtered,
    ))
}

fn pops_common_linear_regression(
    chart_title: &str,
    in_pop_file_name: &str,
    in_raw_file_name: &str,
    out_file_name: &str,
    threshold: Real,
    resample_start: Real,
    resample_end: Real,
    resample_by: Real,
) -> Result<(Vec<Real>, Vec<PopPoint>, Point2, AngleRadian)> {
    let (out_file_path, actual_pops, pops, data_raw, data_pop, data_filtered) =
        pops_common(
            chart_title,
            in_pop_file_name,
            in_raw_file_name,
            out_file_name,
            threshold,
            resample_start,
            resample_end,
            resample_by,
        )?;

    let x_values_filtered = chan_data_filter_only_x(&data_filtered);
    let y_values_filtered = chan_data_filter_only_y(&data_filtered);
    let (point, slope) =
        linear_regression(&x_values_filtered, &y_values_filtered)?;
    println!("point={point:?} slope={slope:?}");

    let chart_resolution = CHART_RESOLUTION;
    save_chart_linear_regression_pop(
        &data_raw,
        &data_pop,
        &data_filtered,
        point,
        slope,
        chart_title,
        &out_file_path,
        chart_resolution,
    )?;

    assert_eq!(data_filtered.len(), data_raw.len());
    assert!(pops.len() >= actual_pops.len());

    Ok((actual_pops, pops, point, slope))
}

fn pops_common_line_n3(
    chart_title: &str,
    in_pop_file_name: &str,
    in_raw_file_name: &str,
    out_file_name: &str,
    threshold: Real,
    resample_start: Real,
    resample_end: Real,
    resample_by: Real,
) -> Result<(Vec<Real>, Vec<PopPoint>, Point2, Point2, Point2)> {
    let (out_file_path, actual_pops, pops, data_raw, data_pop, data_filtered) =
        pops_common(
            chart_title,
            in_pop_file_name,
            in_raw_file_name,
            out_file_name,
            threshold,
            resample_start,
            resample_end,
            resample_by,
        )?;

    let x_values_filtered = chan_data_filter_only_x(&data_filtered);
    let y_values_filtered = chan_data_filter_only_y(&data_filtered);
    let (point_a, point_b, point_c) =
        nonlinear_line_n3(&x_values_filtered, &y_values_filtered)?;
    println!("point_a={point_a:?}");
    println!("point_b={point_b:?}");
    println!("point_c={point_c:?}");

    let chart_resolution = CHART_RESOLUTION;
    save_chart_linear_n3_regression_pop(
        &data_raw,
        &data_pop,
        &data_filtered,
        point_a,
        point_b,
        point_c,
        chart_title,
        &out_file_path.as_os_str(),
        chart_resolution,
    )?;

    Ok((actual_pops, pops, point_a, point_b, point_c))
}

fn pops_common_line_n_points(
    chart_title: &str,
    in_pop_file_name: &str,
    in_raw_file_name: &str,
    out_file_name: &str,
    threshold: Real,
    resample_start: Real,
    resample_end: Real,
    resample_by: Real,
    n_points: usize,
    interpolation_method: InterpolationMethod,
) -> Result<(Vec<Real>, Vec<PopPoint>, Vec<Point2>)> {
    let (out_file_path, actual_pops, pops, data_raw, data_pop, data_filtered) =
        pops_common(
            chart_title,
            in_pop_file_name,
            in_raw_file_name,
            out_file_name,
            threshold,
            resample_start,
            resample_end,
            resample_by,
        )?;

    let x_values_filtered = chan_data_filter_only_x(&data_filtered);
    let y_values_filtered = chan_data_filter_only_y(&data_filtered);
    let points = nonlinear_line_n_points(
        &x_values_filtered,
        &y_values_filtered,
        n_points,
        interpolation_method,
    )?;
    println!("points={points:?}");

    let chart_resolution = CHART_RESOLUTION;
    save_chart_linear_n_points_regression_pop(
        &data_raw,
        &data_pop,
        &data_filtered,
        &points,
        chart_title,
        &out_file_path.as_os_str(),
        chart_resolution,
    )?;

    Ok((actual_pops, pops, points))
}

#[test]
fn pops_identity_pop1() -> Result<()> {
    let chart_title = "pops_identity_pop1";
    let in_pop_file_name = "identity_pop1.chan";
    let in_raw_file_name = "identity_raw.chan";
    let out_file_name = "pops_identity_pop1.png";
    let threshold = 0.8;
    let resample_start = 1001.0;
    let resample_end = 1101.0;
    let resample_by = 1.0;

    let (actual_pops, pops, point, slope) = pops_common_linear_regression(
        chart_title,
        in_pop_file_name,
        in_raw_file_name,
        out_file_name,
        threshold,
        resample_start,
        resample_end,
        resample_by,
    )?;

    let not_found_count = compare_detected_vs_actual_pops(&actual_pops, &pops);
    assert_eq!(not_found_count, 1);
    assert!(pops.len() < 13);

    assert_relative_eq!(point.x(), 1051.0, epsilon = 1.0e-9);
    assert_relative_eq!(point.y(), 0.0, epsilon = 1.0e-5);
    assert_relative_eq!(slope.as_degrees(), 0.0, epsilon = 1.0e-3);

    Ok(())
}

#[test]
fn pops_identity_pop2() -> Result<()> {
    let chart_title = "pops_identity_pop2";
    let in_pop_file_name = "identity_pop2.chan";
    let in_raw_file_name = "identity_raw.chan";
    let out_file_name = "pops_identity_pop2.png";
    let threshold = 0.5;
    let resample_start = 1001.0;
    let resample_end = 1101.0;
    let resample_by = 1.0;

    let (actual_pops, pops, point, slope) = pops_common_linear_regression(
        chart_title,
        in_pop_file_name,
        in_raw_file_name,
        out_file_name,
        threshold,
        resample_start,
        resample_end,
        resample_by,
    )?;

    let not_found_count = compare_detected_vs_actual_pops(&actual_pops, &pops);
    assert_eq!(not_found_count, 3);
    assert!(pops.len() < 19);

    assert_relative_eq!(point.x(), 1051.0, epsilon = 1.0e-9);
    assert_relative_eq!(point.y(), 0.0, epsilon = 1.0e-2);
    assert_relative_eq!(slope.as_degrees(), 0.0, epsilon = 1.0e-2);

    Ok(())
}

#[test]
fn pops_linear_3_point_pop3() -> Result<()> {
    let chart_title = "pops_linear_3_point_pop3";
    let in_pop_file_name = "linear_3_point_pop3.chan";
    let in_raw_file_name = "linear_3_point_raw.chan";
    let out_file_name = "pops_linear_3_point_pop3.png";
    let threshold = 1.0;
    let resample_start = 1001.0;
    let resample_end = 1101.0;
    let resample_by = 1.0;

    let (actual_pops, pops, point_a, point_b, point_c) = pops_common_line_n3(
        chart_title,
        in_pop_file_name,
        in_raw_file_name,
        out_file_name,
        threshold,
        resample_start,
        resample_end,
        resample_by,
    )?;

    let not_found_count = compare_detected_vs_actual_pops(&actual_pops, &pops);
    assert_eq!(not_found_count, 2);
    assert!(pops.len() < 19);

    assert_relative_eq!(point_a.x(), 1001.0, epsilon = 1.0e-1);
    assert_relative_eq!(point_a.y(), -1.21533949192, epsilon = 0.3);

    assert_relative_eq!(point_b.x(), 1051.0, epsilon = 1.0e-9);
    assert_relative_eq!(point_b.y(), 4.5, epsilon = 0.2);

    assert_relative_eq!(point_c.x(), 1101.0, epsilon = 1.0e-1);
    assert_relative_eq!(point_c.y(), 1.855, epsilon = 0.3);

    Ok(())
}

#[test]
fn pops_degree_45_up_pop3() -> Result<()> {
    let chart_title = "pops_degree_45_up_pop3";
    let in_pop_file_name = "degree_45_up_pop3.chan";
    let in_raw_file_name = "degree_45_up_raw.chan";
    let out_file_name = "pops_degree_45_up_pop3.png";
    let threshold = 0.75;
    let resample_start = 1001.0;
    let resample_end = 1101.0;
    let resample_by = 1.0;

    let (actual_pops, pops, point, slope) = pops_common_linear_regression(
        chart_title,
        in_pop_file_name,
        in_raw_file_name,
        out_file_name,
        threshold,
        resample_start,
        resample_end,
        resample_by,
    )?;

    let not_found_count = compare_detected_vs_actual_pops(&actual_pops, &pops);
    assert_eq!(not_found_count, 1);
    assert!(pops.len() < 23);

    assert_relative_eq!(point.x(), 1051.0, epsilon = 1.0e-9);
    assert_relative_eq!(point.y(), 50.0, epsilon = 0.05);
    assert_relative_eq!(slope.as_degrees(), 45.0, epsilon = 1.0e-1);

    Ok(())
}

#[test]
fn pops_bounce_5_up_down_pop3() -> Result<()> {
    let chart_title = "pops_bounce_5_up_down_pop3";
    let in_pop_file_name = "bounce_5_up_down_pop3.chan";
    let in_raw_file_name = "bounce_5_up_down_raw.chan";
    let out_file_name = "pops_bounce_5_up_down_pop3.png";
    let threshold = 1.0;
    let resample_start = 1001.0;
    let resample_end = 1101.0;
    let resample_by = 1.0;
    let n_points = 5;
    let interpolation_method = InterpolationMethod::CubicNUBS;

    let (actual_pops, pops, points) = pops_common_line_n_points(
        chart_title,
        in_pop_file_name,
        in_raw_file_name,
        out_file_name,
        threshold,
        resample_start,
        resample_end,
        resample_by,
        n_points,
        interpolation_method,
    )?;

    let point_a = points[0];
    let point_b = points[1];
    let point_c = points[2];
    let point_d = points[3];
    let point_e = points[4];
    println!("point_a={point_a:?}");
    println!("point_b={point_b:?}");
    println!("point_c={point_c:?}");
    println!("point_d={point_d:?}");
    println!("point_e={point_e:?}");

    let not_found_count = compare_detected_vs_actual_pops(&actual_pops, &pops);
    assert_eq!(not_found_count, 0);
    assert!(pops.len() < 14);

    assert_relative_eq!(point_a.x(), 1001.0, epsilon = 1.0e-9);
    assert_relative_eq!(point_a.y(), -2.0439280344879265, epsilon = 1.0e-9);

    assert_relative_eq!(point_b.x(), 1026.0, epsilon = 1.0e-9);
    assert_relative_eq!(point_b.y(), 5.442782586088859, epsilon = 1.0e-9);

    assert_relative_eq!(point_c.x(), 1051.0, epsilon = 1.0e-1);
    assert_relative_eq!(point_c.y(), -4.322908639680146, epsilon = 1.0e-9);

    assert_relative_eq!(point_d.x(), 1076.0, epsilon = 1.0e-9);
    assert_relative_eq!(point_d.y(), 5.850924226414753, epsilon = 1.0e-9);

    assert_relative_eq!(point_e.x(), 1101.0, epsilon = 1.0e-9);
    assert_relative_eq!(point_e.y(), -4.009521216807484, epsilon = 1.0e-9);

    Ok(())
}
