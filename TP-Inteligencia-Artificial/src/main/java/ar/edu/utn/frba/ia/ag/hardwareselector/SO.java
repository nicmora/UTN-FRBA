package main.java.ar.edu.utn.frba.ia.ag.hardwareselector;

public class SO extends Componente {

	private String arquitectura;
	
	public SO(Integer i) {
		switch (i) {
			case 0: {
				this.setId(0);
				this.setModelo("WINDOWS_10_HOME_EDITION");
				this.arquitectura = "x86";
				this.setPrecio(2200);
				break;
			}
			case 1: {
				this.setId(1);
				this.setModelo("WINDOWS_10_HOME_EDITION");
				this.arquitectura = "x64";
				this.setPrecio(2500);
				break;
			}
			case 2: {
				this.setId(2);
				this.setModelo("WINDOWS_7_ENTERPRISE_EDITION");
				this.arquitectura = "x86";
				this.setPrecio(1500);
				break;
			}
			case 3: {
				this.setId(3);
				this.setModelo("WINDOWS_7_ENTERPRISE_EDITION");
				this.arquitectura = "x64";
				this.setPrecio(1800);
				break;
			}
		}
	}

	public String getArquitectura() {
		return arquitectura;
	}

	public void setArquitectura(String arquitectura) {
		this.arquitectura = arquitectura;
	}
	
}
