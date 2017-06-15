package main.java.ar.edu.utn.frba.ia.ag.hardwareselector;

import java.util.Random;

import main.java.ar.edu.utn.frba.ia.ag.Individuo;

public class Hardware extends Individuo {

	//--- Atributos ---//
	private Procesador procesador;
	private Motherboard motherboard;
	private RAM ram;
	private Disco disco;
	private SO so;
	
	//--- Getters & Setters ---//
	public Procesador getProcesador() {
		return procesador;
	}

	public void setProcesador(Procesador procesador) {
		this.procesador = procesador;
	}

	public Motherboard getMotherboard() {
		return motherboard;
	}

	public void setMotherboard(Motherboard motherboard) {
		this.motherboard = motherboard;
	}

	public RAM getRam() {
		return ram;
	}

	public void setRam(RAM ram) {
		this.ram = ram;
	}

	public Disco getDisco() {
		return disco;
	}

	public void setDisco(Disco disco) {
		this.disco = disco;
	}
	
	public SO getSo() {
		return so;
	}

	public void setSo(SO so) {
		this.so = so;
	}

	@Override
    public Individuo generarRandom() {
    	
    	Hardware hardware = new Hardware();
    	Random random = new Random();
    	
        // random.nextInt(max - min + 1) + min
    	hardware.setProcesador(new Procesador(random.nextInt(7 - 0 + 1) + 0));
    	hardware.setMotherboard(new Motherboard(random.nextInt(7 - 0 + 1) + 0));
    	hardware.setRam(new RAM(random.nextInt(3 - 0 + 1) + 0));
    	hardware.setDisco(new Disco(random.nextInt(7 - 0 + 1) + 0));
    	hardware.setSo(new SO(random.nextInt(3 - 0 + 1) + 0));

    	return hardware;
    }
    

	//--- Funcion de aptitud ---//
	@Override
	public double aptitud() {

		double total = 0;
		
		total += this.verificarCondicionesDeseables();
		total += this.verificarCombinacionesInvalidas();
		
		return total;
	}

