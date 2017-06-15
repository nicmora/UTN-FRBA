package main.java.ar.edu.utn.frba.ia.ag.hardwareselector;

public class Procesador extends Componente {

	private Integer generacion;
	private String socket;

	public Procesador(Integer i) {
		switch (i) {
			case 0: {
				this.setId(0);
				this.setModelo("INTEL_CORE_I5_4690");
				this.generacion = 4;
				this.socket = "LGA_1150";
				this.setPrecio(4300);
				break;
			}
			case 1: {
				this.setId(1);
				this.setModelo("INTEL_CORE_I7_4790");
				this.generacion = 4;
				this.socket = "LGA_1150";
				this.setPrecio(5500);
				break;
			}
			case 2: {
				this.setId(2);
				this.setModelo("INTEL_CORE_I3_6320");
				this.generacion = 6;
				this.socket = "LGA_1151";
				this.setPrecio(3000);
				break;
			}
			case 3: {
				this.setId(3);
				this.setModelo("INTEL_CORE_I7-6700");
				this.generacion = 6;
				this.socket = "LGA_1151";
				this.setPrecio(5700);
				break;
			}
			case 4: {
				this.setId(4);
				this.setModelo("INTEL_PENTIUM_G4400");
				this.generacion = 1;
				this.socket = "LGA_1150";
				this.setPrecio(1500);
				break;
			}
			case 5: {
				this.setId(5);
				this.setModelo("INTEL_CORE_I5-7600");
				this.generacion = 7;
				this.socket = "LGA_1151";
				this.setPrecio(4500);
				break;
			}
			case 6: {
				this.setId(6);
				this.setModelo("INTEL_CORE_I7-7700");
				this.generacion = 7;
				this.socket = "LGA_1151";
				this.setPrecio(6000);
				break;
			}
			case 7: {
				this.setId(7);
				this.setModelo("INVALIDO");
				this.generacion = 0;
				this.socket = "";
				this.setPrecio(0);
				break;
			}
		}
	}
	
	public Integer getGeneracion() {
		return generacion;
	}
	public void setGeneracion(Integer generacion) {
		this.generacion = generacion;
	}
	public String getSocket() {
		return socket;
	}
	public void setSocket(String socket) {
		this.socket = socket;
	}
	
}
