# Godot ImageTools

[![Build Status](https://travis-ci.com/Xrayez/godot-imagetools.svg?branch=master-gd3)](https://travis-ci.com/Xrayez/godot-imagetools)
[![Build status](https://ci.appveyor.com/api/projects/status/tbtra8e221si05bq/branch/master-gd3?svg=true)](https://ci.appveyor.com/project/Xrayez/godot-imagetools/branch/master-gd3)

This is a [Godot](https://github.com/godotengine/godot) module aimed to extend, optimize, and/or simplify various operations that can
be performed on an image. It provides and exposes the main `ImageTools` singleton for easy image processing. It's aimed to build upon Godot's built-in `Image` class to cater specific user needs.

### List of public methods:
| Method              | Short Description                                                           |
|---------------------|-----------------------------------------------------------------------------|
| `replace_color`     | Replaces all occurrences of a given color with another one within an image. |
| `bucket_fill`       | Fills the area with a color confined by opaque pixels.                      |
| `has_pixel`         | Tells whether a pixel lies inside image coordinates.                        |
| `get_pixel_or_null` | Returns a pixel at specified image coordinates, or null if doesn't exist.   |

### List of classes:

| Class        | Short Description                                                                                          |
|----------------|----------------------------------------------------------------------------------------------------|
| [ImageIndexed](classes/descriptions/ImageIndexed.md) | Adds pseudo support for indexed images with a color palette. |

## Installation

Before installing, you must be able to 
[compile Godot Engine](https://docs.godotengine.org/en/latest/development/compiling/) 
from source.

```bash
# Change directory to `modules` subfolder of Godot repository
cd godot/modules/
# Clone the module under directory named `imagetools`
git clone https://github.com/Xrayez/godot-imagetools.git imagetools && cd ..
# Compile the engine manually, for instance:
scons platform=windows target=release_debug bits=64
```
     
## Contributing
     
See [Contributing](CONTRIBUTING.md) if you are interested in extending the module in any way.

## License and thirdparty libraries:
The module is under MIT license.

The module uses [exoquant](https://github.com/exoticorn/exoquant) library for image
quantization which is under MIT license, see [thirdparty/README.md](thirdparty/README.md)
