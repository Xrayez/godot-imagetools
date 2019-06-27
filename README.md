# Godot ImageTools

[![Build Status](https://travis-ci.com/Xrayez/godot-imagetools.svg?branch=master)](https://travis-ci.com/Xrayez/godot-imagetools)
[![Build status](https://ci.appveyor.com/api/projects/status/tbtra8e221si05bq/branch/master?svg=true)](https://ci.appveyor.com/project/Xrayez/godot-imagetools/branch/master)

This is a [Godot](https://github.com/godotengine/godot) module aimed to extend, optimize, and/or simplify various operations that can
be performed on an image. It provides and exposes the main `ImageTools` singleton for easy image processing. It's aimed to build upon Godot's built-in `Image` class to cater specific user needs.

The module also provides some more dedicated image classes, currently you'll find the following:

* `ImageIndexed` (see [description](classes/descriptions/ImageIndexed.md))
     * adds pseudo support for indexed images with a color palette.
     
## Contributing
     
See [Contributing](CONTRIBUTING.md) if you are interested in extending the module in any way.

## License and thirdparty libraries:
The module is under MIT license.

The module uses [exoquant](https://github.com/exoticorn/exoquant) library for image
quantization which is under MIT license, see [thirdparty/README.md](thirdparty/README.md)
