(import testament :prefix "" :exit true)
(import ../build/outfool :as outfool)


(def ascii-upper "ABC")
(def ascii-lower "abc")
(def ascii-other "abcd")

(def utf8-upper "CAFÉ")
(def utf8-lower "café")
(def utf8-other "cafés")


(deftest casecmp-with-equal-ascii-strings
  (is (= 0 (outfool/casecmp ascii-upper ascii-lower))))


(deftest casecmp-with-unequal-ascii-strings
  (is (< (outfool/casecmp ascii-upper ascii-other) 0)))


(deftest casecmp-with-equal-utf8-strings
  (is (= 0 (outfool/casecmp utf8-upper utf8-lower))))


(deftest casecmp-with-unequal-utf8-strings
  (is (< (outfool/casecmp utf8-upper utf8-other) 0)))


(deftest convert-rope-to-string
  (def rope (outfool/rope "abc"))
  (def str "abc")
  (is (= str (string rope))))


(deftest iterate-ascii-chars
  (def rope (outfool/rope "hello world"))
  (def buf @"")
  (with-dyns [:out buf]
    (each c rope (print c)))
  (is (deep= @"h\ne\nl\nl\no\n \nw\no\nr\nl\nd\n" buf)))


(deftest iterate-non-ascii-chars
  (def rope (outfool/rope "hello 世界"))
  (def buf @"")
  (with-dyns [:out buf]
    (each c rope (print c)))
  (is (deep= @"h\ne\nl\nl\no\n \n\xE4\xB8\x96\n\xE7\x95\x8C\n" buf)))


(deftest compare-equal-ropes
  (def rope1 (outfool/rope "abc"))
  (def rope2 (outfool/rope "abc"))
  (is (= rope1 rope2)))


(deftest compare-unequal-ropes
  (def rope1 (outfool/rope "abc"))
  (def rope2 (outfool/rope "def"))
  (is (not (= rope1 rope2))))


(run-tests!)
