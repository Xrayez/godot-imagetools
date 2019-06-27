# Contributing

Thanks for taking interest in contributing!

If you think that the module is missing some useful image processing utility methods, you can open an issue describing your use case or discuss the ways of implementing such methods if you want to do it yourself. 

In general, most (if not any) contribution to the `ImageTools` class is welcomed. Here are some examples:

- image filtering
- image transformations (rotation)
- pattern generation
- pattern recognition

Some specific methods like:

- `bucket_fill(x, y, with_color)`
- `draw_line(from, to, color)`

---

## Where do I start?

Any utility methods must be implemented as static methods in:

* [image_tools.h](image_tools.h)
* [image_tools.cpp](image_tools.cpp) 

and later exposed to scripting in:

* [bind/image_tools_bind.h](bind/image_tools_bind.h)
* [bind/image_tools_bind.cpp](bind/image_tools_bind.cpp)

You can leave the binding part and let other contributors finish your work. In that's the case, the method will be available within the engine only.

Creating a new class by extending the `Image` class is acceptable if you feel like the complexity of added feature(s) deserves a separate implementation. For instance, see [classes/image_indexed.h](classes/image_indexed.h).

---

## Thirdparty

Binding to thirdparty libraries is acceptable but they must be compatible with MIT license.
