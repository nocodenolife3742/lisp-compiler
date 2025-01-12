; list length
(defun len (lst)
  (if (null lst)
      0
      (+ 1 (len (cdr lst)))))

(print (len '(1 2 3 4 5)))
(print (len nil))
(print (len '(1 (2 9 (8 6)) 4)))

