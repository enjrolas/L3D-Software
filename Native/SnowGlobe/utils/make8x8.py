#!/usr/bin/python

import sys
from PIL import Image
from os.path import basename, splitext

if not len(sys.argv) >= 2:
    print "usage: make8x8 <image> <image> ..."
    sys.exit(1)

for i in range(1, len(sys.argv)):
    filename = sys.argv[i]
    image = Image.open(filename)
    pixels = image.load()

    imageArray = "uint8_t image_{0}[192] = {{\n".format(splitext(basename(filename))[0])

    if image.size == (8, 8):
        for y in range(0, 8):
            imageArray += "    "

            for x in range(0, 8):
                for chan in pixels[x, y]:
                    imageArray += "{0:#0{1}x}".format(chan, 4) + ", "
                if x == 3:
                    imageArray += "\n    "
            imageArray += "\n"

    imageArray += "};\n\n"
    sys.stdout.write(imageArray)
