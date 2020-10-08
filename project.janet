(declare-project
  :name "Outfool"
  :description "Janet bindings for utf8.h"
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
  :headers ["src/outfool.h"]
  :source ["src/outfool.c"])
