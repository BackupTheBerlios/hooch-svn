;;;
;;; $Id: addressbook.scm 153 2005-02-25 08:16:12Z sjamaan $
;;;
;;; addressbook.scm - Addressbook datastructure & manipulation functionality
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

(define-record-type addressbook
  (make-addressbook defaults contacts groups)
  addressbook?
  (defaults addressbook-defaults)		; Just settings
  (contacts addressbook-contacts)
  (groups   addressbook-groups))

(define (create-addressbook)
  (make-addressbook '() '() '()))

(define-record-type contact
  (make-contact name ids options structs)
  contact?
  (name     contact-name)
  (ids      contact-ids)
  (options  contact-options)
  (structs  contact-structs))

(define-record-type contact-id
  (make-contact-id name options structs)
  contact-id?
  (name    contact-id-name)
  (options contact-id-options)
  (structs contact-id-structs))

(define-record-type group
  (make-group name members options structs)
  group?
  (name    group-name)
  (members group-members)
  (options group-options)
  (structs group-structs))

(define-record-type struct
  (make-struct name structs options)
  struct?
  (name    struct-name)
  (structs struct-structs)
  (options struct-options))

(define defaults?    list?)

(define option?      pair?)
(define make-option  cons)
(define option-name  car)
(define option-value cdr)

(define (struct-add-detail detail struct)
  (cond
    ((not detail) struct)
    ((option? detail) (struct-add-option detail struct))
    ((struct? detail) (struct-add-struct detail struct))
    (else (error "Struct detail type not recognised" detail))))

(define (struct-add-option option struct)
  (make-struct (struct-name struct)
	       (alist-cons (option-name option)
			   (option-value option)
			   (struct-options struct))
	       (struct-structs struct)))

(define (struct-add-struct substruct struct)
  (make-struct (struct-name struct)
	       (struct-options struct)
	       (alist-cons (struct-name substruct)
			   substruct
			   (struct-structs struct))))

(define (defaults-add-entry entry defaults)
  (if (not entry)
      defaults
      (cons entry defaults)))

(define (contact-add-detail detail contact)
  (cond
    ((not detail) contact)
    ((contact-id? detail) (contact-add-id detail contact))
    ((option? detail) (contact-add-option detail contact))
    ((struct? detail) (contact-add-struct detail contact))
    (else (error "Contact detail type not recognised" detail))))

(define (contact-id-add-detail detail id)
  (cond
    ((not detail) id)
    ((option? detail) (contact-id-add-option detail id))
    ((struct? detail) (contact-id-add-struct detail id))
    (else (error "id detail type not recognised" detail))))

(define (group-add-members members group)
  (if (not members)
      group
      (make-group (append members (group-members group))
		  (group-options group)
		  (group-structs group))))

(define (addressbook-add-entry entry book)
  (cond
    ((not entry) book)
    ((group? entry) (addressbook-add-group entry book))
    ((contact? entry) (addressbook-add-contact entry book))
    ((defaults? entry) (addressbook-add-defaults entry book))
    (else (error "addressbook entry type not recognised"))))

(define (addressbook-add-defaults defaults book)
  (if (not defaults)
      book
      (make-addressbook (append defaults (addressbook-defaults book))
			(addressbook-contacts book)
			(addressbook-groups book))))

(define (addressbook-add-contact contact book)
  (if (not contact)
      book
      (make-addressbook (addressbook-defaults book)
			(alist-cons (contact-name contact)
				    contact
				    (addressbook-contacts book))
			(addressbook-groups book))))

(define (addressbook-add-group group book)
  (if (not group)
      book
      (make-addressbook (addressbook-defaults book)
			(addressbook-contacts book)
			(alist-cons (group-name group)
				    group
				    (addressbook-groups book)))))

(define (contact-add-id id contact)
  (if (not id)
      contact
      (make-contact (contact-name    contact)
		    (alist-cons (contact-id-name id)
				id
				(contact-ids     contact))
		    (contact-options contact)
		    (contact-structs contact))))

(define (contact-add-option option contact)
  (if (not option)
      contact
      (make-contact (contact-name    contact)
		    (contact-ids     contact)
		    (alist-cons (option-name  option)
				(option-value option)
				(contact-options contact))
		    (contact-structs contact))))

(define (contact-add-struct struct contact)
  (if (not struct)
      contact
      (make-contact (contact-name    contact)
		    (contact-ids     contact)
		    (contact-options contact)
		    (alist-cons (struct-name struct)
				struct
		    		(contact-structs contact)))))

(define (contact-id-add-struct struct id)
  (if (not struct)
      id
      (make-contact-id (contact-id-name id)
		       (contact-id-options id)
		       (alist-cons (struct-name struct)
				   struct
				   (contact-id-structs id)))))

(define (contact-id-add-option option id)
  (if (not option)
      id
      (make-contact-id (contact-id-name id)
		       (alist-cons (option-name option)
				   option
				   (contact-id-options id))
		       (contact-id-structs id))))

(define find-contact	assq)
(define find-group	assq)
(define find-contact-id	assq)
(define find-setting	assq)
