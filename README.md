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

### Contact
If you have a problem, please send a mail to :
- ihor.husak@epita.fr
- benoist.andre@epita.fr
