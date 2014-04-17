package comprarEntradas.domain;

import java.util.*;
import org.uqbar.commons.utils.Observable;


@Observable
public class Noche {
	
	private Collection<Entrada> entradas = new ArrayList<Entrada>();
	private Collection<Banda> bandas = new ArrayList<Banda>();
	private Integer nroNoche;
	private Integer fechaInicio;
	
	
	public Noche(Collection<Entrada> entradas, Collection<Banda> bandas, Integer fechaInicio) {
		this.entradas = entradas;
		this.bandas = bandas;
		this.fechaInicio = fechaInicio;
	}
	

	public Noche(Integer nroNoche, Collection<Banda> bandas, Integer fechaInicio) {		
		this.nroNoche = nroNoche;
		this.bandas = bandas;
		this.fechaInicio = fechaInicio;
	}
	
	
	public Collection<Entrada> getEntradas() {
		return entradas;
	}
	

    public void setEntradas(Collection<Entrada> entradas) {
		this.entradas = entradas;
	}
    

    public Collection<Banda> getBandas() {
		return bandas;
	}
    

    public void setBandas(Collection<Banda> bandas) {
		this.bandas = bandas;
	}

    
    public Integer getFechaInicio() {
		return fechaInicio;
	}

    
    public void setFechaInicio(Integer fechaInicio) {
		this.fechaInicio = fechaInicio;
	}
    
    
	public Integer getNroNoche() {
		return nroNoche;
	}


	public void setNroNoche(Integer nroNoche) {
		this.nroNoche = nroNoche;
	}
    

	public Entrada seleccionarEntrada(Integer idEntrada) 
	{
		for (Entrada entrada:entradas)
		{
			if (entrada.getIdEntrada() == idEntrada)
				 return entrada;
		}
		
		return null;
	}
	

	public Integer precioExtra() 
	{
		Integer precioExtra = this.obtenerMejorBandaCategoria().valorCategoria();
		
		return precioExtra;
	}

	
	public Banda obtenerMejorBandaCategoria() 
	{
		Banda bandaMax = new Banda("aux", new Categoria(0,0));
		
		for(Banda banda:bandas)
		{
			if(banda.getNroCategoria() > bandaMax.getNroCategoria())
			bandaMax = banda;
		}
		
		return bandaMax;
	}

	
	public Collection<Entrada> entradasDisponibles()
	{
		Collection<Entrada> entradasAux = new ArrayList<Entrada>();
		
		for (Entrada entrada:entradas)
		{
			if (!entrada.isVendida())
			{
				entradasAux.add(entrada);
			}
		}
		
		return entradasAux;
	}
	
	
	public Entrada buscarEntradaPorUbicacion(Ubicacion ubicacion)
	{
		for (Entrada entradaAux:entradas)
		{
			if (entradaAux.getUbicacion().equals(ubicacion))
			{
				return entradaAux;
			}
		}
		
		return null;
	}
	
    /*public void listarLocalidades()
    {
	    Integer i = 0;
	    System.out.println("Entradas disponibles: ");
	    Collection <Entrada> entradasDisponibles = entradasDisponibles();
	    
	    for(Entrada entrada : entradasDisponibles)
     	{
	    	System.out.println("Entrada nro: "+ i +" Nro. Sector: "+ entrada.getUbicacion().getSector()+" Nro. Fila: "+entrada.getUbicacion().getNroFila()+" Nro. Silla "+entrada.getUbicacion().getButaca());
			i++;
     	}
    }*/
	
}
