package comprarEntradas.domain;

public class Jubilado extends Cliente{

	private static Integer porcentaje = 15;
	
	public Jubilado() {
	}
	
	public Jubilado(String nombre, String apellido, Integer edad) {
		super(nombre, apellido, edad);
	}
		
	@Override
	public Integer calcularDescuentoCliente(Entrada entrada)
	{
		return entrada.getPrecio() * porcentaje / 100;
	}	
}
