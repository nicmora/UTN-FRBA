package comprarEntradas.domain;

import java.util.ArrayList;
import java.util.Collection;

import org.uqbar.commons.utils.Observable;

//TODO abstract
@Observable
public class Cliente {	
	
	public TarjetaCredito tarjeta;
	public Collection<Entrada> entradas = new ArrayList<Entrada>();
	private String nombre;
	private String apellido;
	private Integer edad;
	//TODO
	private Boolean jubilado;
	
	public Cliente() {
	}
	
	public Cliente(String nombre, String apellido, Integer edad) {
		this.nombre = nombre;
		this.apellido = apellido;
		this.edad = edad;
	}
	
	public TarjetaCredito getTarjeta() {
		return tarjeta;
	}

	
	public void setTarjeta(TarjetaCredito tarjeta) {
		this.tarjeta = tarjeta;
	}


	public Collection<Entrada> getEntradas() {
		return entradas;
	}

	public void setEntradas(Collection<Entrada> entradas) {
		this.entradas = entradas;
	}
	
    
    public void agregarEntrada(Entrada entrada) {
		entradas.add(entrada);	
	}
    
	
	public Integer calcularDescuentoCliente(Entrada entrada) {
		return 0;
	}
	
	
	public Boolean isDama() {
		return false;
	}


	public String getNombre() {
		return nombre;
	}


	public void setNombre(String nombre) {
		this.nombre = nombre;
	}


	public String getApellido() {
		return apellido;
	}


	public void setApellido(String apellido) {
		this.apellido = apellido;
	}


	public Integer getEdad() {
		return edad;
	}


	public void setEdad(Integer edad) {
		this.edad = edad;
	}

	public Boolean getJubilado() {
		return jubilado;
	}

	public void setJubilado(Boolean jubilado) {
		this.jubilado = jubilado;
	}

}
