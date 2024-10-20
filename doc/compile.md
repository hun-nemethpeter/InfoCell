Compiling InfoCell

Dependencies are in Conan ( https://conan.io/ )

Ususally need a pip and conan upgrade before 3rd party update
  python.exe -m pip install --upgrade pip
  pip install conan --upgrade
Also sometimes you need a conan profile refresh
  conan profile detect

Project structure:
ARC indicate a root folder somewhere on disk

Example on Windows platform
ARC
  ├─ ARC (the original ARC (now renamed to ARC-AGI) repo from https://github.com/fchollet/ARC-AGI )
  ├─ arc-prize-2024 (the ARC Prize 2024 dataset files can be downloaded from https://www.kaggle.com/competitions/arc-prize-2024/data -> Download All)
  ├─ conan
  │   ├─ Debug
  │   ├─ Release
  │   └─ RelWithDebInfo
  └─ InfoCell
      └─ build
          ├─ Debug
          ├─ Release
          └─ RelWithDebInfo

On Windows:
  in "path\to\ARC\conan\Debug"
  conan install ..\..\InfoCell --settings=build_type=Debug --build=missing -of .

  in "path\to\ARC\conan\Release"
  conan install ..\..\InfoCell --settings=build_type=Release --build=missing -of .

  in "path\to\ARC\conan\RelWithDebInfo"
  conan install ..\..\InfoCell --settings=build_type=RelWithDebInfo --build=missing -of .

On Linux:
  in path/to/ARC/conan/Release
  conan install ../../InfoCell/ --settings=build_type=Release --build=missing -of .