package comprarEntradas.domain;

public class Categoria {
	
	private Integer nro;
	private Integer valor;
    
    
	public Categoria(Integer nro, Integer valor) {
		this.nro = nro;
		this.valor = valor;
	}
	

	public Integer getValor() {
		return valor;
	}
	

	public void setValor(Integer valor) {
		this.valor = valor;
	}
	

	public Integer getNro() {
		return nro;
	}
	

	public void setNro(Integer nro) {
		this.nro = nro;
	}
	
}
