(define (function1 V1 V2)
	(display (+ V1 V2))
	(newline)
	(display (+ 4.1 2.5))
	(newline)
)

(define (function2 V1 V2)
	(display (- V1 V2))
	(newline)
	(display (- V2 V1))
	(newline)
)

(define (function3 V1 V2)
	(display (* V1 V2))
	(newline)
	(display (* V2 V1))
	(newline)
)

(define (function4 V1 V2)
	(display (/ V1 V2))
	(newline)
	(display (/ V2 V1))
	(newline)
)

(define (function5 V1 V2)
	(display (modulo V1 V2))
	(newline)
	(display (modulo V2 V1))
	(newline)
)

(define (main)
	(function1 6 3)
	(function2 6 3)
	(function3 6 3)
	(function4 6 3)
	(function5 6 3)
)

(main)