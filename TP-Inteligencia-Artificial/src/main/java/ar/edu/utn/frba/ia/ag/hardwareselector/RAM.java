package main.java.ar.edu.utn.frba.ia.ag.hardwareselector;

public class RAM extends Componente {

	private Integer capacidad;
	private String socket;

	public RAM(Integer i) {
		switch (i) {
			case 0: {
				this.setId(0);
				this.setModelo("KINGSTON_HYPERX_FURY");
				this.capacidad = 16;
				this.socket = "DDR3";
				this.setPrecio(2800);
				break;
			}
			case 1: {
				this.setId(1);
				this.setModelo("KINGSTON_HYPERX_FURY");
				this.capacidad = 8;
				this.socket = "DDR4";
				this.setPrecio(1200);
				break;
			}
			case 2: {
				this.setId(2);
				this.setModelo("KINGSTON_HYPERX_FURY");
				this.capacidad = 16;
				this.socket = "DDR4";
				this.setPrecio(2400);
				break;
			}
			case 3: {
				this.setId(3);
				this.setModelo("CORSAIR_VENGEANCE_(2X16)");
				this.capacidad = 32;
				this.socket = "DDR4";
				this.setPrecio(4800);
				break;
			}
		}
	}

	public Integer getCapacidad() {
		return capacidad;
	}
	public void setCapacidad(Integer capacidad) {
		this.capacidad = capacidad;
	}
	public String getSocket() {
		return socket;
	}
	public void setSocket(String socket) {
		this.socket = socket;
	}
	
}
