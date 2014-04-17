package comprarEntradas.domain;

public class PuestoDeVenta {

	private Integer numero;
	private String direccion;
	
	public PuestoDeVenta(Integer numero, String direccion){
		this.numero = numero;
		this.direccion = direccion;
	}
	
	public Integer getNumero() {
		return numero;
	}
	
	public void setNumero(Integer numero) {
		this.numero = numero;
	}
	
	public String getDireccion() {
		return direccion;
	}
	
	public void setDireccion(String direccion) {
		this.direccion = direccion;
	}
}
