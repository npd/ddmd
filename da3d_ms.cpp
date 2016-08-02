//
// Created by Nicola Pierazzo on 02/08/16.
//

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstring>

#include "da3d/Image.hpp"
#include "da3d/DA3D.hpp"
#include "nl-bayes/Image.hpp"
#include "nl-bayes/utils.hpp"
#include "nl-bayes/NLBayes.h"
#include "DCT.hpp"

using imgutils::pick_option;
using imgutils::read_image;
using imgutils::save_image;
using imgutils::Image;
using imgutils::dct_inplace;
using imgutils::idct_inplace;
using std::cerr;
using std::endl;
using std::string;
using std::move;
using std::vector;
using std::sqrt;
using std::to_string;

vector<Image> decompose(const Image &img, int levels) {
  vector<Image> pyramid;
  Image freq = img.copy();
  dct_inplace(freq);
  int h{freq.rows()}, w{freq.columns()};
  for (int i = 0; i < levels; ++i) {
    // Copy data
    Image layer(h, w, freq.channels());
    for (int ch = 0; ch < freq.channels(); ++ch) {
      for (int r = 0; r < h; ++r) {
        for (int c = 0; c < w; ++c) {
          layer.val(c, r, ch) = freq.val(c, r, ch);
        }
      }
    }
    // Inverse DCT
    idct_inplace(layer);
    w /= 2;
    h /= 2;
    pyramid.push_back(move(layer));
  }
  return pyramid;
}

Image recompose(const vector<Image> &pyramid, float recompose_factor) {
  // Use the bigger image to determine width, height and number of channels
  Image output = pyramid[0].copy();
  // Perform the DCT
  dct_inplace(output);

  for (int i = 1; i < static_cast<int>(pyramid.size()); ++i) {
    // Read level i of the pyramid
    Image layer = pyramid[i].copy();
    // Perform the DCT
    dct_inplace(layer);
    // Copy data (selected by recompose_factor)
    for (int ch = 0; ch < layer.channels(); ++ch) {
      for (int r = 0; r < layer.rows() * recompose_factor; ++r) {
        for (int c = 0; c < layer.columns() * recompose_factor; ++c) {
          output.val(c, r, ch) = layer.val(c, r, ch);
        }
      }
    }
  }
  // IDCT of the output image
  idct_inplace(output);
  return output;
}

da3d::Image to_vec(const Image &input) {
  da3d::Image output(input.rows(), input.columns(), input.channels());
  for (int row = 0; row < input.rows(); ++row) {
    for (int col = 0; col < input.columns(); ++col) {
      for (int chan = 0; chan < input.channels(); ++chan) {
        output.val(col, row, chan) = input.val(col, row, chan);
      }
    }
  }
  return output;
}

Image to_split(const da3d::Image &input) {
  Image output(input.rows(), input.columns(), input.channels());
  for (int row = 0; row < input.rows(); ++row) {
    for (int col = 0; col < input.columns(); ++col) {
      for (int chan = 0; chan < input.channels(); ++chan) {
        output.val(col, row, chan) = input.val(col, row, chan);
      }
    }
  }
  return output;
}

Image DA3D(const Image &noisy, const Image &guide, float sigma) {
  return to_split(da3d::DA3D(to_vec(noisy), to_vec(guide), sigma, vector<float>(), vector<float>(), false));
}

int main(int argc, char **argv) {
  const bool usage = static_cast<bool>(pick_option(&argc, argv, "h", nullptr));
  const bool verbose = static_cast<bool>(pick_option(&argc, argv, "v", nullptr));
  const float recompose_factor
      = static_cast<float>(atof(pick_option(&argc, argv, "c", ".6")));
  const int scales = atoi(pick_option(&argc, argv, "n", "3"));
  const char* out_single = pick_option(&argc, argv, "single", "");
  const char* out_ms = pick_option(&argc, argv, "ms", "");
  const char* out_da3d = pick_option(&argc, argv, "da3d", "");

  if (usage || argc < 2) {
    cerr << "usage: " << argv[0] << " sigma [input [output]] [-c recomposition "
         << "factor] [-n scales] [-single file] [-ms file] [-da3d file] [-v]"
         << endl;
    return usage ? EXIT_SUCCESS : EXIT_FAILURE;
  }

#ifndef _OPENMP
  cerr << "Warning: OpenMP not available. The algorithm will run in a single" <<
       " thread." << endl;
#endif

  Image noisy = read_image(argc > 2 ? argv[2] : "-");
  const float sigma = static_cast<float>(atof(argv[1]));

  if (verbose) cerr << "Decomposing input image" << endl;
  vector<Image> noisy_p = decompose(noisy, scales);
  vector<Image> denoised_p;

  for (Image &layer : noisy_p) {
    float s = sigma * sqrt(static_cast<float>(layer.pixels()) / noisy.pixels());
    if (verbose) cerr << "NL-Bayes with sigma = " << s << endl;
    Image guide = NLBstep1(layer, s);
    denoised_p.push_back(NLBstep2(layer, guide, s));
  }

  if (strlen(out_single)) save_image(denoised_p[0], out_single);

  if (strlen(out_ms)) save_image(recompose(denoised_p, recompose_factor), out_ms);

  if (verbose) cerr << "DA3D of first level" << endl;
  denoised_p[0] = DA3D(noisy_p[0], denoised_p[0], sigma);

  if (strlen(out_da3d)) save_image(denoised_p[0], out_da3d);

  if (verbose) cerr << "Recomposing pyramid" << endl;
  Image result = recompose(denoised_p, recompose_factor);
  save_image(result, argc > 3 ? argv[3] : "TIFF:-");

  return EXIT_SUCCESS;
}
