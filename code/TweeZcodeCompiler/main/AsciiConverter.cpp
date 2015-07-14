//
// Created by Danni on 13.07.15.
//

#include "AsciiConverter.h"

#include <aalib.h>
#include <cstring>
#include <iostream>
#include <jpeglib.h>
#include <cmath>
#include <assert.h>
#include <sstream>

using namespace std;


/**
 * Bilinear resize RGB image.
 * pixels is an array of size w * h * 3. (rgb)
 * Target dimension is w2 * h2 * 3. (rgb)
 * w2 * h2 cannot be zero.
 *
 * @param pixels Image pixels.
 * @param w Image width.
 * @param h Image height.
 * @param w2 New width.
 * @param h2 New height.
 * @return New array with size w2 * h2.
 */
unsigned char* resizeBilinear(unsigned char* pixels, unsigned w, unsigned h, unsigned w2, unsigned h2) {
    const int elemCount = 3;
    unsigned char* temp = new unsigned char[w2*h2*elemCount]();

    int a, b, c, d, x, y, index;
    float x_ratio = ((float)(w-1))/w2;
    float y_ratio = ((float)(h-1))/h2;
    float x_diff, y_diff, blue, red, green;
    int offset = 0;
    for (int i=0;i<h2;i++) {
        for (int j=0;j<w2;j++) {
            x = (int)(x_ratio * j);
            y = (int)(y_ratio * i);
            x_diff = (x_ratio * j) - x;
            y_diff = (y_ratio * i) - y;
            index = elemCount*(y*w+x);
            a = index;
            b = index+1*elemCount;
            c = index+w*elemCount;
            d = index+(w+1)*elemCount;


            // red element
            // Yr = Ar(1-w)(1-h) + Br(w)(1-h) + Cr(h)(1-w) + Dr(wh)
            red = pixels[a]*(1-x_diff)*(1-y_diff) + pixels[b]*(x_diff)*(1-y_diff) +
                  pixels[c]*(y_diff)*(1-x_diff) + pixels[d]*(x_diff*y_diff);

            // green element
            // Yg = Ag(1-w)(1-h) + Bg(w)(1-h) + Cg(h)(1-w) + Dg(wh)
            green = pixels[a+1]*(1-x_diff)*(1-y_diff) + pixels[b+1]*(x_diff)*(1-y_diff) +
                    pixels[c+1]*(y_diff)*(1-x_diff) + pixels[d+1]*(x_diff*y_diff);

            // blue element
            // Yb = Ab(1-w)(1-h) + Bb(w)(1-h) + Cb(h)(1-w) + Db(wh)
            blue = pixels[a+2]*(1-x_diff)*(1-y_diff) + pixels[b+2]*(x_diff)*(1-y_diff) +
                   pixels[c+2]*(y_diff)*(1-x_diff) + pixels[d+2]*(x_diff*y_diff);


            temp[offset] = (unsigned char)red;
            temp[offset+1] = (unsigned char)green;
            temp[offset+2] = (unsigned char)blue;

            offset += elemCount;
        }
    }
    return temp;
}


string AsciiConverter::convertJpgToAscii(string filePath, unsigned int width) {
    // libjpeg
    unsigned int jpgwidth, jpgheight;
    unsigned int data_size;     // length of the file
    int channels;               //  3 =>RGB   4 =>RGBA
    unsigned int type;
    unsigned char * rowptr[1];    // pointer to an array
    unsigned char * jdata;        // data for the image
    struct jpeg_decompress_struct info; //for our jpeg info
    struct jpeg_error_mgr err;          //the error handler

    FILE* file = fopen(filePath.c_str(), "rb");

    if(!file) {
        cerr << "Error reading JPEG file %s" << filePath << endl;
        throw std::exception();
    }

    info.err = jpeg_std_error(& err);

    jpeg_create_decompress(&info);

    jpeg_stdio_src(&info, file);
    jpeg_read_header(&info, TRUE);

    jpeg_start_decompress(&info);

    jpgwidth = info.output_width;
    jpgheight = info.output_height;
    channels = info.num_components;

    if(channels != 3) {
        cerr << "More (or less) than 3 color channels are not supported (maybe the jpeg img uses alpha values?)" << endl;
        jpeg_finish_decompress(&info);
        throw exception();
    }

    data_size = jpgwidth * jpgheight * 3;

    jdata = (unsigned char *)malloc(data_size);
    while (info.output_scanline < info.output_height) // loop
    {
        // Enable jpeg_read_scanlines() to fill our jdata array
        rowptr[0] = (unsigned char *)jdata +  // secret to method
                    3* info.output_width * info.output_scanline;

        jpeg_read_scanlines(&info, rowptr, 1);
    }

    jpeg_finish_decompress(&info);

    float scale = (float)width / (float)jpgwidth * 2;
    const int targetWidth = jpgwidth * scale;
    const int targetHeight = jpgheight * scale * 0.5;

    unsigned char* resized = resizeBilinear(jdata, jpgwidth, jpgheight, targetWidth, targetHeight);

    fclose(file);


    /*FILE* outfile = fopen("/tmp/test.jpeg", "wb");

    if (!outfile)
        return;

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr       jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width      = targetWidth;
    cinfo.image_height     = targetHeight;
    cinfo.input_components = 3;
    cinfo.in_color_space   = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    //set the quality [0..100]
    jpeg_set_quality (&cinfo, 75, true);
    jpeg_start_compress(&cinfo, true);

    JSAMPROW row_pointer;          // pointer to a single row
    unsigned char* stride = (unsigned char *)malloc(targetWidth * 3);
    for (int i=0; i<targetHeight; i++) {
        memcpy(stride, resized + i * targetWidth * 3, targetWidth * 3);
        row_pointer = stride;
        jpeg_write_scanlines(&cinfo, &row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);

    free(stride);*/

    // aalib
    struct aa_hardware_params myaaparams = aa_defparams;
    myaaparams.width = targetWidth/2;
    myaaparams.height = targetHeight/2;
    aa_context* context = aa_init(&mem_d, &myaaparams, NULL);


    const struct aa_driver* mymemd = &mem_d;

    for(int y = 0; y < aa_imgheight(context); ++y) {
        for(int x = 0; x < aa_imgwidth(context); ++x) {
            int flatcoord = 3*(targetWidth*y + x);
            assert(flatcoord < 3*(targetWidth * targetHeight));

            unsigned char r = resized[flatcoord];
            unsigned char g = resized[flatcoord+1];
            unsigned char b = resized[flatcoord+2];
            //cout << "x: " << x << " y: " << y << " | " << flatcoord << ": " << (int)r << "::" << (int)g << "::" << (int)b << endl;

            aa_putpixel(context, x, y, 0.21 * r + 0.72 * g + 0.07 * b);
        }
    }

    aa_renderparams* renderparams = aa_getrenderparams();
    renderparams->dither = AA_FLOYD_S;
    aa_render(context, renderparams, 0, 0, aa_scrwidth(context), aa_scrheight(context));

    char* text = reinterpret_cast<char*>aa_text(context);
    stringstream ss;

    for(int y = 0; y < aa_scrheight(context); ++y) {
        for (int x = 0; x < aa_scrwidth(context); ++x) {
            ss << text[aa_scrwidth(context) * y + x];
        }
        ss << endl;
    }

    aa_flush(context);

    aa_close(context);

    free(jdata);

    delete[] resized;

    return ss.str();
}


