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

(define-record-type contact
  (make-contact name ids settings)
  contact?
  (name     contact-name)
  (ids      contact-ids)
  (settings contact-settings))

(define-record-type contact-id
  (make-contact-id name settings)
  contact-id?
  (name     contact-id-name)
  (settings contact-id-settings))

(define-record-type group
  (make-group name members settings)
  group?
  (name     group-name)
  (members  group-members)
  (settings group-settings))

(define-record-type struct
  (make-struct name settings)
  struct?
  (name     struct-name)
  (settings struct-settings))

;;
;; This could be implemented a bit simpler, but for consistency we're using
;; a record type anyway.
;;
(define-record-type option
  (make-option name value)
  option?
  (name  option-name)
  (value option-value))

;;
;; We could use inheritance to make this a bit more elegant.  But that would
;; mean using a complicated OO system or something.  I prefer this.
;;
(define (setting-name setting)
  (if (option? setting)
      (option-name setting)
      (struct-name setting)))

(define (setting? thing)
  (or (option? thing) (struct? thing)))

(define defaults?    list?)
(define settings?    list?)

(define (struct-add-detail detail struct)
  (cond
    ((not detail) struct)
    ((setting? detail) (struct-add-setting detail struct))
    (else (error "Struct detail type not recognised" detail))))

(define (struct-add-setting setting struct)
  (make-struct (struct-name struct)
	       (alist-cons (setting-name setting)
			   setting
			   (struct-settings struct))))

(define (defaults-add-entry entry defaults)
  (if (not entry)
      defaults
      (cons entry defaults)))

(define (contact-add-detail detail contact)
  (cond
    ((not detail) contact)
    ((contact-id? detail) (contact-add-id detail contact))
    ((setting? detail)	  (contact-add-setting detail contact))
    (else (error "Contact detail type not recognised" detail))))

(define (contact-id-add-detail detail id)
  (cond
    ((not detail) id)
    ((setting? detail) (contact-id-add-setting detail id))
    (else (error "id detail type not recognised" detail))))

(define (group-add-members members group)
  (if (not members)
      group
      (make-group (append members (group-members group))
		  (group-settings group))))

(define (addressbook-add-entry entry book)
  (cond
    ((not entry) book)
    ((group? entry)	(addressbook-add-group entry book))
    ((contact? entry)	(addressbook-add-contact entry book))
    ((defaults? entry)	(addressbook-add-defaults entry book))
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
		    (contact-settings contact))))

(define (contact-add-setting setting contact)
  (if (not setting)
      contact
      (make-contact (contact-name    contact)
		    (contact-ids     contact)
		    (alist-cons (setting-name  setting)
				setting
				(contact-settings contact)))))

(define (contact-id-add-setting setting id)
  (if (not setting)
      id
      (make-contact-id (contact-id-name id)
		       (alist-cons (setting-name setting)
				   setting
				   (contact-id-settings id)))))

;; XXX These need to be looked at
(define find-contact	assq)
(define find-group	assq)
(define find-contact-id	assq)
(define find-setting	assq)
