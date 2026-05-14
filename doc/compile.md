Compiling InfoCell
==================

Dependencies
------------

Download cmake ( https://cmake.org/ )

Download python ( https://www.python.org/ )

Dependencies are handled with Conan ( https://conan.io/ )

Ususally need a pip and conan upgrade before 3rd party update
  - python.exe -m pip install --upgrade pip
  - pip install conan --upgrade

Also sometimes you need a conan profile refresh
  - conan profile detect

Project structure
-----------------

ARC indicate a root folder somewhere on the disk. Inside that folder:

- Checkout the InfoCell repo from https://github.com/hun-nemethpeter/InfoCell
- Checkout the original ARC-AGI repo from https://github.com/fchollet/ARC-AGI
- Download the ARC Prize 2024 dataset files from https://www.kaggle.com/competitions/arc-prize-2024/data -> Download All (optional)


```
ARC
  ├─ ARC-AGI
  ├─ arc-prize-2024
  └─ InfoCell
      ├─ build
      │   ├─ Debug
      │   ├─ Release
      │   └─ RelWithDebInfo
      ├─ conan
      │   ├─ Debug
      │   ├─ Release
      │   └─ RelWithDebInfo
```

The `build` and `conan` folders will be created automatically during the conan setup and the build process.

Conan Setup
-----------

Download and setup 3rd party libraries with Conan:

On Windows:

  - in "path\to\ARC\InfoCell"
  - `conan install . --settings=build_type=Debug --build=missing -of conan\Debug`
  - `conan install . --settings=build_type=Release --build=missing -of conan\Release`
  - `conan install . --settings=build_type=RelWithDebInfo --build=missing -of conan\RelWithDebInfo`

On Linux:

  - in path/to/ARC/InfoCell
  - `conan install . --settings=build_type=Release --build=missing -of conan/Debug`
  - `conan install . --settings=build_type=Release --build=missing -of conan/Release`
