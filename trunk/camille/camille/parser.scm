;;;
;;; $Id$
;;;
;;; parser.scm - Addressbook file parsing
;;;
;;; Copyright (c) 2005 Peter Bex and Vincent Driessen
;;; All rights reserved.
;;;
;;; Redistribution and use in source and binary forms, with or without
;;; modification, are permitted provided that the following conditions
;;; are met:
;;; 1. Redistributions of source code must retain the above copyright
;;;    notice, this list of conditions and the following disclaimer.
;;; 2. Redistributions in binary form must reproduce the above copyright
;;;    notice, this list of conditions and the following disclaimer in the
;;;    documentation and/or other materials provided with the distribution.
;;; 3. Neither the names of Peter Bex or Vincent Driessen nor the names of any
;;;    contributors may be used to endorse or promote products derived
;;;    from this software without specific prior written permission.
;;;
;;; THIS SOFTWARE IS PROVIDED BY PETER BEX AND VINCENT DRIESSEN AND CONTRIBUTORS
;;; ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
;;; TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
;;; PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
;;; BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
;;; CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
;;; SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;;; INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
;;; CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
;;; ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
;;; POSSIBILITY OF SUCH DAMAGE.

;;
;; This definition allows the error handler to continue the reading of the
;; configuration after the offending s-expression.
;; Just invoke the continuation with #f as argument, the addressbook
;; functions ignore #f entries.
;;
(define-condition-type &parse-error &error
  parse-error?
  (continuation parse-error-continuation)
  (message parse-error-message))

;; First argument must be what is returned if the parsing failed
(define-syntax parser
  (syntax-rules ()
    ((parser (?name ?arg1 ?arg2 ...) ?error-func
       ?body ...)
     (define (?name ?arg1 ?arg2 ...)
       (call-with-current-continuation
	 (lambda (cont)
	   (let ((?error-func
		   (lambda (errstr sym)
		     (raise (make-condition &parse-error
					    'continuation cont
					    'message (string-append
							errstr
							" "
							(->string sym)))))))
	     ?body ...)))))))

;;
;; <addressbook>       -> <addressbook-entry>*
;; <addressbook-entry> -> ;; See parse-addressbook-entry
;;
;; The port argument is optional, just like READ's is.
;;
;; Returns: An addressbook structure
;;
(define (read-addressbook . port)
  (let next ((book (create-addressbook)))
    (let ((datum (apply read port)))
      (if (eof-object? datum)
	  book
	  (next (addressbook-add-entry
	  	  (parse-addressbook-entry book datum)
		  book))))))

;;
;; <addressbook-entry> -> (contact <contact>)
;;			  | (group <group>)
;;			  | (defaults <defaults>)
;; <defaults>	       -> ;; See parse-defaults
;; <contact>	       -> ;; See parse-contact
;; <group>	       -> ;; See parse-group
;;
(parser (parse-addressbook-entry book entry) parse-error
  (if (not (pair? entry))
      (parse-error "Addressbook entry is not a pair:" entry))
      (let ((type (car entry))
	    (contents (cdr entry)))
	(case type
	  ((contact)  (parse-contact  contents))
	  ((group)    (parse-group    contents))
	  ((defaults) (parse-defaults contents))
	  (else (parse-error "Unknown addresbook entry type:" type)))))

