# Welcome to Umoc engine

Umoc is a Vulkan renderer for practice.
It supports Windows & Mac at this time.

# Samples

| Quad | Normal mapping  | skybox<br/>(Environment Refraction) |
|---|---|---|
| ![quad](captures/quad.png) | ![normalmap](captures/normalmapping.png) | ![skybox](captures/skybox.png) |

| Bloom | Deferred shading | PBR |
|---|---|---|
| ![bloom](captures/bloom.png) | ![deferred](captures/deferred.png) | ![pbr](captures/pbr.png) |

| SSAO | IBL | Ray tracing |
|---|---|---|
|  |   |   |

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
