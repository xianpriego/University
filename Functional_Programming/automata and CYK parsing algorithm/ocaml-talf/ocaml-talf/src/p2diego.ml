#load "talf.cma";;
open Conj;;
open Auto;;
open Ergo;;
open Graf;;

(* EJERCICIO 1 *)

(* pruebas *)
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



let es_fnc gic = match gic with
  Gic (_, _, Conjunto reglas, No_terminal s) -> let rec aux l = match l with
                                                  []-> true
                                                  | Regla_gic (No_terminal _, [No_terminal _; No_terminal _])::t
                                                  | Regla_gic (No_terminal _, [Terminal _])::t -> aux t
                                                  | _ -> false
                                                in aux reglas
  | _ -> raise (Failure "es_fnc: el axioma de la gramatica es un terminal");;

(* pruebas *)
es_fnc g;;
es_fnc g2;;
es_fnc g3;;
es_fnc g4;;



(* EJERCICIO 2*)

(* obtenemos el simbolo inicial de la gramatica *)
let simbolo_inicial gic = match gic with
  Gic(_, _, _, s) -> s;; 

(* dada la lista con la tabla acabada, comprobamos si "S" esta en la casilla final*)
let pertenencia l longitud gic = match List.assoc (longitud,1) l with
  conj -> pertenece (simbolo_inicial gic) conj
| exception Not_found -> false;;

(* para calcular las posibles combinaciones a traves de la diagonal *) (* Uso la posicion (x,y) siendo "x" la fila e "y" la columna *)
let posibilidades l x y =
  let rec aux kAux cAux = if (kAux < x) then aux (kAux+1) (union (cartesiano (List.assoc (kAux,y) l) (List.assoc (x-kAux,y+kAux) l)) cAux)
                          else cAux
  in aux 1 conjunto_vacio;;

(* dado dos simbolos y el conjunto de reglas obtenemos los no terminales que generan esos dos simbolos*)
let aux_buscar_gen_no_term s1 s2 gic = match gic with
  Gic (_, _, Conjunto reglas, _) -> let rec aux s1 s2 reglas acum = match reglas with 
                                      [] -> acum
                                      | Regla_gic (h1,h2)::t -> if ([s1;s2] = h2) then aux s1 s2 t (agregar h1 acum)
                                                                else aux s1 s2 t acum
                                    in aux s1 s2 reglas conjunto_vacio;;

(* almacenamos los no terminales que generan a cada posible par de no terminales *)
let buscar_gen_no_term l gic x y = 
  let rec aux cart acum = match cart with
    Conjunto [] -> acum
    | Conjunto ((h1,h2)::t) -> aux (Conjunto t) (union (aux_buscar_gen_no_term h1 h2 gic) acum)
  in aux (posibilidades l x y) conjunto_vacio;;

(* a partir de lo generado anteriormente se generan las columnas de la fila siguiente*)
let linea_siguiente l gic x num =
  let rec aux lAux numAux y = if (numAux > 0) then aux (((x,y),(buscar_gen_no_term l gic x y))::lAux) (numAux-1) (y+1)
                                else lAux
  in aux l num 1;;


(* buscamos los simbolos no terminales que generan a los terminales de la cadena *)
let buscar_gen_term simb gic = match gic with
  Gic (_, _, Conjunto reglas, _) -> let rec aux simb reglas acum = match reglas with 
                                      [] -> acum
                                      | Regla_gic (h1,h2)::t -> if ([simb] = h2) then aux simb t (agregar h1 acum)
                                                              else aux simb t acum
                                    in aux simb reglas conjunto_vacio;;

(* auxiliar para sacar la primera fila de la tabla de analisis *)
let primera_linea l gic =
  let rec aux lAux acum x y = match lAux with
    [] -> acum
    | h::t -> aux t (((x,y),(buscar_gen_term h gic))::acum) x (y+1)
  in aux l [] 1 1;;

let cyk w gic = 
  if (es_fnc gic) then match w with 
    [] -> raise (Failure "cyk: la cadena debe tener al menos un simbolo")
    | l -> let acum = primera_linea l gic
          in let longitud = List.length l
          in let rec aux altura anchura lAux = if (altura <= longitud) then aux (altura+1) (anchura-1) (linea_siguiente lAux gic altura anchura)
                                  else pertenencia lAux longitud gic
            in aux 2 (longitud-1) acum
  else raise (Failure "cyk: la gramatica no esta en FNC");;