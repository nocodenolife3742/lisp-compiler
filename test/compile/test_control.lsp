; test control structures
(progn (print 1) (print 2) 3)
(progn (print 1) (print 2) (print 3))
(if t (print 1) (print 2))
(if nil (print 1) (print 2))
(if (<= 1 2) (print 1) (print 2))
(if (<= 2 1) (print 1) (print 2))