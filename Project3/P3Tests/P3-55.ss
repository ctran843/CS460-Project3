(define (function1 V1 V2)
	(cond 
		((< V1 V2) (+ V1 V2))
		((> V1 V2) (- V1 V2))
		((= V1 V2) (* V1 V2))
	)
)

(define (main)
	(display (function1 3 5))
	(newline)
	(display (function1 13 5))
	(newline)
)

(main)
