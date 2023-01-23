# OM3D - Transparency

## Authors 

* Ihor Husak
* Benoist Andre

### How to build
Requirements: cmake 3.20 minimum, C++17, and OpenGL 4.5.
```bash
# At the project root
mkdir -p TP/build/debug
cd TP/build/debug
cmake ../..
make
```

In order to make materials transparent we had to add a new BlendMode and a new behavior to the Material class.

The new mode `Add` is defined in the enum class `BlendMode`.

Below you can find the behavior specific to materials for which this `BlendMode` has been set.

```c++
switch(_blend_mode) 
{
        // Other BlendModes
        ...
        // Transparency BlendMode
        case BlendMode::Add:
            glEnable(GL_BLEND);
            glDepthMask(GL_FALSE);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;
    }
```

If you run the code you will see the `forest.glb` scene with transparent trees and opaque stones.


In this version of the code we also handle the backface culling from the first Practical with the following code:
```c++
switch (_cull_mode) {
        case CullMode::None:
            glDisable(GL_CULL_FACE);
            break;

        case CullMode::Back:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
            break;

        case CullMode::Front:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            glFrontFace(GL_CCW);
            break;

        case CullMode::Both:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT_AND_BACK);
            glFrontFace(GL_CCW);
            break;
    }
```

### Contact
If you have a problem, please send a mail to :
- ihor.husak@epita.fr
- benoist.andre@epita.fr
