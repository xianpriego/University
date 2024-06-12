%Xian Priego Martín, xian.priego@udc.es
%Iria Pardo Neira, iria.pardo.neira@udc.es

%------------------MASYU PUZZLE------------------%

%Definición de nodoos.
node(0..(n*n)-1).

%Generacion de aristas.
%Para el subconjunto de nodos que elimina la última columna, generamos un seg que une a cada uno con el de su derecha.
{segment(X, X+1) : node(X)} :- X\n != n-1.

%Para el subconjunto de nodos que elimina la última fila, generamos un seg que une a cada uno con el inmediatamente inferior.
{segment(X, X+n) : node(X)} :- X/n != n-1.

%Aplicamos espejo para los seg, ya que ir de x a y es lo mismo que ir de y a x.
segment(X,Y) :- segment(Y,X).

%Definimos un predicado que para un nodo cualquiera dice si está tocado por el camino, es decir, si existe algun seg que lo incluya.
touched(X) :- segment(X,Y).

%Nos quedamos con el primero de los nodos que está en la solución.
minnode(M):- #min{X:touched(X)} = M.

%Definimos un predicado recursivo reach, que dice que un nodo Y es alcanzable desde el primer nodo M (mínimo).
%Caso base: Existe un segmento entre ellos.
%Este predicado está optimizado, ya que se verifica si un nodo es alcanzable respecto a un único nodo y no respecto a todos.
reach(Y) :- minnode(M), segment(M,Y).
reach(Z) :- reach(Y), segment(Y,Z). 

%No puede ser que dado un nodo tocado, no alcance el primer nodo (mínimo). Esto hace que las soluciones sean conexas.
:- touched(X), not reach(X).

%No puede ser que un nodo tocado, no lo esté por exactamente 2 segmentos. Esto hace que la solución sea un ciclo.
:- touched(X), #count{Y: segment(X,Y)} != 2.

%Definimos predicado circulo(X) que nos dice si en el nodo X hay un círculo(sea blanco o negro)
circle(X) :- white(X).
circle(X) :- black(X).

%Predicado para indicar que un nodo es atravesado en línea recta.
straight(X) :- segment(X-1,X), segment(X,X+1).
straight(X) :- segment(X-n,X), segment(X,X+n).

%Predicado para indicar que un nodo es un codo.
corner(X) :- touched(X), not straight(X).

%No puede ser que un círculo blanco no esté en una línea recta y que niguno de sus vecinos sea una esquina.
:- white(X), not straight(X).
:- white(X), segment(X,Y), segment(X,Z), Y!=Z, not corner(Y), not corner(Z).

%No puede ser que un círculo negro no esté en un codo y que alguno de sus vecinos contega otro codo.
:- black(X), not corner(X).
:- black(X), segment(X,Y),  not straight(Y).

seg(X,Y) :- segment(X,Y), X<Y.

#show seg/2.