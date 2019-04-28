## Godot ImageTools

[![Build Status](https://travis-ci.com/Xrayez/godot-imagetools.svg?branch=master)](https://travis-ci.com/Xrayez/godot-imagetools)
[![Build status](https://ci.appveyor.com/api/projects/status/tbtra8e221si05bq/branch/master?svg=true)](https://ci.appveyor.com/project/Xrayez/godot-imagetools/branch/master)

This is a Godot module aimed to simplify and optimize various operations that can
be performed on an image. Currently you'll find the following classes for image
manipulation:

* `ImageIndexed` (see [description](classes/descriptions/ImageIndexed.md))
     * adds pseudo support for indexed images with a color palette.

### License and thirdparty libraries:
The module is under MIT license.

The module uses [exoquant](https://github.com/exoticorn/exoquant) library for image
quantization which is under MIT license, see [thirdparty/README.md](thirdparty/README.md)
