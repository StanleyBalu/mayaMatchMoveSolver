//
// Copyright (C) 2023 David Cattermole.
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

use crate::encoder::ImageExrEncoder;
use crate::metadata::ImageMetaData;
use crate::pixelbuffer::BufferDataType;
use crate::pixelbuffer::ImagePixelBuffer;
use crate::pixeldata::ImagePixelDataF32x4;
use anyhow::bail;
use anyhow::Result;
use exr::prelude::traits::*;

pub mod datatype;
pub mod encoder;
pub mod metadata;
pub mod pixelbuffer;
pub mod pixeldata;

/// Read the Metadata from an EXR image.
//
// https://github.com/johannesvollmer/exrs/blob/master/examples/0b_read_meta.rs
pub fn image_read_metadata_exr(file_path: &str) -> Result<ImageMetaData> {
    // 'pedantic = false' means "do not throw an error for invalid or
    // missing attributes", skipping them instead.
    let pedantic = false;

    let exr_meta_data =
        exr::meta::MetaData::read_from_file(file_path, pedantic)?;

    let image_metadata = if exr_meta_data.headers.len() == 0 {
        ImageMetaData::new()
    } else {
        let exr_header = &exr_meta_data.headers[0];
        // TODO: Get the EXR encoding from the header and return it.
        let image_attributes = &exr_header.shared_attributes;
        let layer_attributes = &exr_header.own_attributes;
        ImageMetaData::with_attributes(image_attributes, layer_attributes)
    };

    Ok(image_metadata)
}

/// Read an EXR image from a file path.
//
// https://github.com/johannesvollmer/exrs/blob/master/GUIDE.md
// https://github.com/johannesvollmer/exrs/blob/master/examples/0c_read_rgba.rs
pub fn image_read_pixels_exr_f32x4(
    file_path: &str,
) -> Result<(ImageMetaData, ImagePixelBuffer)> {
    let image = exr::image::read::read()
        .no_deep_data()
        .largest_resolution_level()
        .rgba_channels(
            |resolution, _channels: &exr::image::RgbaChannels| {
                // instantiate your image type with the size of the image in
                // file.
                let default_pixel = (0.0, 0.0, 0.0, 0.0);
                let empty_line = vec![default_pixel; resolution.width()];
                let empty_image = vec![empty_line; resolution.height()];
                empty_image
            },
            |pixel_vector, position, (r, g, b, a): (f32, f32, f32, f32)| {
                // transfer the colors from the file to your image type,
                // requesting all values to be converted to f32 numbers (you
                // can also directly use f16 instead) and you could also use
                // `Sample` instead of `f32` to keep the original data type
                // from the file
                pixel_vector[position.y()][position.x()] = (r, g, b, a)
            },
        )
        .first_valid_layer()
        .all_attributes()
        .from_file(file_path)?;

    // printing all pixels might kill the console, so only print some
    // meta data about the image.
    let _data_window = image.layer_data.absolute_bounds();
    let layer_attributes = image.layer_data.attributes;
    let image_width = image.layer_data.size.width();
    let image_height = image.layer_data.size.height();

    // println!("Opened file: {}", file_path);
    // println!("Layer Attributes: {:#?}", layer_attributes);
    // println!("Data Window: {:?}", data_window);

    let pixel_count = image_width * image_height;
    let default_pixel = 0.0;
    let mut pixel_data: Vec<f64> = vec![default_pixel; pixel_count * 2];
    let pixel_data_slice = unsafe {
        std::mem::transmute::<&mut [f64], &mut [(f32, f32, f32, f32)]>(
            pixel_data.as_mut_slice(),
        )
    };

    for y in 0..image_height {
        for x in 0..image_width {
            let position = exr::math::Vec2(x, y);
            let pixel = image.layer_data.channel_data.pixels[position.y()]
                [position.x()];
            let pixel_index = (image_width * y) + x;
            pixel_data_slice[pixel_index] = pixel;
        }
    }

    let num_channels = 4;
    let image_data = ImagePixelBuffer::from_data(
        BufferDataType::F32,
        image_width,
        image_height,
        num_channels,
        pixel_data,
    );

    let image_metadata =
        ImageMetaData::with_attributes(&image.attributes, &layer_attributes);

    Ok((image_metadata, image_data))
}

/// Write a 32-bit float image to an EXR file.
//
// https://github.com/johannesvollmer/exrs/blob/master/examples/0a_write_rgba.rs
// https://github.com/johannesvollmer/exrs/blob/master/examples/1a_write_rgba_with_metadata.rs
// https://github.com/johannesvollmer/exrs/blob/master/examples/7_write_raw_blocks.rs
pub fn image_write_pixels_exr_f32x4(
    file_path: &str,
    encoder: ImageExrEncoder,
    meta_data: &ImageMetaData,
    pixel_buffer: &ImagePixelBuffer,
) -> Result<()> {
    // TODO: How can we control the number of threads used for
    // compression?

    let layer_attributes = meta_data.as_layer_attributes();

    let pixel_data = ImagePixelDataF32x4::from_buffer(pixel_buffer);
    let generate_pixels =
        |position: exr::math::Vec2<usize>| (pixel_data.get_pixel(position));

    let encoding = ImageExrEncoder::as_exr_encoding(encoder);
    let layer = exr::image::Layer::new(
        (pixel_buffer.image_width(), pixel_buffer.image_height()),
        layer_attributes,
        encoding,
        exr::image::SpecificChannels::rgba(generate_pixels),
    );

    let mut image = exr::image::Image::from_layer(layer);
    image.attributes = meta_data.as_image_attributes();

    // TODO: Set a specific number of threads to be used by the
    // thread-pool.
    let use_threads = true;
    let ok = if use_threads {
        // Write it to a file with all cores in parallel.
        image.write().to_file(file_path)
    } else {
        // Sequentially compress and write the images (in a single
        // thread).
        image.write().non_parallel().to_file(file_path)
    };

    match ok {
        Ok(..) => Ok(()),
        Err(err) => bail!(err),
    }
}
