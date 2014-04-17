anioActual :: Int
anioActual = 2012
anioIngreso "george" = 2012
anioIngreso "sexy99" = 2011
anioIngreso "boca_cabj" = 2010
anioIngreso "jroman" = 2009

antiguedadUsuario user = anioActual - anioIngreso (user)

puntosBaseUsuario user = antiguedadUsuario (user) * length (user)


nivelUsuario user 		| antiguedadUsuario (user) < 1 = "Newbie"
				| puntosBaseUsuario (user) < 50 = "Intermedio"
				| True = "Avanzado"


puntosNivelUsuario user		| nivelUsuario (user) == "Newbie" = 1
				| nivelUsuario (user) == "Intermedio" = 5
				| nivelUsuario (user) == "Avanzado" = 10

puedeOtorgar user ptos = (user /= "admin") && (ptos < puntosNivelUsuario (user))