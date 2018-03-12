{
  "targets": [
    {
      "target_name": "ration_calculator",
      "sources": [ 
        "nutrition_wrapper.cc",
        "NutritionModuleCppSrc/NutritionModule.cpp",
        "NutritionModuleCppSrc/Food.cpp",
        "NutritionModuleCppSrc/FoodTree.cpp"
      ],
      "xcode_settings": {
        "OTHER_CFLAGS" : [ "-std=c++14", "-stdlib=libc++" ],
        "OTHER_CPLUSPLUSFLAGS" : [ "-std=c++14", "-stdlib=libc++" ],
        "OTHER_LDFLAGS": [ "-stdlib=libc++" ],
        "CLANG_CXX_LIBRARY": "libc++",
        "GCC_ENABLE_CPP_RTTI": "YES",
        "GCC_ENABLE_CPP_EXCEPTIONS": "YES"
      }
    }
  ]
}