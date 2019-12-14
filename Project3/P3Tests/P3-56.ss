(define (sum L)
	(if (null? L)
		0
		(+ (car L) (sum (cdr L)))
	)
)

(define (main)
	(display (sum '(3 5)))
	(newline)
	(display (sum '(13 3 4 7 12 5)))
	(newline)
)

(main)