;;
;; <contact>	    -> (<name> <contact-detail>*)
;; <name>	    -> <identifier>
;; <contact-detail> -> ;; See parse-contact-detail
;;
;; Returns: A contact structure
;;
(parser (parse-contact contact) parse-error
  (if (not (pair? contact))
      (parse-error "Contact is not a pair:" contact)
      (let ((name (car contact))
	    (details (cdr contact)))
	(cond
	  ((not (list? details))
	    (parse-error "Expected details for contact" name))
	  ((not (symbol? name))
	    (parse-error "Expected name for contact" name))
	  (else
	    (let next ((details details)
		       (contact (make-contact name '() '() '())))
	      (if (null? details)
		  contact
		  (let ((detail (car details))
			(rest   (cdr details)))
		    (next rest
			  (contact-add-detail
			    (parse-contact-detail contact detail)
			    contact))))))))))

;;
;; <contact-detail> -> (id <contact-id>)
;;			| (option <option>)
;;			| (struct <struct>)
;; <contact-id>	    -> ;; See parse-contact-id
;; <option>	    -> ;; See parse-option
;; <struct>	    -> ;; See parse-struct
;;
(parser (parse-contact-detail contact detail) parse-error
  (if (not (pair? detail))
      (parse-error "Contact detail is not a pair" detail)
      (let ((type (car detail))
	    (contents (cdr detail)))
	(case type
	  ((id) (parse-contact-id contact contents))
	  ((option) (parse-option contact contents))
	  ((struct) (parse-struct contact contents))
	  (else (parse-error "Unknown contact detail type:" type))))))

;;
;; <contact-id>		-> (<name> <contact-id-detail>*)
;; <name>		-> <identifier>
;; <contact-id-detail>	-> ;; See parse-contact-id-detail
;;
;; Returns: An id structure
;;
(parser (parse-contact-id contact id) parse-error
  (if (not (pair? id))
      (parse-error "Id not a pair for contact" (contact-name contact))
      (let ((name (car id))
	    (details (cdr id)))
	(cond
	  ((not (list? details)) (parse-error "Expected details for id" name))
	  ((not (symbol? name))  (parse-error "Expected name for id" name))
	  (else
	    (let next ((details details)
		       (id (make-contact-id name '() '())))
	      (if (null? details)
		  id
		  (let ((detail (car details))
			(rest   (cdr details)))
		    (next rest
			  (contact-id-add-detail
			    (parse-contact-id-detail id detail) id))))))))))

;;
;; <setting> -> (option <option>) | (struct <struct>)
;; <option>  -> ;; See parse-option
;; <struct>  -> ;; See parse-struct
;;
(parser (parse-setting parent setting) parse-error
  (if (not (pair? setting))
      (parse-error "Setting is not a pair:" setting)
      (let ((type (car setting))
	    (contents (cdr setting)))
	(case type
	  ((option) (parse-option parent contents))
	  ((struct) (parse-struct parent contents))
	  (else (parse-error "Unknown setting detail type:" type))))))

;;
;; <contact-id-detail>	-> <setting>
;; <setting>		-> ;; See parse-setting
;;
(define parse-contact-id-detail parse-setting)

;;
;; <group>	  -> (group <name> (<member>*) <group-detail>*)
;; <name>	  -> <identifier>
;; <member>	  -> <identifier>
;; <group-detail> -> ;; See parse-group-detail
;;
;; Returns: A group structure
;;
(parser (parse-group group) parse-error
  (if (not (pair? group))
      (parse-error "Expected identifier for group" group)
      (let ((name (car group))
	    (body (cdr group)))
	(cond
	  ((not (symbol? name))
	    (parse-error "Expected name for group" name))
	  ((not (list? body))
	    (parse-error "Expected list of member identifiers for group" name))
	  (else
	    (letrec ((members (car body))
		     (details (cdr body))
		     (lsym? (lambda (l)
			      (or (null? l)
				  (and (symbol? (car l))
				       (lsym? (cdr l)))))))
	      (cond
		((not (lsym? members)) (parse-error "Expected member names"
						    name))
		(else (let next ((details details)
				 (group (make-group name members '() '())))
			(if (null? details)
			    group
			    (let ((detail (car details))
				  (rest   (cdr details)))
			      (next
				rest
				(parse-group-detail group detail)))))))))))))

;;
;; <option> -> (<name> <value>)
;; <name>   -> <identifier>
;; <value>  -> <s-expr>
;;
(parser (parse-option parent option) parse-error
  (if (not (pair? option))
      (parse-error "Option is not a pair:" option)
      (let ((name  (car option))
	    (value-list (cdr option)))
	(cond
	  ((not (symbol? name))
	    (parse-error "Option name is not a symbol:" name))
	  ((not (= (length value-list) 1))
	    (parse-error "Expected only one value for option" name))
	  (else (make-option name (car value-list)))))))

;;
;; <struct>	   -> (name <struct-detail>*)
;; <struct-detail> -> ;; See parse-struct-detail
;;
(parser (parse-struct parent struct) parse-error
  (if (not (pair? struct))
      (parse-error "Struct is not a pair:" struct)
      (let ((name (car struct))
	    (details (cdr struct)))
	(let next ((details details)
		   (struct (make-struct name '() '())))
	  (if (null? details)
	      struct
	      (next (cdr details)
		    (struct-add-detail
		      (parse-struct-detail struct (car details))
		      struct)))))))

;;
;; <struct-detail> -> <setting>
;; <setting>	   -> ;; See parse-setting
;;
(define parse-struct-detail parse-setting)

;;
;; <group-detail> -> <setting>
;; <setting>	  -> ;; See parse-setting
(define parse-group-detail parse-setting)

;;
;; <defaults>	     -> (<defaults-detail>*)
;; <defaults-detail> -> ;; See parse-defaults-detail
;;
(parser (parse-defaults defs) parse-error
  (if (not (list? defs))
      (parse-error "Defaults is not a list:" defs)
      (let next ((details defs)
		 (defaults '()))
	(if (null? details)
	    defaults
	    (let ((detail (car details))
		  (rest   (cdr details)))
	      (next rest
		    (defaults-add-entry (parse-defaults-detail defaults detail)
		    defaults)))))))

;;
;; <defaults-detail> -> <setting>
;; <setting>	     -> ;; See parse-setting
;;
(define parse-defaults-detail parse-setting)
