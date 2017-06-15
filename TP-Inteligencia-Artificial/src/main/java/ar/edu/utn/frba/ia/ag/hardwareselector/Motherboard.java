package main.java.ar.edu.utn.frba.ia.ag.hardwareselector;

public class Motherboard extends Componente {

	private String socketProcesador;
	private String socketMemoria;
	
	public Motherboard(Integer i) {
		switch (i) {
			case 0: {
				this.setId(0);
				this.setModelo("ASUS_H97_PRO");
				this.socketProcesador = "LGA_1150";
				this.socketMemoria = "DDR3";
				this.setPrecio(1700);
				break;
			}
			case 1: {
				this.setId(1);
				this.setModelo("ASUS_Z97_PRO");
				this.socketProcesador = "LGA_1150";
				this.socketMemoria = "DDR3";
				this.setPrecio(2700);
				break;
			}
			case 2: {
				this.setId(2);
				this.setModelo("ASUS_CROSSHAIR_V_FORMULA-Z");
				this.socketProcesador = "LGA_1150";
				this.socketMemoria = "DDR3";
				this.setPrecio(4300);
				break;
			}
			case 3: {
				this.setId(3);
				this.setModelo("ASUS_H170_PRO");
				this.socketProcesador = "LGA_1151";
				this.socketMemoria = "DDR4";
				this.setPrecio(2200);
				break;
			}
			case 4: {
				this.setId(4);
				this.setModelo("ASUS_Z170_PRO");
				this.socketProcesador = "LGA_1151";
				this.socketMemoria = "DDR4";
				this.setPrecio(2900);
				break;
			}
			case 5: {
				this.setId(5);
				this.setModelo("ASUS_ROG_MAXIMUS_IX_HERO");
				this.socketProcesador = "LGA_1151";
				this.socketMemoria = "DDR4";
				this.setPrecio(5200);
				break;
			}
			case 6: {
				this.setId(6);
				this.setModelo("INVALIDO");
				this.socketProcesador = "";
				this.socketMemoria = "";
				this.setPrecio(0);
				break;
			}
			case 7: {
				this.setId(7);
				this.setModelo("INVALIDO");
				this.socketProcesador = "";
				this.socketMemoria = "";
				this.setPrecio(0);
				break;
			}
		}
	}
	
	public String getSocketProcesador() {
		return socketProcesador;
	}
	public void setSocketProcesador(String socketProcesador) {
		this.socketProcesador = socketProcesador;
	}
	public String getSocketMemoria() {
		return socketMemoria;
	}
	public void setSocketMemoria(String socketMemoria) {
		this.socketMemoria = socketMemoria;
	}
		
}
