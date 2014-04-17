package comprarEntradas.ui;

import java.io.Serializable;
import java.util.List;

import org.uqbar.commons.utils.Observable;
import org.uqbar.commons.utils.Transactional;

import comprarEntradas.domain.Cliente;
import comprarEntradas.domain.Entrada;
import comprarEntradas.domain.Noche;
import comprarEntradas.domain.Ubicacion;
import comprarEntradas.repositorio.Repositorio;

@SuppressWarnings("serial")
@Observable
@Transactional
public class BuscadorEntrada implements Serializable {
	
	private Noche nocheSeleccionada;
	private List<Noche> noches;
	private Ubicacion ubicacionSeleccionada;
	private List<Ubicacion> ubicaciones;
    private String nombreCliente;
    private String apellidoCliente;
    private Integer fechaDesde;
    private Integer fechaHasta;
    private Integer numeroPuestoDeVenta;
    private String festivalID;
	private List<Entrada> resultados;
	private Entrada entradaSeleccionada;

	
	// ********************************************************
	// ** ACCIONES
	// ********************************************************

	public void searchDisponibles(){
		this.resultados = Repositorio.getInstance().searchDisponibles(this.nocheSeleccionada, this.ubicacionSeleccionada);
	}
	
	public void searchOcupadas() {
		Cliente cliente = new Cliente();
		cliente.setNombre(nombreCliente);
		cliente.setApellido(apellidoCliente);
		this.resultados = Repositorio.getInstance().searchOcupadas(this.nocheSeleccionada, this.ubicacionSeleccionada, cliente, this.fechaDesde, this.fechaHasta, this.numeroPuestoDeVenta, this.festivalID);
	}

	public void clear() {
		this.nocheSeleccionada = null;
		this.ubicacionSeleccionada = null;
		this.nombreCliente = null;
		this.apellidoCliente = null;
		this.fechaDesde = null;
		this.fechaHasta = null;
		this.numeroPuestoDeVenta = null;
		this.festivalID = null;
	}

	// ********************************************************
	// ** ACCESSORS
	// ********************************************************

	public Noche getNocheSeleccionada() {
		return nocheSeleccionada;
	}

	public void setNocheSeleccionada(Noche nocheSeleccionada) {
		this.nocheSeleccionada = nocheSeleccionada;
	}

	public List<Noche> getNoches() {
		return Repositorio.getInstance().getNoches();
	}
	
	public Ubicacion getUbicacionSeleccionada(){
		return ubicacionSeleccionada;
	}
	
	public void setUbicacionSeleccionada(Ubicacion ubicacionSeleccionada){
		this.ubicacionSeleccionada = ubicacionSeleccionada;
	}
	
	public List<Ubicacion> getUbicaciones(){
		return Repositorio.getInstance().getUbicaciones();
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
	
	public Integer getFechaDesde() {
		return this.fechaDesde;
	}
	
	public void setFechaDesde(Integer fechaDesde) {
		this.fechaDesde = fechaDesde;
	}
	
	public Integer getFechaHasta() {
		return this.fechaHasta;
	}
	
	public void setFechaHasta(Integer fechaHasta) {
		this.fechaHasta = fechaHasta;
	}
	
	public Integer getNumeroPuestoDeVenta() {
		return this.numeroPuestoDeVenta;
	}
	
	public void setNumeroPuestoDeVenta(Integer numero) {
		this.numeroPuestoDeVenta = numero;
	}
	
	public String getFestivalID() {
		return festivalID;
	}
	
	public void setFestivalID(String festivalID) {
		this.festivalID = festivalID;
	}

	public Entrada getEntradaSeleccionada() {
		return this.entradaSeleccionada;
	}

	public void setEntradaSeleccionada(Entrada entradaSeleccionada) {
		this.entradaSeleccionada = entradaSeleccionada;
	}

	public List<Entrada> getResultados() {
		return this.resultados;
	}

	public void setResultados(List<Entrada> resultados) {
		this.resultados = resultados;
	}
}
