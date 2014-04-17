-- PARADIGMAS DE PROGRAMACION - AÑO 2012 - Curso K2011 - Grupo Nº 4 - Integrantes: Juan Francisco Briones, Esteban Chacho Taylor, Diego Federico Forcella, Nicolás Martín Morales, Flavio Ovidio Sirianni.


-- TP FUNCIONAL AGUANTE ARBOLITO


-- PRIMERA PARTE


-- La pampa tiene el ombú


arbolesEn "la reserva" = [(limonero, 3.0, 2.0, 4.2), (jacaranda, 6.0, 1.0, 1.4), (pino, 5.0, 3.0, 1.9), (eucalipto, 5.0, 4.0, 0.7), (jacaranda, 10.0, 2.0, 1.0), (cerezo, 7.0, 11.0, 0.9), (ombu, 8.0, 10.0, 2.1)]

primeraTemporada = [(granizo), (lluvia 3.0), (lluvia 1.0), (temperatura 4.0), (lluvia 55.0)]

segundaTemporada = [(crece), (podar 1.0), (lluvia 3.0), (lluvia 1.0), (temperatura 4.0), (lluvia 55.0)]

terceraTemporada = [(incendio), (lluvia 3.0), (lluvia 1.0), (temperatura 4.0), (lluvia 55.0), (podar 1.0)]


cerezo = ("cerezo", escalonado)
ombu = ("ombu", progresivo)
jacaranda = ("jacaranda", progresivo)
pino = ("pino", progresivo)
eucalipto = ("eucalipto", escalonado)
limonero =("limonero", chueco)


progresivo x = x / 2
escalonado x = sqrt x
torcido = progresivo.escalonado
chueco x = (escalonado x) + x / 5


danioPorIncendioArbol = [("jacaranda", moderado), ("ombu", critico), ("cerezo", moderado), ("pino", parcial), ("eucalipto", critico), ("limonero", minimo)]


minimo x = x * 5 / 100
moderado  x = x * 20 / 100
parcial x = x * 50 / 100
critico x = x * 90 / 100


type Especie = (String, Double -> Double)
type Arbol = (Especie, Double, Double, Double)
type Arbol2 = (String, Double, Double, Double)
type Incendio = Double -> Double


especie :: Arbol -> Especie
especie (x,_,_,_) = x


altura :: Arbol -> Double
altura (_,x,_,_) = x


anchura :: Arbol -> Double
anchura (_,_,x,_) = x


vitalidad :: Arbol -> Double
vitalidad (_,_,_,x) = x


mostrar :: Arbol -> Arbol2
mostrar (especie, altura, anchura, vitalidad) = (fst especie, altura, anchura, vitalidad)


--mostrarxs :: [Arbol] -> [Arbol2]
mostrarxs [(especie, altura, anchura, vitalidad)] = [(fst especie, altura, anchura, vitalidad)]


-- a)


nombresFrondosos :: [Arbol] -> [String]
nombresFrondosos xs = map (fst.especie) (filter esFrondoso xs)

-- nombresFrondosos xs = [ (fst.especie) x | x <- xs, esFrondoso x ] (Por listas por comprensión)


esFrondoso :: Arbol -> Bool
esFrondoso (_, altura, anchura, _) = (altura >= 6) && (altura <= 15) && (anchura > altura)


-- b)


todosLosFrondososSonVitales :: [Arbol] -> Bool
todosLosFrondososSonVitales xs = all (>1) (vitalidadArboles (filter esFrondoso xs))


vitalidadArboles :: [Arbol] -> [Double]
vitalidadArboles xs = map vitalidad xs 


-- ¡Qué buen clima!


-- a)


lluvia :: Double -> Arbol -> Arbol
lluvia mm unArbol = (especie unArbol, (altura unArbol) + 1, anchura unArbol, (vitalidad unArbol) + (vitalidad unArbol) * mm / 100)


temperatura :: Double -> Arbol -> Arbol
temperatura grados unArbol	|	grados < 0  = (especie unArbol, altura unArbol, anchura unArbol, (vitalidad unArbol) / 2)
							|	grados > 40 = (especie unArbol, altura unArbol, anchura unArbol, (vitalidad unArbol) - (vitalidad unArbol) * 0.4)
							|	otherwise = (especie unArbol, altura unArbol, anchura unArbol, vitalidad unArbol)


granizo :: Arbol -> Arbol
granizo unArbol = (especie unArbol, (altura unArbol) / 2, (anchura unArbol) / 2, vitalidad unArbol)


--b)


hacerCrecerArboles :: [Arbol] -> [Arbol]

hacerCrecerArboles xs = map crece xs


crece :: Arbol -> Arbol

crece unArbol = (especie unArbol, nuevaAltura unArbol, anchura unArbol, vitalidad unArbol)


-- La definición de la funcion nuevaAltura forma parte del siguiente punto.


-- Dejalo crecer que ya le va a llegar la poda...


-- a)


nuevaAltura :: Arbol -> Double

nuevaAltura unArbol = ((altura unArbol) + ((snd.especie) unArbol) (altura unArbol))


-- b)


podar :: Double -> Arbol -> Arbol

podar m unArbol = (especie unArbol, (altura unArbol) - m, (anchura unArbol) - m, (vitalidad unArbol) + (vitalidad unArbol) * 0.1)


-- c)

-- Agregado al comienzo el arbol: (limonero, 3, 2, 4.2)

