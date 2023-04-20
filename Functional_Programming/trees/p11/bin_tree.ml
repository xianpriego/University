
   
type 'a bin_tree =
    Empty
  | Node of 'a * 'a bin_tree * 'a bin_tree
;;


let rec map_tree f = function
   Empty -> Empty
   |Node (x,l,r) -> Node (f x, map_tree f l, map_tree f r)
;;   


let rec fold_tree f a = function
    Empty -> a
  | Node (x, l, r) -> f x (fold_tree f a l) (fold_tree f a r)
;;

let sum t = fold_tree (fun x y z -> x + y + z) 0 t;;

let prod t = fold_tree (fun x y z -> x *. y *. z) 1. t;;


let size t = fold_tree (fun x y z -> 1 + y + z ) 0 t;;

let height t = fold_tree (fun x y z -> 1 + max y z ) 0 t;;

let inorder t = fold_tree (fun root l r -> l @ [root] @ r) [] t ;;

let mirror t = fold_tree (fun root l r -> Node(root, r, l)) Empty t;;


(*******************************************************************************)

