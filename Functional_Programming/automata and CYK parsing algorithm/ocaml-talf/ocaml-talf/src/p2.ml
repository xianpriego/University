(*Xian Priego Martin, 39457432N*)
#load "talf.cma";;
open Conj;;
open Auto;;
open Ergo;;
open Graf;;

let g = gic_of_string "S A B C; a b; S;
                      S -> A B | B C;
                      A -> B A | a;
                      B -> C C | b;
                      C -> A B | a;";;

let g2 = gic_of_string "S A B C D; a b; S;
                      S -> A B D| B C;
                      A -> B A | a;
                      B -> C C | b;
                      C -> A B | a;
                      D -> b;";;

let g3 = gic_of_string "S A B C D; a b; S;
                      S -> A B | B C;
                      A -> B A | a;
                      B -> C C | D b;
                      C -> A B | a;
                      D -> b;";;

let g4 = gic_of_string "S A B C; a b; S;
                        S -> A B | B C;
                        A -> B A | a;
                        B -> C | b;
                        C -> A B | a;";;

(*Ejercicio 1: es_fnc*)

let es_fnc gic = match gic with
  Gic (_, _, Conjunto reglas, No_terminal s) -> let rec aux l = match l with
                                                  [] -> true
                                                  | Regla_gic (No_terminal _, [No_terminal _; No_terminal _])::t
                                                  | Regla_gic (No_terminal _, [Terminal _])::t -> aux t
                                                  | _ -> false 
                                                in aux reglas
| _ -> raise(Failure "es_fnc: el axioma de la gramática es un terminal")
                                        
 
(*Ejercicio 2: cyk*)

let generadores_de_terminal simb gic = match gic with
  Gic(_, _, Conjunto reglas, _) ->  let rec aux reglas acum = match reglas with
                                      [] -> acum
                                      |Regla_gic(generador,generado)::t -> if generado = [simb] then aux t (agregar generador acum)
                                                                  else aux t acum
                                    in aux reglas (Conjunto [])

                                    
                                    
let primera_fila gic cadena = 
  let rec aux cadena fila y = match cadena with 
    [] -> fila
    |h::t -> aux t (((1, y), (generadores_de_terminal h gic))::fila) (y+1)
  in aux cadena [] 1


let aux_generadores_de_no_terminal tabla gic coord1 coord2 = 
  let combinaciones = cartesiano (List.assoc coord1 tabla) (List.assoc coord2 tabla)
  in let Gic(_, _, Conjunto reglas, _) = gic
  in let rec aux reglas generadores = match reglas with
    [] -> generadores
  | Regla_gic(generador, [simb1; simb2])::t -> if pertenece (simb1, simb2) combinaciones then aux t (agregar generador generadores)
                                               else aux t generadores
  | _ :: t -> aux t generadores
  in aux reglas conjunto_vacio  



let generadores_de_no_terminal tabla gic k x y = 
  let rec aux k generadores = match k with 
    0 -> generadores
  | _ -> aux (k-1) (union generadores (aux_generadores_de_no_terminal tabla gic (x-k, y) (k, y+(x-k))))
  in aux k conjunto_vacio
  

let anadir_fila altura anchura tabla gic = 
  let rec aux tabla j = 
    if j > anchura then tabla
    else aux (((altura, j), (generadores_de_no_terminal tabla gic (altura-1) altura j))::tabla) (j+1)
  in aux tabla 1 


let get_s = function
  Gic(_,_,_,s) -> s

let comprobacion tabla longitud gic = match (List.assoc (longitud, 1)) tabla with
  conjunto -> pertenece (get_s gic) conjunto
| exception Not_found -> false


let cyk w gic = 
  if (es_fnc gic) then match w with
    [] -> raise (Failure "La cadena debe tener al menos un símbolo")
    |l -> let fila1 = primera_fila gic w
          in let longitud = List.length l
          in let rec aux altura anchura tabla = if (altura <= longitud) then aux (altura+1) (anchura-1) (anadir_fila altura anchura tabla gic) 
                                                else comprobacion tabla longitud gic

          in aux 2 (longitud-1) fila1

  else raise(Failure "cyk: la gramática introducida no está en FNC.")
  
