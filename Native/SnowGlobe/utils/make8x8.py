#!/usr/bin/python

import sys
from PIL import Image

if not len(sys.argv) == 2:
    print "usage: make8x8 <image>"
    sys.exit(1)

filename = sys.argv[1]

image = Image.open(filename)
pixels = image.load()

imageArray = "uint8_t image[192] = {\n"

if image.size == (8, 8):
    for y in range(0, 8):
        imageArray += "\t"

        for x in range(0, 8):
            for chan in pixels[x, y]:
                imageArray += "{0:#0{1}x}".format(chan, 4) + ", "
            if x == 3:
                imageArray += "\n\t"
        imageArray += "\n"

imageArray += "};"

sys.stdout.write(imageArray)
