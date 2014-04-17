package comprarEntradas.domain;

import java.text.SimpleDateFormat;
import java.util.*;

public class Venta {
	
    private Collection<Noche> noches = new ArrayList<Noche>();
    private Collection<Cliente> clientes = new ArrayList<Cliente>();
    private PaymentGateway plataformaDePago = new PaymentGateway();
    private Descuento descuento = new Descuento();
    private Integer fecha;
       
    public Venta() {
    	
    }
    
    public Venta(Descuento descuento)
    {
    	this.descuento = descuento;
    	this.fecha = this.obtenerFecha();
    }
    
	public Integer getFecha() {
		return fecha;
	}


	public void setFecha(Integer fecha) {
		this.fecha = fecha;
	}


	public void addNoche(Noche noche) {
		this.noches.add(noche);
	}
	

	public void addCliente(Cliente cliente) {
		this.clientes.add(cliente);
	}
	

	public PaymentGateway getPlataformaDePago() {
		return plataformaDePago;
	}
	

	public void setPlataformaDePago(PaymentGateway plataformaDePago) {
		this.plataformaDePago = plataformaDePago;
	}
	
	
	public void setDescuento(Descuento descuento) {
		this.descuento = descuento;
	}
	

	public Integer comprarEntrada(Cliente cliente, Entrada entrada, String tipoPago)
	{	
		Integer precioFinal = 0;	
		
		if (!entrada.isVendida())
		{					
			precioFinal = calcularPrecioFinal(cliente, entrada);
			
			//TODO Hacer objeto polimorfico tipo de pago
			if (tipoPago == "Tarjeta")
			{
				if (plataformaDePago.procesarPago(precioFinal, cliente))
				{
					cliente.agregarEntrada(entrada);
					entrada.estasVendida(cliente, fecha);
				}
				else
				{
					System.out.println ("No se pudo realizar la compra"); //TODO Quitar interacción con el usuario
				}
			}
			else
			{
				cliente.agregarEntrada(entrada);
				entrada.estasVendida(cliente, fecha);
			}
		}
		
		return precioFinal;
	}
	
	
	public Integer comprarCombo(Cliente cliente, ArrayList<Entrada> entradas, String tipoPago)
	{
		Integer precioCombo = 0;
		Integer precioFinal = 0;
		
		Entrada entradaAux;
		
		while (entradas.size() != 0)
		{			
			entradaAux = entradas.remove(0);
			
			if (entradaAux.isVip())
			{
				precioCombo += comprarEntradaVip(cliente, entradaAux, tipoPago);
			}
			else
			{
				precioCombo += comprarEntrada(cliente, entradaAux, tipoPago);
			}		
		}
		
		precioFinal = calcularPrecioFinalCombo(precioCombo);
		
		return precioFinal;
	}
	
	
	public Integer comprarEntradaVip(Cliente cliente, Entrada entrada, String tipoPago)
	{
		Integer precioFinal = 0;
		Integer recargo = 0;
		
		Entrada entradaAux;
		
		if (!entrada.isVendida())
		{
			for(Noche nocheAux:noches)
			{
				entradaAux = nocheAux.buscarEntradaPorUbicacion(entrada.getUbicacion());
				
				recargo = entradaAux.getPrecio() * 50 / 100;
				precioFinal += calcularPrecioFinal(cliente, entradaAux) + recargo;
				
				if (tipoPago == "Tarjeta")
				{
					if (plataformaDePago.procesarPago(precioFinal, cliente))
					{
						cliente.agregarEntrada(entrada);
						entrada.estasVendida(cliente, fecha);
					}
					else
					{
						System.out.println ("No se pudo realizar la compra");
					}	
				}
				else
				{
					cliente.agregarEntrada(entrada);
					entrada.estasVendida(cliente, fecha);
				}
			}
			
			return precioFinal;	
		}
		
		return -1;
	}
	
	
	private Integer calcularPrecioFinal(Cliente cliente, Entrada entrada)
	{
		Integer precioTotal = entrada.getPrecio();
		Integer descuentoTotal = descuento.calcularDescuentoTotal(cliente, entrada, fecha);
		Integer precioFinal = precioTotal - descuentoTotal;
		
		return precioFinal;
	}
	
	
	private Integer calcularPrecioFinalCombo(Integer precioCombo)
	{
		Integer descuentoCombo = descuento.calcularDescuentoCombo(precioCombo); 
		Integer precioFinal = precioCombo - descuentoCombo;
		
		return precioFinal;
	}
	
	
	public Integer obtenerFecha() 
	{
		 String DATE_FORMAT = "yyyyMMdd";
		 SimpleDateFormat sdf = new SimpleDateFormat(DATE_FORMAT);
		 Calendar calendario = Calendar.getInstance();
		 Integer fecha = Integer.parseInt(sdf.format(calendario.getTime()));
		 
		 return fecha;
	}
}
