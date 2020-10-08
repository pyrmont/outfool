(declare-project
  :name "Outfool"
  :description "A foolish attempt to provide better UTF-8 support in Janet"
  :author "Michael Camilleri"
  :license "MIT"
  :url "https://github.com/pyrmont/outfool"
  :repo "git+https://github.com/pyrmont/outfool"
  :dependencies ["https://github.com/pyrmont/testament"])


(def cflags
  (case (os/which)
    :windows ["/Iutf8.h"]
    ["-Iutf8.h" "-std=c99" "-Wall" "-Wextra" "-O3"]))


(def lflags
  [])


(declare-native
  :name "outfool"
  :cflags cflags
  :lflags lflags
  :headers ["src/rope.h"
            "src/outfool.h"]
  :source ["src/rope.c"
           "src/outfool.c"
           "src/outfool/functions.c"
           "src/outfool/type.c"])
