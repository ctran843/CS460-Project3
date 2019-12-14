(define (main)
	(display (append (cdr '(1 2 3)) (cdr '(4 5 6))))
	(newline)
	(display (cons (car '(1 2 3)) (cdr '(4 5 6))))
	(newline)
)

(main)
