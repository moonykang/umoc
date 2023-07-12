# Welcome to Umoc engine

Umoc is a Vulkan renderer for practice.
It supports Windows & Mac at this time.

# Samples

| Quad | Normal mapping  | Basic PBR |
|---|---|---|
| ![quad](captures/quad.png) | ![normalmap](captures/normalmapping.png) | ![pbr](captures/pbr.png) |

| Skybox<br/>(Environment Refraction) | Bloom | IBL |
|---|---|---|
| ![skybox](captures/skybox.png) | ![bloom](captures/bloom.png) |   |

| Deferred shading | SSAO | Ray tracing |
|---|---|---|
| ![deferred](captures/deferred.png) |   |   |

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
