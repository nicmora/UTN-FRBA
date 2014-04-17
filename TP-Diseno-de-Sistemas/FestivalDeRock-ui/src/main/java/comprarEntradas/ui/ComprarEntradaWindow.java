package comprarEntradas.ui;

import org.uqbar.arena.actions.MessageSend;
import org.uqbar.arena.aop.windows.TransactionalDialog;
import org.uqbar.arena.bindings.NotNullObservable;
import org.uqbar.arena.layout.ColumnLayout;
import org.uqbar.arena.widgets.Button;
import org.uqbar.arena.widgets.Label;
import org.uqbar.arena.widgets.Panel;
import org.uqbar.arena.widgets.TextBox;
import org.uqbar.arena.windows.WindowOwner;

import comprarEntradas.domain.Cliente;
import comprarEntradas.domain.Entrada;
import comprarEntradas.domain.Venta;


@SuppressWarnings("serial")
public class ComprarEntradaWindow extends TransactionalDialog<Entrada> {
	
	public ComprarEntradaWindow(WindowOwner owner, Entrada entrada) {
		super(owner, entrada);
		entrada.setCliente(new Cliente());
		entrada.setFechaVenta(new Venta().obtenerFecha());		
	}
	
	@Override
	protected void createFormPanel(Panel mainPanel) {
		Panel form = new Panel(mainPanel);
		form.setLayout(new ColumnLayout(2));
		
		new Label(form).setText("Nombre");
		TextBox nombre = new TextBox(form);
		nombre.setWidth(80);
		nombre.bindValueToProperty("cliente.nombre");
		
		new Label(form).setText("Apellido");
		TextBox apellido = new TextBox(form);
		apellido.setWidth(80);
		apellido.bindValueToProperty("cliente.apellido");
		
		new Label(form).setText("Edad");
		TextBox edad = new TextBox(form);
		edad.setWidth(80);
		edad.bindValueToProperty("cliente.edad");
		
//		new Label(form).setText("Jubilado");
//		new CheckBox(form).bindValueToProperty("cliente.jubilado");
	}
	
	@Override
	protected void addActions(Panel actions) {
		Button aceptar = new Button(actions);
		aceptar.setCaption("Aceptar");
		aceptar.onClick(new MessageSend(this, "accept"));
		aceptar.setAsDefault();
		aceptar.disableOnError();

		new Button(actions)
		.setCaption("Cancelar")
		.onClick(new MessageSend(this, "cancel"));
		
		NotNullObservable elementSelected1 = new NotNullObservable("cliente.nombre");
		NotNullObservable elementSelected2 = new NotNullObservable("cliente.apellido");
		NotNullObservable elementSelected3 = new NotNullObservable("cliente.edad");
		aceptar.bindEnabled(elementSelected1);
		aceptar.bindEnabled(elementSelected2);
		aceptar.bindEnabled(elementSelected3);
	}
	
	@Override
	public void accept(){
		this.getModelObject().setVendida(true);
		super.accept();
	}
}