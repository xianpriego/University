

open Bin_tree;;

let rec insert_tree ord x = function
    Empty -> Node(x, Empty, Empty)
  | Node(root, left, right) -> if ord x root then  Node(root, insert_tree ord x left, right)
                               else Node(root, left, insert_tree ord x right);;
            

let tsort ord l =
  inorder (List.fold_left (fun a x -> insert_tree ord x a) Empty l)
;;