-- limonero = ("limonero", chueco)

-- chueco x = escalonado x + x / 5

-- Se garantiza que sigue funcionando ya que se respetan las estructuras de datos.


-- SEGUNDA PARTE


-- ¡Hay que pasar el invierno!


-- a)


finTemporadaRecursiva unArbol factoresClimaticos = mostrar (arbolTemporadaRecursiva unArbol factoresClimaticos)

arbolTemporadaRecursiva unArbol (x:[]) = x unArbol

arbolTemporadaRecursiva unArbol (x:xs) = arbolTemporadaRecursiva (x unArbol) xs 


finTemporadaNoRecursiva unArbol factoresClimaticos = mostrar (arbolTemporadaNoRecursiva unArbol factoresClimaticos)

arbolTemporadaNoRecursiva unArbol factoresClimaticos = (armaComposicion factoresClimaticos) unArbol

armaComposicion (x:xs) = foldr (.) x xs


-- b)


-- No se realizaron cambios, solo agregamos al comienzo la lista segundaTemporada que incluye las fuciones "podar" y "crece".


-- c)


incendio :: Arbol -> Arbol

incendio unArbol = (especie unArbol, altura unArbol, anchura unArbol, vitalidad unArbol - ((tipoIncendio unArbol) (vitalidad unArbol)))


tipoIncendio :: Arbol -> Incendio

-- Opción 1 (expresión lambda)

tipoIncendio unArbol = snd (head (filter (\arbolDanio	->	((fst arbolDanio) == ((fst.especie) unArbol))) danioPorIncendioArbol))


-- Opción 2

-- tipoIncendio unArbol = snd (head (filter ((==((fst.especie) unArbol)).fst) danioPorIncendioArbol))


{- 
Reflexiones a la sombra del árbol


a) Solo se podrían reutilizar aquellas funciones que no reciban esas estructuras de datos, por ejemplo, las funciones de crecimiento (progresivo, escalonado, torcido, chueco) y las funciones de incendio (minimo, moderado, parcial, critico). No se podrían reutilizar el resto de las funciones.


b) Las funciones pueden trabajar con listas potencialmente infinitas gracias a la evaluación diferida (lazy evaluation), pero deben realizar un corte en algún momento.


c) Realizado en Opción 1 y 2 del punto c) de la SEGUNDA PARTE (¡Hay que pasar el invierno!). Utilizamos la Opción 1 (expresión lambda) ya que nos parece más clara (más expresiva) que la Opción 2. 


PRUEBAS EN WINHUGS


Main> nombresFrondosos (arbolesEn "la reserva")
["cerezo","ombu"]

Main> todosLosFrondososSonVitales (arbolesEn "la reserva")
False

Main> mostrar (lluvia 30 (arbolesEn "la reserva"!!2))
("pino",6.0,3.0,2.47)

Main> mostrar (temperatura 50 (arbolesEn "la reserva"!!2))
("pino",5.0,3.0,1.14)

Main> mostrar (granizo (arbolesEn "la reserva"!!2))
("pino",2.5,1.5,1.9)

Main> mostrarxs (hacerCrecerArboles (arbolesEn "la reserva"))
Program error: pattern match failure: mostrarxs [(especie (limonero,3.0,2.0,4.2),nuevaAltura (limonero,3.0,2.0,4.2),anchura (limonero,3.0,2.0,4.2),vitalidad (limonero,3.0,2.0,4.2)),crece (jacaranda,6.0,1.0,1.4)] ++ map_v810 crece [(pino,5.0,3.0,1.9),(eucalipto,5.0,4.0,0.7),(jacaranda,10.0,2.0,1.0),(cerezo,7.0,11.0,0.9),(ombu,8.0,10.0,2.1)]

Main> nuevaAltura (arbolesEn "la reserva"!!2)
7.5

Main> mostrar (podar 2 (arbolesEn "la reserva"!!2))
("pino",3.0,1.0,2.09)

Main> mostrar (lluvia 20 (limonero, 3.0, 2.0, 4.2))
("limonero",4.0,2.0,5.04)

Main> mostrar (temperatura (-5) (limonero, 3.0, 2.0, 4.2))
("limonero",3.0,2.0,2.1)

Main> mostrar (granizo (limonero, 3.0, 2.0, 4.2))
("limonero",1.5,1.0,4.2)

Main> mostrar (podar 2 (limonero, 3.0, 2.0, 4.2))
("limonero",1.0,0.0,4.62)

Main> finTemporadaRecursiva (arbolesEn "la reserva"!!2) primeraTemporada
("pino",5.5,1.5,3.0636835)

Main> finTemporadaNoRecursiva (arbolesEn "la reserva"!!2) primeraTemporada
("pino",5.5,1.5,3.0636835)

Main> finTemporadaRecursiva (arbolesEn "la reserva"!!2) segundaTemporada
("pino",9.5,2.0,3.37005185)

Main> finTemporadaNoRecursiva (arbolesEn "la reserva"!!2) segundaTemporada
("pino",9.5,2.0,3.37005185)

Main> mostrar (incendio (arbolesEn "la reserva"!!2))
("pino",5.0,3.0,0.95)

Main> finTemporadaRecursiva (arbolesEn "la reserva"!!2) terceraTemporada
("pino",7.0,2.0,1.685025925)

Main> finTemporadaNoRecursiva (arbolesEn "la reserva"!!2) terceraTemporada
("pino",7.0,2.0,1.685025925)
-}



