package comprarEntradas.domain;

import java.util.*;

public class Dama extends Cliente {

	private static Integer porcentaje = 20;
	private static Double porcentajeEntradas = 0.20;

	public Dama() {
	}
	
	public Dama(String nombre, String apellido, Integer edad) {
		super(nombre, apellido, edad);
	}
	
	@Override
	public Boolean isDama() {
		return true;
	}
		
	@Override
	public Integer calcularDescuentoCliente(Entrada entrada)
	{
		Collection <Entrada> entradasDamas = new ArrayList<Entrada>();
		Collection <Entrada> entradasDeLaNoche = entrada.getNoche().getEntradas();
		
		for (Entrada entradaAux:entradasDeLaNoche){
		if(entradaAux.isVendida() && entradaAux.getCliente().isDama())
			entradasDamas.add(entradaAux);
		}
		
		Integer cantEntradasDamas = entradasDamas.size();
		
		if ((cantEntradasDamas / entrada.getNoche().getEntradas().size()) < porcentajeEntradas)
		{
			return entrada.getPrecio() * porcentaje / 100;
		}
		else
		{
			return 0;
		}
	}
	
}
