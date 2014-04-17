package comprarEntradas.ui;

import java.io.Serializable;
import java.util.List;

import org.uqbar.commons.utils.Observable;

import comprarEntradas.repositorio.Repositorio;
import comprarEntradas.domain.Banda;
import comprarEntradas.domain.Cliente;

@SuppressWarnings("serial")
@Observable
public class BuscadorBandas implements Serializable {
	
    private String nombreBanda;
    private String festivalID;
	private List<Banda> resultados;
	private String nombreCliente;
	private String apellidoCliente;
	
	// ********************************************************
	// ** ACCIONES
	// ********************************************************

	public void searchBandas(){
		this.resultados = Repositorio.getInstance().searchBandas(this.nombreBanda, this.festivalID);
	}
	
	public void searchBandasCliente(){
		Cliente cliente = new Cliente();
		cliente.setNombre(nombreCliente);
		cliente.setApellido(apellidoCliente);
		this.resultados = null;
		this.resultados = Repositorio.getInstance().searchBandasCliente(cliente, this.festivalID);
	}

	public void clear() {
		this.nombreBanda = null;
		this.nombreCliente = null;
		this.apellidoCliente = null;
		this.festivalID = null;
	}

	// ********************************************************
	// ** ACCESSORS
	// ********************************************************


	public String getNombreBanda() {
		return nombreBanda;
	}

	public void setNombreBanda(String nombreBanda) {
		this.nombreBanda = nombreBanda;
	}

	public List<Banda> getResultados() {
		return resultados;
	}

	public void setResultados(List<Banda> resultados) {
		this.resultados = resultados;
	}
	
	public String getFestivalID() {
		return festivalID;
	}
	public void setFestivalID(String festivalID) {
		this.festivalID = festivalID;
	}
	
	public String getNombreCliente() {
		return nombreCliente;
	}
	
	public void setNombreCliente(String nombreCliente) {
		this.nombreCliente = nombreCliente;
	}
	
	public String getApellidoCliente() {
		return apellidoCliente;
	}
	
	public void setApellidoCliente(String apellidoCliente) {
		this.apellidoCliente = apellidoCliente;
	}
}