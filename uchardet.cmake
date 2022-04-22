include(FetchContent)

FetchContent_Declare(
  uchardet
  URL https://www.freedesktop.org/software/uchardet/releases/uchardet-0.0.7.tar.xz
  URL_HASH SHA256=3fc79408ae1d84b406922fa9319ce005631c95ca0f34b205fad867e8b30e45b1
)

set(BUILD_BINARY OFF)
set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC ON)

FetchContent_MakeAvailable(uchardet)
