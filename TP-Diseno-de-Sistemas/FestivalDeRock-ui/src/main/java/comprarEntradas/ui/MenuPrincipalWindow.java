package comprarEntradas.ui;

import org.uqbar.arena.actions.MessageSend;
import org.uqbar.arena.layout.ColumnLayout;
import org.uqbar.arena.widgets.Button;
import org.uqbar.arena.widgets.Panel;
import org.uqbar.arena.windows.SimpleWindow;
import org.uqbar.arena.windows.WindowOwner;


//Ventana que contiene el menú principal.

@SuppressWarnings("serial")
public class MenuPrincipalWindow extends SimpleWindow<MenuPrincipal>{

	public MenuPrincipalWindow(WindowOwner owner) {
		super(owner, new MenuPrincipal());
	}

	@Override
	protected void createMainTemplate(Panel mainPanel) {
		this.setTitle("Festival de Rock");
		this.setTaskDescription("Seleccione una opcion");

		super.createMainTemplate(mainPanel);
	}
	
	@Override
	protected void createFormPanel(Panel mainPanel) {
		@SuppressWarnings("unused")
		Panel menuFormPanel = new Panel(mainPanel);
	}
	
	//Acciones asociadas a la pantalla.
	
	@Override
	protected void addActions(Panel actionsPanel) {
		actionsPanel.setLayout(new ColumnLayout(2));
		
		new Button(actionsPanel)
			.setCaption("Vender entrada")
			.onClick(new MessageSend(this, "abrirVentanaVender"));

		new Button(actionsPanel)
			.setCaption("Anular entrada")
			.onClick(new MessageSend(this, "abrirVentanaAnular"));
		
		new Button(actionsPanel)
			.setCaption("Buscar entradas por cliente/fecha")
			.onClick(new MessageSend(this, "buscarEntradasClienteFecha"));
		
		new Button(actionsPanel)
			.setCaption("Buscar entradas por puesto de venta/festival")
			.onClick(new MessageSend(this, "buscarEntradasPuestoDeVentaFestival"));
		
		new Button(actionsPanel)
			.setCaption("Buscar bandas por nombre/festival")
			.onClick(new MessageSend(this, "buscarBandasNombreFestival"));
		
		new Button(actionsPanel)
		.setCaption("Buscar bandas por cliente/festival")
		.onClick(new MessageSend(this, "buscarBandasClienteFestival"));
	}	
	
	public void abrirVentanaVender(){
		new VentaDeEntradasWindow(this).open(); 
	}
	
	public void abrirVentanaAnular(){
		new AnulacionDeEntradasWindow(this).open(); 
	}
	
	public void buscarEntradasClienteFecha(){
		new BuscarEntradasClienteFechaWindow(this).open();
	}
	
	public void buscarEntradasPuestoDeVentaFestival(){
		new BuscarEntradasPuestoDeVentaFestivalWindow(this).open();
	}
	
	public void buscarBandasNombreFestival(){
		new BuscarBandasNombreFestivalWindow(this).open();
	}
	
	public void buscarBandasClienteFestival(){
		new BuscarBandasClienteFestivalWindow(this).open();
	}
	
}