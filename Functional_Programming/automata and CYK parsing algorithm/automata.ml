

#load "talf.cma";;
open Conj;;
open Auto;;
open Ergo;;
open Graf;;


(*Pruebas: *)
let af1 = af_of_string "0 1 2 3; a b c; 0; 1 3; 0 1 a; 1 1 b; 1 2 a; 2 0 epsilon; 2 3 epsilon; 2 3 c;";;
let af2 = af_of_string "0 1 2 3; a b c; 0; 1 3; 0 1 epsilon; 1 1 b; 1 2 a; 2 0 epsilon; 2 3 epsilon; 2 3 c;";;
let afd = af_of_string "0 1 2 3; a b c; 0; 1 3; 0 1 a; 1 1 b; 1 2 a; 2 3 c;";; 
let afd2 = af_of_string "0 1 2 3; a b c; 0; 1 2 3; 0 1 a; 1 1 b; 1 2 a; 2 3 c; 3 2 c;";;
let afnd = af_of_string "0 1 2 3; a b c; 0; 1 3; 0 1 a; 1 1 b; 1 2 a; 2 3 c; 1 2 b;";;
let afnd2 = af_of_string "0 1 2 3; a b c; 0; 1 3; 0 1 a; 1 1 b; 1 2 a; 2 3 c; 1 2 b;";;


(*Ejercicio 1:*)


(*A) Implemente una función es_afne : Auto.af -> bool que reciba como argumento un autómata
finito, y que devuelva true si se trata de un autómata que presenta alguna épsilon-transición, o false en
caso contrario.*)

let es_epsilon = function
  Arco_af(_, _, Terminal "") -> true
  |_ -> false

let es_afne = function
  Af(_, _, _, Conjunto arcos, _) -> List.exists es_epsilon arcos

(*B) Implemente una función es_afn : Auto.af -> bool que reciba como argumento un autómata
finito, y que devuelva true si se trata de un autómata que presenta algún tipo de no determinismo
(excepto épsilon-transiciones), o false en caso contrario.*)

let no_determinismo lst = 
  let rec iter lst acum = match lst with
    [] -> false
    |Arco_af(Estado estado, _, Terminal simbolo)::t -> if ((simbolo <> "") && (List.mem (estado, simbolo) acum)) then true
                                                       else iter t ((estado, simbolo)::acum) 
  in iter lst [];;



let es_afn = function 
  Af(_, _, _, Conjunto arcos, _) -> no_determinismo arcos


(*C) Implemente una función es_afd : Auto.af -> bool que reciba como argumento un autómata
finito, y que devuelva true si se trata de un autómata totalmente determinista, o false en caso contrario.*)


let no_determinismo_o_epsilon lst = 
  let rec iter lst acum = match lst with
    [] -> false
    |Arco_af(Estado estado, _, Terminal simbolo)::t -> if ((simbolo = "") || (List.mem (estado, simbolo) acum)) then true
                                                       else iter t ((estado, simbolo)::acum) 
  in iter lst [];;


let es_afd = function
  Af(_, _, _, Conjunto arcos, _) -> not(no_determinismo_o_epsilon arcos)


(*----------------------------------------------------------------------------------------------------------------*)

(*Ejercicio 2*)

(*Implemente una función equivalentes : Auto.af -> Auto.af -> bool que reciba como
argumentos dos autómatas finitos y que devuelva true cuando ambos autómatas acepten el mismo
lenguaje, o false en caso contrario.*)


let rec lprod l1 l2 = match l1 with
  [] -> []
  |h::t -> List.map (fun x -> (h,x)) l2 @ lprod t l2


let obtener transiciones (estado_actual, simbolo) = 
  let rec iter transiciones estados_alcanzables = match transiciones with
    Conjunto [] -> estados_alcanzables
    |Conjunto (Arco_af(origen, destino, simb)::t) -> if origen = estado_actual && simb = simbolo then iter (Conjunto t) (destino::estados_alcanzables)
                                                  else iter (Conjunto t) estados_alcanzables
  in iter transiciones [];;



(*PRECONDICIÓN: LOS AUTÓMATAS INTRODUCIDOS EN LA FUNCIÓN SON DETERMINISTAS*)

let equivalentes af1 af2 =
  let Af(estados1, alfabeto1, estado_inicial1, transiciones1, estados_finales1) = af1 in
  let Af(estados2, alfabeto2, estado_inicial2, transiciones2, estados_finales2) = af2 in
  let alfabeto = Conj.interseccion alfabeto1 alfabeto2 in

  let rec explorar cola estados_visitados =
    match cola with
    | [] -> true
    | (estado_actual1, estado_actual2) :: t ->
        if Conj.pertenece (estado_actual1, estado_actual2) estados_visitados then
          explorar t estados_visitados
        else
          let es_final1 = Conj.pertenece estado_actual1 estados_finales1 in
          let es_final2 = Conj.pertenece estado_actual2 estados_finales2 in
          if (es_final1 && not es_final2) || (not es_final1 && es_final2) then
            false
          else
            let nuevos_estados = 
              List.concat (
                List.map (fun simbolo ->
                  let alcanzables_af1 = obtener transiciones1 (estado_actual1, simbolo) in
                  let alcanzables_af2 = obtener transiciones2 (estado_actual2, simbolo) in
                  lprod alcanzables_af1 alcanzables_af2
                ) (Conj.list_of_conjunto alfabeto)
              )
            in
            let nueva_cola = nuevos_estados @ t in
            explorar nueva_cola (Conj.agregar (estado_actual1, estado_actual2) estados_visitados)
  in explorar [(estado_inicial1, estado_inicial2)] (Conjunto [])



  


  
  

