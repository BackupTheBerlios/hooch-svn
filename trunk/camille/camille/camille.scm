;;;
;;; $Id: camille.scm 153 2005-02-25 08:16:12Z sjamaan $
;;;
;;; Camille - an addressbook library
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

(define-structure utils (export ->string)
  (open scheme srfi-6)
  (files utils))

(define-structure addressbook
  (export addressbook? make-addressbook addressbook-contacts
          addressbook-groups addressbook-defaults
	  addressbook-add-entry addressbook-add-contact
	  addressbook-add-group addressbook-add-defaults
          contact? make-contact contact-name contact-ids contact-settings
	  group? make-group group-name group-settings
	  contact-id? make-contact-id contact-id-name contact-id-settings
	  struct? make-struct struct-name struct-settings struct-add-setting
	  struct-add-detail
	  option? make-option option-name option-value
	  contact-add-detail contact-id-add-detail
	  find-contact find-contact-id find-group find-setting
	  defaults-add-entry)
  (open scheme-with-scsh srfi-1 srfi-9 srfi-23)
  (files addressbook))

;; srfi-34/35 is in sunterlib, use scsh -lel exceptions/load.scm
(define-structure parser
  (export read-addressbook
	  &parse-error parse-error?
	  parse-error-continuation parse-error-message)
  (open srfi-28 scheme-with-scsh utils addressbook srfi-1 srfi-34 srfi-35)
  (files parser))
