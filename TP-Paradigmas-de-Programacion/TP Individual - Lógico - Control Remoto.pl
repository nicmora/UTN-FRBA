/*Parte A*/

hermano(rodrigo).
hermano(sofia).
hermano(miguel).
hermano(alberto).
hermano(natalia).

edad(rodrigo,15).
edad(sofia,12).
edad(miguel,10).
edad(alberto,7).
edad(natalia,5).

esHombre(rodrigo).
esHombre(miguel).
esHombre(alberto).
esMujer(sofia).
esMujer(natalia).

esBromista(natalia).
esBromista(rodrigo).
esBromista(miguel).

/*leEscondeUnObjeto(X,rodrigo):-esBromista(X),esMenor(X,rodrigo),loPelea(rodrigo,X).*/

esMenor(X,Y):-edad(X,A),edad(Y,B),A<B.

loPelea(rodigo,alberto).
loPelea(sofia,X):-esMenor(X,sofia).
loPelea(alberto,X):-hermano(X),not(esMenor(X,miguel)).
loPelea(miguel,X):-hermano(X),not(esBromista(X)).
loPelea(X,Y):-sexoOpuesto(X,Y).

sexoOpuesto(X,Y):-esHombre(X),esMujer(Y).

/*Parte B*/

leInteresa(sofia,patines).
leInteresa(miguel,play).
leInteresa(alberto,pelota).
leInteresa(natalia,muñeca).

leEscondeUnObjeto(X,Y):-esBromista(X),esMenor(X,Y),loPelea(Y,X).
queObjetoLeEsconde(X,Y,A):-leInteresa(Y,A),leEscondeUnObjeto(X,Y).
