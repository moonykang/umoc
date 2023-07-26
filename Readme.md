# Welcome to Umoc engine

Umoc is a Vulkan renderer for practice.
It supports Windows & Mac at this time.

# Samples

## Basic
| Quad | Normal mapping  | skybox<br/>(Environment Refraction) |
|---|---|---|
| ![quad](captures/quad.png) | ![normalmap](captures/normalmapping.png) | ![skybox](captures/skybox.png) |

| Bloom | Deferred shading | SSAO |
|---|---|---|
| ![bloom](captures/bloom.png) | ![deferred](captures/deferred.png) | ![ssao](captures/ssao.png) |

## Physically Based Rendering
| PBR | IBL | TODO |
|---|---|---|
| ![pbr](captures/pbr.png) | ![ibl](captures/ibl.png) | ![todo](captures/todo.png) |


## Compute
| Sharpen | Emboss | Edge detection |
|---|---|---|
| ![pbr](captures/sharpen.png) | ![ibl](captures/emboss.png) | ![todo](captures/edgedetect.png) |

| Particles | N-body particles | TODO |
|---|---|---|
| ![pbr](captures/particle.png) | ![ibl](captures/nbody.png) | ![todo](captures/todo.png) |

## Git clone

```
git clone --recursive https://github.com/moonykang/umoc.git
git submodule init
git submodule update
```


## Development
### Code style

```
{ BasedOnStyle: MicroSoft, Language: Cpp, PointerAlignment: Left, IndentWidth: 4, ColumnLimit: 120 }
```
## Reference

https://learnopengl.com

https://github.com/SaschaWillems/Vulkan