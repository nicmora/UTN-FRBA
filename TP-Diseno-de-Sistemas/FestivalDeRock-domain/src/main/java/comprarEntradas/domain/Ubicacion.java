package comprarEntradas.domain;

import org.uqbar.commons.utils.Observable;
import org.uqbar.commons.utils.Transactional;

@Observable
@Transactional
public class Ubicacion {
	
	private Character sector;
    private Integer fila;
    private Integer butaca;
    private Integer precio;
    private String sfb;

    
	public Ubicacion(Character sector, Integer fila, Integer butaca, Integer precio) {
		this.sector = sector;
		this.fila = fila;
		this.butaca = butaca;
		this.precio = precio;
		this.sfb = Character.toString(this.sector) + " - " + Integer.toString(this.fila) + " - " + Integer.toString(this.butaca);
	}
    
	
	public Character getSector() {
		return sector;
	}

	
	public void setSector(Character sector) {
		this.sector = sector;
	}

	
	public Integer getFila() {
		return fila;
	}

	
	public void setFila(Integer fila) {
		this.fila = fila;
	}

	
	public Integer getButaca() {
		return butaca;
	}

	
	public void setButaca(Integer butaca) {
		this.butaca = butaca;
	}

	
	public Integer getPrecio() {
		return precio;
	}

	
	public void setPrecio(Integer precio) {
		this.precio = precio;
	}


	public String getSfb() {
		return sfb;
	}


	public void setSfb(String sfb) {
		this.sfb = sfb;
	}
}