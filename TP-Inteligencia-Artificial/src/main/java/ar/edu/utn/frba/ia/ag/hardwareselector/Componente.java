package main.java.ar.edu.utn.frba.ia.ag.hardwareselector;

public abstract class Componente {

	private Integer id;
	private String modelo;
	private Integer precio;
	
	public Integer getId() {
		return id;
	}
	public void setId(Integer id) {
		this.id = id;
	}
	public String getModelo() {
		return modelo;
	}
	public void setModelo(String modelo) {
		this.modelo = modelo;
	}
	public Integer getPrecio() {
		return precio;
	}
	public void setPrecio(Integer precio) {
		this.precio = precio;
	}
	
	public String getCodificacionId() {
		
		String codigo = "";
		
		switch(this.getId()) {
			case 0: {
				codigo = "000";
				break;
			}
			case 1: {
				codigo = "001";
				break;
			}
			case 2: {
				codigo = "010";
				break;
			}
			case 3: {
				codigo = "011";
				break;
			}
			case 4: {
				codigo = "100";
				break;
			}
			case 5: {
				codigo = "101";
				break;
			}
			case 6: {
				codigo = "110";
				break;
			}
			case 7: {
				codigo = "111";
				break;
			}
		}
		
		return codigo;
	}
}
