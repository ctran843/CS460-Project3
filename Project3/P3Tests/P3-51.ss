(define (function1 V1 V2)
	(if (< V1 V2)
		(display (+ V1 V2))
	)
)

(define (main)
	(function1 3 5)
	(newline)
	(function1 13 5)
	(newline)
)

(main)
