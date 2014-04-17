package comprarEntradas.domain;

import org.uqbar.commons.utils.Observable;

@Observable
public class Banda {
	
	private String nombre;
	private Categoria categoria;
	
	
	public Banda(String nombre, Categoria categoria) {
		this.nombre = nombre;
		this.categoria = categoria;
	}

	
	public String getNombre(){
		return nombre;
	}
	
	
	public void setNombre(String nombre){
		this.nombre = nombre;
	}
	
	
	public Categoria getCategoria() {
		return categoria;
	}


	public void setCategoria(Categoria categoria) {
		this.categoria = categoria;
	}

	
	public Integer valorCategoria(){
		return this.getCategoria().getValor();
	}
	

	public Integer getNroCategoria(){
		return this.getCategoria().getNro();		
	}

	public void setNroCategoria(Integer nro){
		this.getCategoria().setNro(nro);
	}
	
}
