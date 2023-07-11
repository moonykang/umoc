# Welcome to Umoc engine

Umoc is a Vulkan renderer for practice.
It supports Windows & Mac at this time.

# Samples

| Quad | Normal mapping  | Basic PBR |
|---|---|---|
| ![quad](https://github.com/moonykang/umoc/blob/main/engine/ppm/quad.png) | ![normalmap](https://github.com/moonykang/umoc/blob/main/engine/ppm/normalmapping.png) | ![pbr](https://github.com/moonykang/umoc/blob/main/engine/ppm/pbr.png) |

| Skybox (Environment Refraction) | Bloom | IBL |
|---|---|---|
| ![skybox](https://github.com/moonykang/umoc/blob/main/engine/ppm/skybox.png) | ![bloom](https://github.com/moonykang/umoc/blob/main/engine/ppm/bloom.png) |   |

| Deferred shading | SSAO | Ray tracing |
|---|---|---|
|   |   |   |

## Git clone

```
git clone --recursive https://github.com/Umocmin/Umoc.git
git submodule init
git submodule update
```


## Development
### Code style

```
{ BasedOnStyle: MicroSoft, Language: Cpp, PointerAlignment: Left, IndentWidth: 4, ColumnLimit: 120 }
```
