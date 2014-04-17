package comprarEntradas.domain;

import java.util.List;

public class Festival {
	
	Integer festivalID;
	List<Noche> noches;
	List<Entrada> entradas;
	List<Banda> bandas;

	public Festival(){
		
	}
	
	public Festival(Integer festivalID, List<Noche> noches, List<Entrada> entradas, List<Banda> bandas)
	{
		this.festivalID = festivalID;
		this.noches = noches;
		this.entradas = entradas;
		this.bandas = bandas;
	}
	
	public Integer getFestivalID() {
		return festivalID;
	}
	
	public void setFestivalID(Integer festivalID) {
		this.festivalID = festivalID;
	}
	
	public List<Noche> getNoches() {
		return noches;
	}
	
	public void setNoches(List<Noche> noches) {
		this.noches = noches;
	}
	
	public List<Entrada> getEntradas() {
		return entradas;
	}
	
	public void setEntradas(List<Entrada> entradas) {
		this.entradas = entradas;
	}
	
	public List<Banda> getBandas() {
		return bandas;
	}
	
	public void setBandas(List<Banda> bandas) {
		this.bandas = bandas;
	}		
}
