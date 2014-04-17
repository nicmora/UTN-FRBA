package comprarEntradas.domain;

public class TarjetaCredito {
	
	public Integer numero;
	public Integer credito;
	
	
	public TarjetaCredito() {

	}
	
	
	public TarjetaCredito(Integer numero, Integer credito) {
		this.numero = numero;
		this.credito = credito;
	}
	
	
	public Integer getNumero() {
		return numero;
	}
	

	public void setNumero(Integer numero) {
		this.numero = numero;
	}
	

	public Integer getCredito() {
		return credito;
	}
	

	public void setCredito(Integer credito) {
		this.credito = credito;
	}
	
}
