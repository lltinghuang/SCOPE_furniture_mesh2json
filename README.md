# How to start

## requirement
If you use Visual Studio, then you can install the required package by vcpkg

And check Project>>Property>>Configuration Properties>>vcpkg>>Use Vcpkg Manifest set to "Yes"

```
vcpkg --new application
vcpkg add port cgal
vcpkg add port nlohmann-json
vcpkg add port eigen3
```
reference: [C++ Dependencies in Visual Studio with vcpkg](https://youtu.be/3X4DmBfjy0Y?si=fWz5UoCVFIfJWIQw)

## step
1. Run transform.py (convert .obj to .off)
2. Run ScopeLab1.cpp (generate the JSON for edges)
3. Run visualization.py (generate the result from single/multiple views)
