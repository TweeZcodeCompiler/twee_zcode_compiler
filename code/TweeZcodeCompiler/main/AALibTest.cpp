//
// Created by Danni on 13.07.15.
//

#include "AALibTest.h"

#include <aalib.h>
#include <cstring>
#include <iostream>

using namespace std;

void AALibTest::test() {
    aa_context* context = aa_init(&mem_d, &aa_defparams, NULL);

    unsigned char* image = aa_image(context);
    char* text = reinterpret_cast<char*>aa_text(context);

    const struct aa_driver* mymemd = &mem_d;
    struct aa_hardware_params* myaadefparams = &aa_defparams;

    for(int x = 0; x < context->imgwidth; ++x) {
        for(int y = 0; y < context->imgheight; ++y) {
            aa_putpixel(context, x, y, (x*y)%255);
        }
    }

    aa_fastrender(context, 0, 0, aa_scrwidth(context), aa_scrheight(context));

    size_t len = strlen(text);


    for(int y = 0; y < context->params.height; ++y) {
        for(int x = 0; x < context->params.width; ++x) {
            //cout << context->params.width * y + x;
            cout << (char)text[context->params.width * y + x];
        }
        cout << endl;
    }

    aa_flush(context);

    aa_close(context);
}
