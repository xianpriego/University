


open G_tree;;

let rec breadth_first = function
    Gt (x, []) -> [x]
  | Gt (x, (Gt (y, t2))::t1) -> x :: breadth_first (Gt (y, t1@t2));;

let breadth_first_t gt = 
  let rec iter lst = function
    Gt (x,[]) -> List.rev(x::lst)
  | Gt (x, (Gt (y, t2))::t1) -> iter (x::lst) (Gt (y, List.rev_append (List.rev t1) t2))
  in iter [] gt ;;

(*La función breadth_first_t es recursiva terminal, sin embargo, para árboles muy anchos
  va a ser bastante lenta, ya que el t1 va a ser muy grande y las operaciones rev y rev append van a ser
  costosas en tiempo. Por ello, tenemos que generar árboles muy grandes pero muy profundos, donde 
  breadth_first_t no va a tener problemas de tiempo y breadh_first provocará stack overflow*)

let rec deep_tree = function
  0 -> Gt(0, [])
  |n -> Gt(n, [deep_tree (n-1)]);;

let t2 = deep_tree 200000 ;;

