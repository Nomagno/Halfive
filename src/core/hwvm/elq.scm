;;; Implementation of the Eloquent 
;;;     environment in R5RS scheme
;;; Copyright Nomagno 2022
;;; SPDX Identifier: COIL-1.0

(define private_mem (make-vector (* 256 256) 0))

(define private_boolint (lambda (x) (if x 1 0)))
(define private_intbool (lambda (x) (if (eq? x 0) #f #t)))
(define mu (lambda (x) (modulo x 256)))
(define private_unsigned? (lambda (x) (private_boolint (and (integer? x) (>= x 0)))))
(define private_throw (lambda () -1))

(define elq_cons (lambda (x y) (cons x y)))
(define elq_car (lambda (z) (car z)))
(define elq_cdr (lambda (z) (cdr z)))

(define elq_nil? (lambda (x) (private_boolint (null? x))))
(define elq_procedure? (lambda (x) (private_boolint (procedure? x))))

(define elq_atom? (lambda (x) (private_boolint (or
        (private_intbool (elq_procedure? x))
        (private_intbool (private_unsigned? x))
        (private_intbool (elq_nil? x))))))

(define elq_eq? (lambda (x y) (private_boolint (and
        (private_intbool (elq_atom? x))
        (private_intbool (elq_atom? y))
        (eq? x y)))))

(define elq_add (lambda (x y)
        (if (and (private_intbool (private_unsigned? x)) (private_intbool (private_unsigned? y)))
                (cons (mu (+ x y)) (intbool (modulo (+ x y) 256)))
                (private_throw))))

(define elq_sub (lambda (x y)
        (if (and (private_intbool (private_unsigned? x)) (private_intbool (private_unsigned? y)))
                (cons (mu (- x y)) (intbool (modulo (- x y) 256)))
                (private_throw))))

(define elq_and (lambda (x y)
        (if (and (private_intbool (private_unsigned? x)) (private_intbool (private_unsigned? y)))
                (cons (logand (mu x) (mu y)) 0)
                (private_throw))))

(define elq_or (lambda (x y)
        (if (and (private_intbool (private_unsigned? x)) (private_intbool (private_unsigned? y)))
                (cons (logior (mu x) (mu y)) 0)
                (private_throw))))

(define elq_xor (lambda (x y)
        (if (and (private_intbool (private_unsigned? x)) (private_intbool (private_unsigned? y)))
                (cons (logxor (mu x) (mu y)) 0)
                (private_throw))))

(define elq_shift (lambda (x y)
        (if (and (private_intbool (private_unsigned? x)) (private_intbool (private_unsigned? y)))
                (cons (if (< (mu y) 8)
                        (ash (mu x) (mu y))
                        (ash (mu x) (mu (- (- y 8))))) 0)
                (private_throw))))

(define elq_set (lambda (x y)
        (if (private_intbool (elq_atom? x))
                (private_throw)
                (if (private_intbool (elq_atom? y))
                (begin (vector-set! private_mem (+ (* (car x) 16) (cdr x)) (mu y)) (mu y))
                (begin (vector-set! private_mem (+ (* (car x) 16) (cdr x)) (mu (vector-ref private_mem (+ (* (car y) 16) (cdr y)))))
                        (mu (vector-ref private_mem (+ (* (car y) 16) (cdr y)))))))))
(define elq_halt (lambda () (private_throw)))
