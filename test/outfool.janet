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
  (def rope (outfool/rope ascii-lower))
  (is (= ascii-lower (string rope))))


(deftest get-positive-index-with-ascii-chars
  (def rope (outfool/rope ascii-lower))
  (is (= "a" (get rope 0))))


(deftest get-negative-index-with-ascii-chars
  (def rope (outfool/rope ascii-lower))
  (is (= "c" (get rope -1))))


(deftest get-positive-index-with-non-ascii-chars
  (def rope (outfool/rope utf8-lower))
  (is (= "\xC3\xA9" (get rope 3))))


(deftest get-negative-index-with-non-ascii-chars
  (def rope (outfool/rope utf8-lower))
  (is (= "\xC3\xA9" (get rope -1))))


(deftest iterate-ascii-chars
  (def rope (outfool/rope ascii-lower))
  (def buf @"")
  (with-dyns [:out buf]
    (each c rope (print c)))
  (is (deep= @"a\nb\nc\n" buf)))


(deftest iterate-non-ascii-chars
  (def rope (outfool/rope utf8-lower))
  (def buf @"")
  (with-dyns [:out buf]
    (each c rope (print c)))
  (is (deep= @"c\na\nf\n\xC3\xA9\n" buf)))


(deftest compare-equal-ropes
  (def rope1 (outfool/rope ascii-lower))
  (def rope2 (outfool/rope ascii-lower))
  (is (= rope1 rope2)))


(deftest compare-unequal-ropes
  (def rope1 (outfool/rope ascii-lower))
  (def rope2 (outfool/rope ascii-other))
  (is (not (= rope1 rope2))))


(deftest length-of-rope-of-ascii-chars
  (def rope (outfool/rope ascii-lower))
  (is (= 3 (length rope))))


(deftest length-of-rope-of-non-ascii-chars
  (def rope (outfool/rope utf8-lower))
  (is (= 4 (length rope))))


(run-tests!)
