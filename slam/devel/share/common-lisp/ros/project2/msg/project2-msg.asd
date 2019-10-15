
(cl:in-package :asdf)

(defsystem "project2-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :geometry_msgs-msg
)
  :components ((:file "_package")
    (:file "CustomOdometry" :depends-on ("_package_CustomOdometry"))
    (:file "_package_CustomOdometry" :depends-on ("_package"))
  ))