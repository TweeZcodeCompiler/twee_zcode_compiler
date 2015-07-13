//
// Created by Danni on 13.07.15.
//

#include "AALibTest.h"

#include <aalib.h>

void AALibTest::test() {
    aa_context* context = aa_init(&mem_d, &aa_defparams, NULL);

    const struct aa_driver* mymemd = &mem_d;
    struct aa_hardware_params* myaadefparams = &aa_defparams;

    aa_putpixel(context, 120, 120, 11 );

    aa_fastrender(context, 0, 0, aa_scrwidth(context), aa_scrheight(context));

    aa_flush(context);

    unsigned char* image = aa_image(context);

    aa_close(context);
}
