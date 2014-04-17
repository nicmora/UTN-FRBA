package comprarEntradas.domain;

public class Menor12 extends Cliente {

	private static Integer porcentaje = 50;
	private Boolean acompaniado;
	
	public Menor12() {
	}
	
	public Menor12(String nombre, String apellido, Integer edad, Boolean acompaniado) {
		super(nombre, apellido, edad);
		this.acompaniado = acompaniado;
	}
	
	@Override
	public Integer calcularDescuentoCliente(Entrada entrada)
	{
		if (this.acompaniado)
		{
			return entrada.getPrecio() * porcentaje / 100;
		}
		else
		{
			return entrada.getPrecio();
		}
	}
}
