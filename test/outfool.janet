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


(run-tests!)
