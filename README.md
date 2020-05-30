# ImageTools

[![Build Status](https://travis-ci.com/Xrayez/godot-imagetools.svg?branch=master-gd3)](https://travis-ci.com/Xrayez/godot-imagetools)
[![Build status](https://ci.appveyor.com/api/projects/status/tbtra8e221si05bq/branch/master-gd3?svg=true)](https://ci.appveyor.com/project/Xrayez/godot-imagetools/branch/master-gd3)

This is a [Godot](https://github.com/godotengine/godot) module aimed to extend,
optimize, and/or simplify various operations that can be performed on an image.
It provides and exposes the main `ImageTools` singleton for easy image
processing. It aims to build upon Godot's built-in `Image` class to cater
specific user needs.

### Compatibility
There are separate branches which are targeted for specific Godot Engine's major
version.

Regarding latest module version in development, switch to
* [`master-gd3`](https://github.com/Xrayez/godot-imagetools/tree/master-gd3) branch for Godot 3
* [`master-gd4`](https://github.com/Xrayez/godot-imagetools/tree/master-gd4) branch for Godot 4

Similarly, you may use any of the stable branches with a similar branch
encoding, such as `1.0-gd3` or `1.0-gd4`, if available.

### List of public methods:
| Method              | Short Description                                                           |
| ------------------- | --------------------------------------------------------------------------- |
| `replace_color`     | Replaces all occurrences of a given color with another one within an image. |
| `bucket_fill`       | Fills the area with a color confined by opaque pixels.                      |
| `resize_hqx`        | Expands the image using either HQ2X or HQ3X algorithms.                     |
| `has_pixel`         | Tells whether a pixel lies inside image coordinates.                        |
| `get_pixel_or_null` | Returns a pixel at specified image coordinates, or null if doesn't exist.   |

### List of classes:

| Class                                 | Short Description                                                             |
| ------------------------------------- | ----------------------------------------------------------------------------- |
| [`ImageIndexed`](doc/ImageIndexed.md) | Adds pseudo support for indexed images with a color palette.                  |
| `ImageBlender`                        | Provides advanced image blending methods (derived from OpenGL specification). |

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
     
See [Contributing](CONTRIBUTING.md) if you are interested in extending the
module in any way.

Any new features are encouraged to be developed against the latest Godot
Engine's *stable* version, which may be optionally ported to the latest engine's
*development* version.

## License:
The module is under MIT license.

For the full list of third-party libraries used by the module, see
[thirdparty/README.md](thirdparty/README.md).
