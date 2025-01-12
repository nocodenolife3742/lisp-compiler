; test list
(print (list 1 2 3))
(print (list 1 "two" 0.123456 '(1 2 3) ''(1 2 3) 'symbol '(1 (2 3) 4 (5 '(s 7))) nil t))
(print (cons 1 (cons 2 (cons 3 nil))))
(print (cons 1 ()))
(print (car (cons 1 ())))
(print (cdr '(2 3 (4 4.456789) 5 6)))