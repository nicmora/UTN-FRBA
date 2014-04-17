package comprarEntradas.domain;

import org.uqbar.commons.utils.Observable;


@Observable
public class Entrada {
	
	private Integer idEntrada;
	private Boolean vendida;
	private Boolean vip;
	private Integer precioBase;
	private Integer fechaVenta;
	private Cliente cliente;
	private Noche noche;
	private Ubicacion ubicacion;
	private PuestoDeVenta puestoDeVenta;
	
	
	public Entrada() {
		
	}
	
    public Entrada(Integer idEntrada, Boolean vip, Ubicacion ubicacion, Noche noche, PuestoDeVenta puestoDeVenta) {
		this.idEntrada = idEntrada;
		this.vendida = false;
		this.vip = vip;
		this.precioBase = ubicacion.getPrecio();
		this.noche = noche;
		this.ubicacion = ubicacion;
		this.puestoDeVenta = puestoDeVenta;
	}
	
	public Entrada(Integer idEntrada, Boolean vip, Ubicacion ubicacion) {
		this.idEntrada = idEntrada;
		this.vendida = false;
		this.vip = vip;
		this.precioBase = ubicacion.getPrecio();
		this.ubicacion = ubicacion;
	}
	
	public Integer getIdEntrada() {
		return idEntrada;
	}

    public void setIdEntrada(Integer idEntrada) {
    	this.idEntrada = idEntrada;
    }
    
    public void setVendida(Boolean vendida) {
    	this.vendida = vendida;
    }
        
    public Boolean isVendida() {
    	return vendida;	
    }
        
    public Boolean isVip() {
    	return vip;
    }
    
    public void setVip(Boolean vip) {
    	this.vip = vip;
    }
        
    public Cliente getCliente() {
    	return cliente;
    }
    
    public void setCliente(Cliente cliente) {
    	this.cliente = cliente;
    }
   
    public Integer getPrecioBase() {
    	return precioBase;
    }
      
    public void setPrecioBase(Integer precioBase) {
    	this.precioBase = precioBase;
    }
       
    public Ubicacion getUbicacion() {
    	return ubicacion;
    }
       
    public void setUbicacion(Ubicacion ubicacion) {
    	this.ubicacion = ubicacion;
    }
        
    public Noche getNoche() {
    	return noche;
    }
       
    public void setNoche(Noche noche) {
    	this.noche = noche;
    } 
   
	public Integer getFechaVenta() {
		return fechaVenta;
	}
	
	public void setFechaVenta(Integer fechaVenta) {
		this.fechaVenta = fechaVenta;
	}
	
	public void estasVendida(Cliente cliente, Integer fechaVenta) {
		this.vendida = true;
		this.cliente = cliente;
		this.fechaVenta = fechaVenta;
    }
	
	public Integer getPrecio() {		
		Integer precioExtra = noche.precioExtra();
	    Integer precioTotal = this.precioBase + precioExtra;
	    
	    return precioTotal;
	}
	
	public Integer getNroNoche() {
		return this.noche.getNroNoche();
	}
	
	public char getSector() {
		return this.ubicacion.getSector();
	}
	
	public Integer getFila() {
		return this.ubicacion.getFila();
	}
	
	public Integer getButaca() {
		return this.ubicacion.getButaca();
	}	
	
	public void setNombreCliente(String nombre) {
		this.cliente.setNombre(nombre);
	}
	
	public void setApellidoCliente(String apellido) {
		this.cliente.setApellido(apellido);
	}
	
	public void setEdadCliente(Integer edad) {
		this.cliente.setEdad(edad);
	}

	public String getNombreCliente() {
		return this.cliente.getNombre();
	}
	
	public String getApellidoCliente() {
		return this.cliente.getApellido();
	}
	
	public Integer getFechaInicio() {
		return this.noche.getFechaInicio();
	}
	
	public void setFechaInicio(Integer fechaInicio) {
		this.noche.setFechaInicio(fechaInicio);
	}
	
	public PuestoDeVenta getPuestoDeVenta(){
		return this.puestoDeVenta;
	}
	
	public void setPuestoDeVenta(PuestoDeVenta puestoDeVenta){
		this.puestoDeVenta = puestoDeVenta;
	}
	
	public Integer getNumeroPuestoDeVenta(){
		return this.puestoDeVenta.getNumero();
	}
	
	public void setNumeroPuestoDeVenta(Integer numero){
		this.puestoDeVenta.setNumero(numero);
	}
}
