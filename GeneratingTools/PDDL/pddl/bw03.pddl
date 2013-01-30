;; satplan bw_large.b blocks

(define (problem eleven) 
   (:domain blocks_world)
   (:objects A B C D E F G H I J K)
   (:init (clear C)
          (on C B)
          (on B A)
          (on-table A)
          (clear K)
          (on K J)
          (on J E)
          (on E D)
          (on-table D)
          (clear I)
          (on I H)
	  (on H G)
	  (on G F)
  	  (on-table F))
   (:goal (and (on E J)
               (on A E)
               (on I D)
               (on H I)
               (on C K)
               (on K G)
               (on B C))))
