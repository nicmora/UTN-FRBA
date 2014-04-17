package comprarEntradas.repositorio;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import org.uqbar.commons.utils.Observable;
import org.uqbar.commons.utils.Transactional;

import comprarEntradas.domain.Banda;
import comprarEntradas.domain.Categoria;
import comprarEntradas.domain.Cliente;
import comprarEntradas.domain.Entrada;
import comprarEntradas.domain.Festival;
import comprarEntradas.domain.Noche;
import comprarEntradas.domain.PuestoDeVenta;
import comprarEntradas.domain.Ubicacion;
	

@SuppressWarnings("serial")
@Observable
@Transactional
public class Repositorio implements Serializable {
	private static Repositorio instance;
	private List<Festival> festivales = new ArrayList<Festival>();
	private List<Categoria> categorias = new ArrayList<Categoria>();
	private List<Banda> bandas = new ArrayList<Banda>();
	private List<Noche> noches = new ArrayList<Noche>();
	private List<Ubicacion> ubicaciones = new ArrayList<Ubicacion>();
	private List<Entrada> entradas = new ArrayList<Entrada>();
	private List<Cliente> clientes = new ArrayList<Cliente>();
	private List<PuestoDeVenta> puestosDeVenta = new ArrayList<PuestoDeVenta>();
	
	
	public static synchronized Repositorio getInstance() {
		if (instance == null) {
			instance = new Repositorio();
		}
		return instance;
	}

	private Repositorio() {
		
		Categoria categoria1 = new Categoria(1,50);
		Categoria categoria2 = new Categoria(2,100);
		Categoria categoria3 = new Categoria(3,150);
		Categoria categoria4 = new Categoria(4,200);
		
		categorias.add(categoria1);
		categorias.add(categoria2);
		categorias.add(categoria3);
		categorias.add(categoria4);
		
		Banda banda1 = new Banda("Banda 1",categoria1);
		Banda banda2 = new Banda("Banda 2",categoria2);
		Banda banda3 = new Banda("Banda 3",categoria3);
		Banda banda4 = new Banda("Banda 4",categoria2);
		Banda banda5 = new Banda("Banda 5",categoria4);

		bandas.add(banda1);
		bandas.add(banda2);
		bandas.add(banda3);
		bandas.add(banda4);
		bandas.add(banda5);
		
		Collection<Banda> bandas1 = new ArrayList<Banda>();
		bandas1.add(banda1);
		bandas1.add(banda2);
		bandas1.add(banda3);

		Collection<Banda> bandas2 = new ArrayList<Banda>();
		bandas2.add(banda4);
		bandas2.add(banda5);

		Noche noche1 = new Noche(1,bandas1, 20140110);
		Noche noche2 = new Noche(2,bandas2, 20140111);
		
		noches.add(noche1);
		noches.add(noche2);

		Ubicacion ubicacion1 = new Ubicacion('A', 1, 1, 100);
		Ubicacion ubicacion2 = new Ubicacion('B', 2, 2, 200);
		Ubicacion ubicacion3 = new Ubicacion('C', 3, 3, 300);
		Ubicacion ubicacion4 = new Ubicacion('D', 4, 4, 400);
		
		ubicaciones.add(ubicacion1);
		ubicaciones.add(ubicacion2);
		ubicaciones.add(ubicacion3);
		ubicaciones.add(ubicacion4);
		
		PuestoDeVenta puestoDeVenta1 = new PuestoDeVenta(1, "direccion1");
		PuestoDeVenta puestoDeVenta2 = new PuestoDeVenta(2, "direccion2");
		PuestoDeVenta puestoDeVenta3 = new PuestoDeVenta(3, "direccion3");
		PuestoDeVenta puestoDeVenta4 = new PuestoDeVenta(4, "direccion4");
		
		puestosDeVenta.add(puestoDeVenta1);
		puestosDeVenta.add(puestoDeVenta2);
		puestosDeVenta.add(puestoDeVenta3);
		puestosDeVenta.add(puestoDeVenta4);
		
		Entrada entrada1 = new Entrada(1, false, ubicacion1, noche1, puestoDeVenta1);
		Entrada entrada2 = new Entrada(2, false, ubicacion2, noche1, puestoDeVenta2);
		Entrada entrada3 = new Entrada(3, true, ubicacion3, noche1, puestoDeVenta1);
		Entrada entrada4 = new Entrada(4, true, ubicacion4, noche1, puestoDeVenta3);
		Entrada entrada5 = new Entrada(5, false, ubicacion1, noche2, puestoDeVenta3);
		Entrada entrada6 = new Entrada(6, false, ubicacion2, noche2, puestoDeVenta4);
		Entrada entrada7 = new Entrada(7, true, ubicacion3, noche2, puestoDeVenta2);
		Entrada entrada8 = new Entrada(8, true, ubicacion4, noche2, puestoDeVenta4);
		
		entradas.add(entrada1);
		entradas.add(entrada2);
		entradas.add(entrada3);
		entradas.add(entrada4);
		entradas.add(entrada5);
		entradas.add(entrada6);
		entradas.add(entrada7);
		entradas.add(entrada8);
		
		Collection<Entrada> entradas1 = new ArrayList<Entrada>();
		entradas1.add(entrada1);
		entradas1.add(entrada2);
		entradas1.add(entrada3);
		entradas1.add(entrada4);
		
		Collection<Entrada> entradas2 = new ArrayList<Entrada>();
		entradas2.add(entrada5);
		entradas2.add(entrada6);
		entradas2.add(entrada7);
		entradas2.add(entrada8);
		
		noche1.setEntradas(entradas1);
		noche2.setEntradas(entradas2);
		
		Festival festival1 = new Festival(1, noches, entradas, bandas);
		
		festivales.add(festival1);
	}
	
