; test let
(let ((x 1)) (print x))
(let ((x 1) (y 2)) (print (+ x y)))
(let ((x 1)) (let ((y 2)) (print (+ x y))))
(let ((x 1)) (let ((x 2)) (print x)))
(print (let ((x 1)) x))
