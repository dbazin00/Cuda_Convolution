#include <iostream>
#include <cstdlib>
#include "lodepng.h"
#include <cuda.h>
#include <cuda_runtime.h>
#include "kernel.h"
#include <functional>
#include "timer.h"

int main() {
    // Timer
    Timer tmr;
    double t1, t2;

    // Velicina filtera
    const int filterSize = 5;

    // Imena datoteka slika
    const char* input_file = "lena.png";
    const char* blur_file = "blurFilter.png";


    std::vector<unsigned char> in_image;
    unsigned int width, height;

    // Pocetak timera
    t1 = tmr.elapsed();

    // Ucitavanje slike
    unsigned error = lodepng::decode(in_image, width, height, input_file);
    if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

    // Priprema podataka
    unsigned char* input_image = new unsigned char[(in_image.size() * 3) / 4];
    unsigned char* blur_image = new unsigned char[(in_image.size() * 3) / 4];
    int where = 0;
    for (int i = 0; i < in_image.size(); ++i) {
        if ((i + 1) % 4 != 0) {
            input_image[where] = in_image.at(i);
            blur_image[where] = 255;
            where++;
        }
    }

    // Primjena filtera na sliku
    blurFilter(input_image, blur_image, width, height, filterSize);

    // Priprema podataka za izlaznu sliku
    std::vector<unsigned char> out_blur_image;
    for (int i = 0; i < in_image.size(); ++i) {
        out_blur_image.push_back(blur_image[i]);
        if ((i + 1) % 3 == 0) {
            out_blur_image.push_back(255);
        }
    }

    // Spremanje zamagljene slike
    error = lodepng::encode(blur_file, out_blur_image, width, height);

    // Ako postoji greska, prikaze se
    if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;

    delete[] input_image;
    delete[] blur_image;

    // Kraj timera
    t2 = tmr.elapsed();

    std::cout << "Convolution ended in\t" << t2 - t1 << "\tms\n";

    return 0;

}