	// ********************************************************
	// ** ALTAS Y BAJAS
	// ********************************************************
	
	public void create(Cliente cliente) {
		this.clientes.add(cliente);
	}

	public void delete(Cliente cliente) {
		this.clientes.remove(cliente);
	}
	
	public List<Ubicacion> getUbicaciones() {
		return this.ubicaciones;
	}

	public void setUbicaciones(List<Ubicacion> ubicaciones) {
		this.ubicaciones = ubicaciones;
	}
	
	public List<Noche> getNoches() {
		return this.noches;
	}

	public void setNoches(List<Noche> noches) {
		this.noches = noches;
	}

	// ********************************************************
	// ** BUSQUEDAS
	// ********************************************************
	
	public List<Entrada> searchDisponibles(Noche noche, Ubicacion ubicacion) {
		List<Entrada> resultados = new ArrayList<Entrada>();
			
		for (Entrada entrada : this.entradas) 
		{
			if(!entrada.isVendida()
				&& (this.matchDatos(entrada, noche, ubicacion, null, null))) {
					resultados.add(entrada);
			}
		}

		return resultados;
	}
	
	public List<Entrada> searchOcupadas(Noche noche, Ubicacion ubicacion, Cliente cliente, Integer fechaDesde, Integer fechaHasta, Integer numeroPuestoDeVenta, String festivalID) {
		
		List<Entrada> resultados = new ArrayList<Entrada>();
		List<Entrada> entradasTemp = new ArrayList<Entrada>();
		Festival festivalTemp;
		String nombreCliente = null;
		String apellidoCliente = null;
		
		if(cliente != null){
			nombreCliente = cliente.getNombre();
			apellidoCliente = cliente.getApellido();
		}
		
		if (festivalID == null) {
			for (Festival festival : this.festivales) {
				entradasTemp = festival.getEntradas();
			}	
		}					
		else {
				festivalTemp = this.searchFestival(festivalID);
				if(festivalTemp != null)
					entradasTemp = festivalTemp.getEntradas();
		}
						
		for (Entrada entrada : entradasTemp) {
				if (entrada.isVendida()
					&& match(nombreCliente, entrada.getNombreCliente())
					&& match(apellidoCliente, entrada.getApellidoCliente())
					&& match(numeroPuestoDeVenta, entrada.getNumeroPuestoDeVenta())
					&& this.matchDatos(entrada, noche, ubicacion, fechaDesde, fechaHasta)) {
						resultados.add(entrada);
				}
		}
				
		
	return resultados;
}	
		
	private boolean matchDatos(Entrada entrada, Noche noche, Ubicacion ubicacion, Integer fechaDesde, Integer fechaHasta) {
		Integer nroNoche = null;
		Character sector = null;
		Integer fila = null;
		Integer butaca = null;
		
		
		if(noche != null)
			nroNoche = noche.getNroNoche();
		if(ubicacion != null) {
			sector = ubicacion.getSector();
			fila = ubicacion.getFila();
			butaca = ubicacion.getButaca();
		}
		
		return (match(nroNoche, entrada.getNroNoche()) 
				&& match(sector, entrada.getSector()) 
				&& match(fila, entrada.getFila()) 
				&& match(butaca, entrada.getButaca())
				&& (this.matchFechas(entrada.getFechaInicio(), fechaDesde, fechaHasta)));	
	}
	
	public List<Banda> searchBandas(String nombreBanda, String festivalID) {
		List<Banda> resultadoBandas = new ArrayList<Banda>();
		List<Banda> bandasTemp = new ArrayList<Banda>();
		Festival festivalTemp;
		
		if (festivalID == null) {
			for (Festival festival : this.festivales)
				bandasTemp = festival.getBandas();
		}
		else{
			festivalTemp = this.searchFestival(festivalID);
			if(festivalTemp != null)
				bandasTemp = festivalTemp.getBandas();
		}
				
		for (Banda banda : bandasTemp) {
			if (match(nombreBanda, banda.getNombre()))
				resultadoBandas.add(banda);					
		}
		
		return resultadoBandas;		
	}

	private Festival searchFestival(String festivalID){
		for(Festival festival: this.festivales){
			if(festival.getFestivalID() == Integer.parseInt(festivalID))
				return festival;
		}
		return null;
	}
	
	public List<Banda> searchBandasCliente(Cliente cliente, String festivalID){
		List<Entrada> resultadoEntradas = new ArrayList<Entrada>();
		List<Banda> resultadoBandas = new ArrayList<Banda>();
		String nombreCliente = null;
		String apellidoCliente = null;
	
		if(cliente != null){
			nombreCliente = cliente.getNombre();
			apellidoCliente = cliente.getApellido();
		}
		
		if((nombreCliente == null) && (apellidoCliente == null) && (festivalID == null)) {
			return this.searchBandas(null, null);
		}
		
		resultadoEntradas = this.searchOcupadas(null, null, cliente, null, null, null, festivalID);
			
		for(Entrada entrada : resultadoEntradas)
			resultadoBandas.addAll(entrada.getNoche().getBandas());
	
		return resultadoBandas;
	}
	
	protected boolean match(Object expectedValue, Object realValue) {
		return expectedValue == null || realValue.toString().toLowerCase().contains(expectedValue.toString().toLowerCase());
	}
	
	protected boolean matchFechas(Integer fechaInicio, Integer fechaDesde, Integer fechaHasta){
		
		if((fechaDesde == null) || (fechaHasta == null))
			return true;
		else
			return ((fechaDesde <= fechaInicio) && (fechaInicio <= fechaHasta));
	}
}