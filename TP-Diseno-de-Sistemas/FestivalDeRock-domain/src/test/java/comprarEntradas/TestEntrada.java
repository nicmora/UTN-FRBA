package comprarEntradas;

import static org.junit.Assert.*;

import org.junit.*;

import comprarEntradas.domain.Banda;
import comprarEntradas.domain.Categoria;
import comprarEntradas.domain.Dama;
import comprarEntradas.domain.Descuento;
import comprarEntradas.domain.Entrada;
import comprarEntradas.domain.Jubilado;
import comprarEntradas.domain.Mayor18;
import comprarEntradas.domain.Menor12;
import comprarEntradas.domain.Menor18;
import comprarEntradas.domain.Noche;
import comprarEntradas.domain.Venta;
import comprarEntradas.domain.Ubicacion;
import comprarEntradas.domain.PuestoDeVenta;

import java.text.ParseException;
import java.util.ArrayList;
import java.util.Collection;


public class TestEntrada {
	
	private Banda banda1;
	private Banda banda2;
	private Banda banda3;
	private Banda banda4;
	private Banda banda5;
	
	private Noche noche1;
	private Noche noche2;
	
	private Ubicacion ubicacion1;
	private Ubicacion ubicacion2;
	private Ubicacion ubicacion3;
	private Ubicacion ubicacion4;
	
	private PuestoDeVenta puestoDeVenta1;
	private PuestoDeVenta puestoDeVenta2;
	private PuestoDeVenta puestoDeVenta3;
	private PuestoDeVenta puestoDeVenta4;
	
	private Entrada entrada1;
	private Entrada entrada2;
	private Entrada entrada3;
	private Entrada entrada4;
	private Entrada entrada5;
	private Entrada entrada6;
	private Entrada entrada7;
	private Entrada entrada8;
	
	private Venta sistema;
	
	private Mayor18 mayor18;
	private Menor18 menor18;
	private Jubilado jubilado;
	private Dama dama;
	private Menor12 menor12_1;
	private Menor12 menor12_2;
	

