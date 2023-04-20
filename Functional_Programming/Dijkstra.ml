(*IMPLEMENTATION OF DIJKSTRA'S ALGORITHM IN FUNCTIONAL PARADIGM TO SOLVE THE SHORTEST PATH PROBLEM IN A GRID WITH SEVERAL TREES
  IN WHICH AN SQUIRREL HAVE TO GO FROM (1,1) CELL TO (N,N) CELL JUMPING OVER THE TREES*)


(*Como vamos a realizar búsquedas intensivas sobre los nodos ya visitados,
   estos los vamos a guardar en un Set, que realiza la búsqueda mediante
   un AVL lo cual es mucho más eficiente*)

module IntPair = 
      struct
        type t = int * int
        let compare = compare
      end;;


module IPS = Set.Make (IntPair)


(*cola de prioridad implementada mediante un monticulo leftist
   Esta implementación no es nuestra, la hemos sacado de http://typeocaml.com/2015/03/12/heap-leftist-tree/*)

(*Usaremos esta cola de prioridad para ir alacenando los caminos posibles que podemos tomar, ordenados en función
   de su peso, de esta forma, para elegir el camino mínimo simplemente tenemos que extraer la raíz*)

type 'a leftist =  
  | Leaf 
  | Node of 'a leftist * 'a * 'a leftist * int

let singleton k = Node (Leaf, k, Leaf, 1)

let rank = function Leaf -> 0 | Node (_,_,_,r) -> r  

let rec merge t1 t2 =  
  match t1,t2 with
    | Leaf, t | t, Leaf -> t
    | Node (l, k1, r, _), Node (_, k2, _, _) ->
      if k1 > k2 then merge t2 t1 (* switch merge if necessary *)
      else 
        let merged = merge r t2 in (* always merge with right *)
        let rank_left = rank l and rank_right = rank merged in
        if rank_left >= rank_right then Node (l, k1, merged, rank_right+1)
        else Node (merged, k1, l, rank_left+1) (* left becomes right due to being shorter *)

let insert x t = merge (singleton x) t
let insert t x = insert x t (*Este cambio se hace para que funcione el fold left en la función add_to_queue*)

let get_min = function  
  | Leaf -> failwith "empty"
  | Node (_, k, _, _) -> k

let delete_min = function  
  | Leaf -> failwith "empty"
  | Node (l, _, r, _) -> merge l r;;

(**********************************************************************************************************)


let a_salto d (x1,y1) (x2,y2) = 
  (x1 = x2 && y1 <> y2 && abs(y1-y2)<= d) 
  || (y1 = y2 && x1 <> x2 &&abs(x1-x2) <= d);;


let adjacents visitados node jump trees = 
  List.filter (fun a -> a_salto jump node a && not(IPS.mem a visitados)) trees;;


let weight (x1,y1) (x2,y2) =
  if x1 = x2 then abs(y1-y2)
  else abs(x1 - x2);;


let add_weights node adjacents = 
  List.map (fun x -> (x, weight node x)) adjacents


let posibles_nodos node jump trees visitados = 
  add_weights node (adjacents visitados node jump trees) 


let add_to_queue prio_queue min_way peso lista_nodos = 
  List.fold_left (fun x y -> insert x (peso + snd y, (fst y)::min_way)) prio_queue lista_nodos


let buscar_caminos jump trees prio_queue visitados =    (*Esta función nos devuelve una cola de prioridad a la que le añadimos los posibles caminos desde node*)
  let peso, min_way = get_min prio_queue in
  let node = List.hd min_way in 
  let lista_nodos = posibles_nodos node jump trees visitados 
  in add_to_queue prio_queue min_way peso lista_nodos;;


(*Función principal*)

let shortest_tour m n trees jump =  (*Si la función buscar_caminos devuelve la prio_queue vacía, no hay camino*)
    let rec caminante visitados prio_queue = match prio_queue with
        Leaf -> raise(Not_found)
      | Node(_, (peso, h::t), _, _) -> if h = (m,n) then List.rev (h::t) (*Aquí el compilador da un partial matching, pero ese caso que falta nunca va a tener lugar*)
                                       else if (IPS.mem h visitados) then caminante visitados (delete_min prio_queue)
                                       else caminante (IPS.add h visitados) (buscar_caminos jump trees prio_queue visitados)
    in caminante (IPS.singleton (1,1)) (buscar_caminos jump trees (singleton (0,[(1,1)])) (IPS.singleton (1,1))) 






