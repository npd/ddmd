//
// Created by Nicola Pierazzo on 02/08/16.
//

#include <fftw3.h>
#include "DCT.hpp"

namespace imgutils {

void dct_inplace(Image &img) {
  int n[] = {img.rows(), img.columns()};
  fftwf_r2r_kind dct2[] = {FFTW_REDFT10, FFTW_REDFT10};
  fftwf_plan plan = fftwf_plan_many_r2r(2, n, img.channels(), img.data(), NULL,
                                        1, img.pixels(), img.data(), NULL, 1,
                                        img.pixels(), dct2, FFTW_ESTIMATE);
  fftwf_execute(plan);
  fftwf_destroy_plan(plan);

  // Normalization
  for (int i = 0; i < img.samples(); ++i) {
    img.val(i) /= 4 * img.pixels();
  }
}

void idct_inplace(Image &img) {
  int n[] = {img.rows(), img.columns()};
  fftwf_r2r_kind idct2[] = {FFTW_REDFT01, FFTW_REDFT01};
  fftwf_plan plan = fftwf_plan_many_r2r(2, n, img.channels(), img.data(), NULL,
                                        1, img.pixels(), img.data(), NULL, 1,
                                        img.pixels(), idct2, FFTW_ESTIMATE);
  fftwf_execute(plan);
  fftwf_destroy_plan(plan);
}

}