	@Before
	public void setUp() throws Exception
	{
		banda1 = new Banda("Banda 1",new Categoria(1, 50));
		banda2 = new Banda("Banda 2",new Categoria(2, 100));
		banda3 = new Banda("Banda 3",new Categoria(3, 150));
		
		banda4 = new Banda("Banda 4",new Categoria(2, 100));
		banda5 = new Banda("Banda 5",new Categoria(4, 200));
		
		Collection<Banda> bandas1 = new ArrayList<Banda>();
		bandas1.add(banda1);
		bandas1.add(banda2);
		bandas1.add(banda3);
		
		Collection<Banda> bandas2 = new ArrayList<Banda>();
		bandas2.add(banda4);
		bandas2.add(banda5);
		
		noche1 = new Noche(1,bandas1, 20130810);
		noche2 = new Noche(2,bandas2, 20130815);
		
		ubicacion1 = new Ubicacion('A', 1, 1, 100);
		ubicacion2 = new Ubicacion('B', 2, 2, 200);
		ubicacion3 = new Ubicacion('C', 3, 3, 300);
		ubicacion4 = new Ubicacion('D', 4, 4, 400);
		
		entrada1 = new Entrada(1, false, ubicacion1, noche1, puestoDeVenta1);
		entrada2 = new Entrada(2, false, ubicacion2, noche1, puestoDeVenta3);
		entrada3 = new Entrada(3, true, ubicacion3, noche1, puestoDeVenta2);
		entrada4 = new Entrada(4, true, ubicacion4, noche1, puestoDeVenta4);
		entrada5 = new Entrada(5, false, ubicacion1, noche2, puestoDeVenta4);
		entrada6 = new Entrada(6, false, ubicacion2, noche2, puestoDeVenta1);
		entrada7 = new Entrada(7, true, ubicacion3, noche2, puestoDeVenta3);
		entrada8 = new Entrada(8, true, ubicacion4, noche2, puestoDeVenta2);
		
		Collection<Entrada> entradasNoche1 = new ArrayList<Entrada>();
		entradasNoche1.add(entrada1);
		entradasNoche1.add(entrada2);
		entradasNoche1.add(entrada3);
		entradasNoche1.add(entrada4);
		
		Collection<Entrada> entradasNoche2 = new ArrayList<Entrada>();
		entradasNoche2.add(entrada5);
		entradasNoche2.add(entrada6);
		entradasNoche2.add(entrada7);
		entradasNoche2.add(entrada8);
		
		noche1.setEntradas(entradasNoche1);
		noche2.setEntradas(entradasNoche2);
		
		sistema = new Venta(new Descuento());
		
		sistema.addNoche(noche1);
		sistema.addNoche(noche2);
		
		mayor18 = new Mayor18("nombre1","apellido1",20);
		menor18 = new Menor18("nombre2","apellido2",25);
		jubilado = new Jubilado("nombre3","apellido3",60);
		dama = new Dama("nombre4","apellido4",30);
		menor12_1 = new Menor12("nombre5","apellido5",10,true);
		menor12_2 = new Menor12("nombre6","apellido6",10,false);
	}
	
	
 	@Test
	public void testObtenerPrecio() throws ParseException
 	{
		assertTrue(entrada1.getPrecio() == 250);
	}

 	
	@Test
	public void testPrecioPorNocheCategoria3() throws ParseException 
	{
		assertTrue(noche1.precioExtra() == 150);
	}
	
	
	@Test
	public void testComprarEntradaMayor18Noche1SinDescuentoAnticipada() throws ParseException 
	{	
		assertTrue(sistema.comprarEntrada(mayor18, entrada1, "Efectivo") == 250);
	}
	
	
	@Test
	public void testComprarEntradaMayor18Noche1ConDescuento() throws ParseException 
	{	
		sistema.setFecha(20130501);
		assertTrue(sistema.comprarEntrada(mayor18, entrada1, "Efectivo") == 225);
	}


	@Test
	public void testPrecioEntradaJubiladoSinDescuentoAnticipadoCategoria3() throws ParseException 
	{
		assertTrue(sistema.comprarEntrada(jubilado, entrada1, "Efectivo") == 213);
	}
	
	
	@Test
	public void testComprarEntradaJubiladoConDescuentoAnticipadoCategoria3() throws ParseException 
	{
		sistema.setFecha(20130501);
		assertTrue(sistema.comprarEntrada(jubilado, entrada1, "Efectivo") == 188);
	}


	@Test
	public void testComprarEntradaMenor18SinDescuentoAnticipado() throws ParseException 
	{
		assertTrue(sistema.comprarEntrada(menor18, entrada2, "Efectivo") == 280);
	}
	
	
	@Test
	public void testComprarEntradaMenor18ConDescuentoAnticipado() throws ParseException 
	{
		sistema.setFecha(20130501);
		assertTrue(sistema.comprarEntrada(menor18, entrada2, "Efectivo") == 245);
	}
	

	@Test
	public void testComprarEntradaVipSinDescuentoAnticipada() throws ParseException
	{
		assertTrue(sistema.comprarEntradaVip(mayor18, entrada3, "Efectivo") == 1425);
	}
	
	
	@Test
	public void testComprarEntradaVipConDescuentoAnticipada() throws ParseException
	{
		sistema.setFecha(20130501);
		assertTrue(sistema.comprarEntradaVip(mayor18, entrada3, "Efectivo") == 1330);
	}
	

	@Test
	public void testComprarComboSinDescuentoAnticipada() throws ParseException 
	{
		ArrayList<Entrada> ListaEntradas = new ArrayList<Entrada>();
		
		ListaEntradas.add(entrada1);
		ListaEntradas.add(entrada2);
		ListaEntradas.add(entrada5);
		ListaEntradas.add(entrada6);
		
		assertTrue(sistema.comprarCombo(mayor18, ListaEntradas, "Efectivo") == 1170);
	}
	
}