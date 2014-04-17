% TP ESPIONAJE
% Grupo Nro 4 ;)
%-----------------------------------------------------------------------------------

% Datos del TP

agente(sloane, espia([ingles, frances, italiano])).
agente(vaughn, francotirador(150)).
agente(bristow, tirabombas(26)).
agente(irina,  tirabombas(4)).
agente(guillermo, ninja(judo, [estrellita,nunchaku])).
agente(emilio, ninja(karate, [sai])).

mision(unViolinistaEnElTejado, 2, asesinato).
mision(expresoDeOriente, 4, volarTren).
mision(extermineitors, 2, exterminio).


requisito(asesinato, sigilo).
requisito(asesinato, distancia(100)). /*francotirador*/
requisito(volarTren, explosivos(11)). /*cant. de bombas*/
requisito(volarTren, idioma(italiano)). /*espía*/
requisito(volarTren, fuerza).
requisito(exterminio, fuerza).
requisito(exterminio, sigilo).

% 1)-----------------------------------------------------------------------------------

% cumpleRequisito/2 que permite saber si un agente cumple con un requisito 

cumpleRequerimiento(distancia(Metros), Nombre) :- agente(Nombre, francotirador(MaxMetros)), MaxMetros >= Metros.

cumpleRequerimiento(explosivos(Cantidad), Nombre) :- agente(Nombre, tirabombas(MaxCantidad)), MaxCantidad >= Cantidad.

cumpleRequerimiento(idioma(UnIdioma), Nombre) :- agente(Nombre, espia(Idiomas)), member(UnIdioma,Idiomas).

cumpleRequerimiento(sigilo, Nombre) :- agente(Nombre, espia(_)).
cumpleRequerimiento(sigilo, Nombre) :- agente(Nombre, ninja(_,_)).

cumpleRequerimiento(fuerza, Nombre) :- agente(Nombre, ninja(_,_)).
cumpleRequerimiento(fuerza, Nombre) :- agente(Nombre, tirabombas(_)).

% 2)-----------------------------------------------------------------------------------

% esUtil/2, que relaciona el nombre de una misión con cada agente que cumple con algún requisito de la misma
%esUtil(NombreMision,Nombre) :- mision(NombreMision, _, TipoMision), 
%			        requisito(TipoMision,Requerimiento),
% 			        cumpleRequerimiento(Requerimiento,Nombre).

esUtil(NombreMision, Agente) :- cumpleRequerimientoEnMision(_,NombreMision, Agente).

cumpleRequerimientoEnMision(Requerimiento, NombreMision, Agente) :-     mision(NombreMision, _, TipoMision),
									requisito(TipoMision, Requerimiento),
									cumpleRequerimiento(Requerimiento, Agente).
			        
% 3)-----------------------------------------------------------------------------------

%esIndispensable/2, que relaciona una misión y a un agente cuando este agente es el único que cumple con un algún requerimiento de la misión.

%Solucion con Listas Auxiliares:

esIndispensable(NombreMision,Nombre) :- mision(NombreMision, _, Requisito), 
					requisito(Requisito,Requerimiento), 
					quienesCumplen(Requerimiento,[Nombre]).

quienesCumplen(Requerimiento,Lista) :- findall(Agente, cumpleRequerimiento(Requerimiento,Agente), Lista).

%Faltaria solucion sin listas Auxiliares


% 4)-----------------------------------------------------------------------------------

%rambo/1, que se cumple para un agente si este es útil para todas las misiones.

rambo(Nombre) :- agente(Nombre,_), 
		 forall( mision(NombreMision,_,_), esUtil(NombreMision,Nombre) ).

% 5)-----------------------------------------------------------------------------------

%equipoPosible/2: Relaciona a una misión y un conjunto de agentes útiles para la misma. Considerar la cantidad de agentes requerida.

equipoPosible(NombreMision, Lista) :- mision(NombreMision, CantRequeridos, _), 
				      candidatosPara(NombreMision, Agentes), 
				      combinaciones(Agentes, Lista), 
				      length(Lista,CantRequeridos).

candidatosPara(NombreMision, Agentes) :- setof(Agente, esUtil(NombreMision, Agente), Agentes).

combinaciones([],[]).
combinaciones([X|Xs],[X|Ys]) :- combinaciones(Xs,Ys).
combinaciones([_|Xs],Ys) :- combinaciones(Xs,Ys). 

% 6)------------------------------------------
% eshImposhible/1, que se cumple para aquella misión para la cual ningún equipo posible cumple todos los requisitos.

eshImposhible(Mision) :- mision(Mision,_,_), 
			 forall( equipoPosible(Mision, Equipo), not( cumpleTodosLosRequisitos(Equipo,Mision) )   ).

cumpleTodosLosRequisitos(Equipo,Mision) :- mision(Mision, _, Requisito), forall( requisito(Requisito, Requerimiento), (member(Agente,Equipo), cumpleRequerimiento(Requerimiento, Agente)) ).



% 7)------------------------------------------
% estoEstaLlenoDeNinjas/1, que se cumple para aquella misión en la que todos los equipos posibles están formados únicamente por ninjas.

estoEstaLlenoDeNinjas(Mision) :- mision(Mision,_,_), forall( equipoPosible(Mision,Equipo), equipoLlenoDeNinjas(Equipo) ).

equipoLlenoDeNinjas(Equipo) :- forall( member(Agente,Equipo), esNinja(Agente) ).

esNinja(Agente) :- agente(Agente, ninja(_,_) ). 











