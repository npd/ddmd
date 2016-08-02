//
// Created by Nicola Pierazzo on 02/08/16.
//

#ifndef COMBINEDDENOISING_DCT_HPP
#define COMBINEDDENOISING_DCT_HPP

#include "nl-bayes/Image.hpp"

namespace imgutils {

void dct_inplace(Image &img);
void idct_inplace(Image &img);

}
#endif //COMBINEDDENOISING_DCT_HPP