	private double verificarCondicionesDeseables() {

		double puntosAptitud = 0;
		
		//Si el procesador es Core i7, sumar 10 puntos.
		if(this.procesador.getModelo().contains("INTEL_CORE_I7")) {
			puntosAptitud += 5;
		}
		
		//Si el procesador es Core i5, sumar 3 puntos.
		if(this.procesador.getModelo().contains("INTEL_CORE_I5")) {
			puntosAptitud += 2;
		}
		
		//Si el procesador es Core i3, sumar 1 puntos.
		if(this.procesador.getModelo().contains("INTEL_CORE_I3")) {
			puntosAptitud += 1;
		}
		
		//Si el procesador es 7ma Gen, sumar 2 puntos.
		if(this.procesador.getGeneracion().equals(7)) {
			puntosAptitud += 2;
		}
		
		//Si el procesador es 6ta Gen, sumar 1 puntos.
		if(this.procesador.getGeneracion().equals(6)) {
			puntosAptitud += 1;
		}
		
		//Si la RAM es DDR4, sumar 8 puntos.
		if(this.ram.getSocket().equals("DDR4")) {
			puntosAptitud += 4;
		}
		
		//Si el Disco tiene una Capacidad superior a 320 GB, sumar 6 puntos.
		if(this.ram.getCapacidad() > 8) {
			puntosAptitud += 2;
		}
		
		//Si la RAM es superior a 4 GB y el SO tiene arquitectura x64, sumar 5 puntos.
		if(this.ram.getCapacidad() > 4 && this.so.getArquitectura().equals("x64")) {
			puntosAptitud += 1;
		}
		
		//Si el Disco tiene una Capacidad superior a 320 GB, sumar 6 puntos.
		if(this.disco.getCapacidad() > 320) {
			puntosAptitud += 3;
		}
		
		//Si el Disco es SSD, sumar 3 puntos.
		if(this.disco.getTecnologia().equals("SSD")) {
			puntosAptitud += 1;
		}
		
		//Si el Sistema Operativo es Windows 10 Home Edition, sumar 1 puntos.
		if(this.so.getModelo().equals("WINDOWS_10_HOME_EDITION")) {
			puntosAptitud += 1;
		}
		
		return puntosAptitud;
	}

	
	private double verificarCombinacionesInvalidas() {

		double puntosAptitud = 0;
		
		// Cualquier Procesador que posea una característica marcada como inválida.	
		if (this.getProcesador().getModelo().equals("INVALIDO")) {
			puntosAptitud -= 5;
		}
		
		// Cualquier Motherboard que posea una característica marcada como inválida.	
		if (this.getMotherboard().getModelo().equals("INVALIDO")) {
			puntosAptitud -= 5;
		}
		
		// Cualquier Disco que posea una característica marcada como inválida.
		if(this.disco.getModelo().equals("INVALIDO")) {
			puntosAptitud -= 5;
		}
		
		// Si el procesador es 4ta Gen y el Motherboard no tiene Socket 1150.
		if(this.procesador.getGeneracion() == 4 && !this.motherboard.getSocketProcesador().equals("LGA_1150")) {
			puntosAptitud -= 5;
		}
		
		// Si el procesador es 6ta Gen o 7ma Gen y el Motherboard no tiene Socket 1151.
		if((this.procesador.getGeneracion() == 6 || this.procesador.getGeneracion() == 7) && !this.motherboard.getSocketProcesador().equals("LGA_1151")) {
			puntosAptitud -= 5;
		}
		
		// Si la RAM es DDR4 y el Motherboard no soporta la tecnología DDR4.
		if(this.ram.getSocket().equals("DDR4") && !this.motherboard.getSocketMemoria().equals("DDR4")) {
			puntosAptitud -= 5;
		}
		
		// Si la RAM es DDR3 y el Motherboard no soporta la tecnología DDR3.
		if(this.ram.getSocket().equals("DDR3") && !this.motherboard.getSocketMemoria().equals("DDR3")) {
			puntosAptitud -= 5;
		}
			
		//Si la suma total del precio de todas las prestaciones es mayor a $18.500.
		if(this.procesador.getPrecio() != null 
			&& this.motherboard.getPrecio() != null
			&& this.ram.getPrecio() != null
			&& this.disco.getPrecio() != null
			&& this.so.getPrecio() != null) {
			
			Integer precioTotal = this.procesador.getPrecio() + this.motherboard.getPrecio() + this.ram.getPrecio() + this.disco.getPrecio() + this.so.getPrecio();
			
			if(precioTotal > 15500) {
				puntosAptitud -= 5;
			}
		}
		
		return puntosAptitud;
	}
	
    @Override
    public String toString() {
      
       StringBuilder strBuilder = new StringBuilder();
       strBuilder.append("\n");
       strBuilder.append("Cromosoma = ").append(this.getCromosoma()).append("\n");
       strBuilder.append("Aptitud = ").append(this.aptitud()).append("\n");
       strBuilder.append("Procesador: ").append(this.procesador.getModelo()).append("\n");
       strBuilder.append("Motherboard: ").append(this.motherboard.getModelo()).append("\n");
       strBuilder.append("RAM: ").append(this.ram.getModelo()).append(" ").append(this.ram.getCapacidad()).append("GB").append(" ").append(this.ram.getSocket()).append("\n");
       strBuilder.append("Disco: ").append(this.disco.getModelo()).append(" ").append(this.disco.getCapacidad()).append("GB").append(" ").append(this.disco.getTecnologia()).append("\n");
       strBuilder.append("SO: ").append(this.so.getModelo()).append(" ").append(this.so.getArquitectura()).append("\n");
       strBuilder.append("Precio = ").append(this.procesador.getPrecio() + this.motherboard.getPrecio() + this.ram.getPrecio() + this.disco.getPrecio() + this.so.getPrecio()).append("\n");
      
       return strBuilder.toString();
    }
    
//    @Override
//    public String toString() {
//      
//       StringBuilder strBuilder = new StringBuilder();
//       strBuilder.append(this.getCromosoma());
//      
//       return strBuilder.toString();
//    }
    
    public String getCromosoma() {
    	
        StringBuilder strBuilder = new StringBuilder();
        strBuilder.append("[")
        			.append(this.procesador.getCodificacionId()).append("|")
        			.append(this.motherboard.getCodificacionId()).append("|")
        			.append(this.ram.getCodificacionId()).append("|")
        			.append(this.disco.getCodificacionId()).append("|")
        			.append(this.so.getCodificacionId())
        			.append("]");
        
        return strBuilder.toString();
    }
	
}