; factorial
(defun factorial (n)
  (if (<= n 1)
      1
      (* n (factorial (- n 1)))))
(print (factorial 5))
(print (factorial 10))