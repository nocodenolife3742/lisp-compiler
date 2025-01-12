; test defun
(defun test1 () (print (+ 1 2)))
(defun test2 (x) (print x))
(defun test3 (x y) (print (+ x y)))
(defun test4 (x) (let ((y 2)) (print (+ x y))))

(test1)
(test2 3)
(test3 4 5)
(test4 6)
