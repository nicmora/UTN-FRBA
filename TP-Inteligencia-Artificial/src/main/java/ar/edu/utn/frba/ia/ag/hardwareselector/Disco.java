package main.java.ar.edu.utn.frba.ia.ag.hardwareselector;

public class Disco extends Componente {

	private String tecnologia;
	private Integer capacidad; // Medido en : GB

	public Disco(Integer i) {
		switch (i) {
			case 0: {
				this.setId(0);
				this.setModelo("TOSHIBA");
				this.capacidad = 250;
				this.tecnologia = "HDD";
				this.setPrecio(700);
				break;
			}
			case 1: {
				this.setId(1);
				this.setModelo("TOSHIBA");
				this.capacidad = 320;
				this.tecnologia = "HDD";
				this.setPrecio(1000);
				break;
			}
			case 2: {
				this.setId(2);
				this.setModelo("WD_CAVIAR_BLUE");
				this.capacidad = 500;
				this.tecnologia = "HDD";
				this.setPrecio(1300);
				break;
			}
			case 3: {
				this.setId(3);
				this.setModelo("WD_CAVIAR_BLACK");
				this.capacidad = 1024;
				this.tecnologia = "HDD";
				this.setPrecio(1600);
				break;
			}
			case 4: {
				this.setId(4);
				this.setModelo("SSD_WD_BLUE");
				this.capacidad = 500;
				this.tecnologia = "SSD";
				this.setPrecio(2600);
				break;
			}
			case 5: {
				this.setId(5);
				this.setModelo("SSD_WD_BLUE");
				this.capacidad = 1024;
				this.tecnologia = "SSD";
				this.setPrecio(4900);
				break;
			}
			case 6: {
				this.setId(6);
				this.setModelo("INVALIDO");
				this.capacidad = 0;
				this.tecnologia = "";
				this.setPrecio(0);
				break;
			}
			case 7: {
				this.setId(7);
				this.setModelo("INVALIDO");
				this.capacidad = 0;
				this.tecnologia = "";
				this.setPrecio(0);
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

	public String getTecnologia() {
		return tecnologia;
	}

	public void setTecnologia(String tecnologia) {
		this.tecnologia = tecnologia;
	}

}
