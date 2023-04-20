(*Xian Priego Martin
   39457432N
   Grupo 4.3*)

type 'a g_tree =
  Gt of 'a * 'a g_tree list
;;

let rec size = function 
    Gt (_, []) -> 1
  | Gt (r, h::t) -> size h + size (Gt (r, t))
;;

(* devuelve las hojas de un g_tree, "de izquierda a derecha" *)
    
let rec leaves = function 
    Gt (r, []) -> [r]
  | Gt (r, h::[]) -> leaves h
  | Gt (r, h::t) -> leaves h @ leaves (Gt (r, t))

(* devuelve la "altura", como número de niveles, de un g_tree *)

let maxFromList list = (*DADA UNA LISTA DEVUELVE SU MÁXIMO*)
  let rec iter max = function
    [] -> max
  | h::t -> if max > h then iter max t
            else iter h t
  in iter (List.hd list) (List.tl list)

let rec height = function
    Gt (r, []) -> 1
  | Gt (r, nodos) -> 1 + maxFromList(List.map height nodos)

(* devuelve la imagen especular de un g_tree *)

let rec mirror = function
    Gt (r, []) -> Gt (r,[])
  | Gt (r,lst) -> Gt (r, List.rev (List.map mirror lst))


(* devuelve la lista de nodos de un g_tree en "preorden" *)

let rec preorder = function 
    Gt (r, []) -> [r]
  | Gt (r, lst) -> r :: List.flatten (List.map preorder lst)


let rec postorder = function
    Gt (r, []) -> [r]
  | Gt (r,lst) -> List.flatten (List.map postorder lst) @ [r]


(*-----------------------------------------------------------------*)


  




  
