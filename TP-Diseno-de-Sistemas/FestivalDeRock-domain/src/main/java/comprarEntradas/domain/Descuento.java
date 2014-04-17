package comprarEntradas.domain;

public class Descuento {
	
	private Integer porcentajeAnticipada = 10;
	private Integer porcentajePrecioCombo = 10;


	public void setPorcentajeAnticipada(Integer porcentajeAnticipada) {
		this.porcentajeAnticipada = porcentajeAnticipada;
	}
	

	public void setPorcentajePrecioCombo(Integer porcentajePrecioCombo) {
		this.porcentajePrecioCombo = porcentajePrecioCombo;
	}
	
	
	public Integer diferenciaEnDias(Integer fecha1, Integer fecha2)
	{
		Integer difFecha = fecha1 - fecha2;
		
		Integer aniosAux = difFecha / 10000;
		Integer mesesAux = (difFecha % 10000) / 100;
		Integer diasAux = difFecha % 100;
		
		return aniosAux * 360 + mesesAux * 30 + diasAux;
	}
	
	
	public Integer calcularDescuentoTotal(Cliente cliente, Entrada entrada, Integer fechaVenta) 
	{
		Integer descuentoAnticipada = 0;
		Integer descuentoCliente = cliente.calcularDescuentoCliente(entrada);
		
		
		if (this.diferenciaEnDias(entrada.getNoche().getFechaInicio(), fechaVenta) > 30)
		{
			descuentoAnticipada = entrada.getPrecio() * porcentajeAnticipada / 100; 
		}
		
		return descuentoCliente + descuentoAnticipada;
	}
	
	
	public Integer calcularDescuentoCombo(Integer precioCombo) 
	{	
		if (precioCombo > 1000)
		{
			return precioCombo * porcentajePrecioCombo / 100;
		}
		else
		{
			return 0;
		}
	}
	
}
