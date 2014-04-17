package comprarEntradas.ui;

import java.awt.Color;

import org.uqbar.arena.actions.MessageSend;
import org.uqbar.arena.bindings.NotNullObservable;
import org.uqbar.arena.bindings.ObservableProperty;
import org.uqbar.arena.bindings.PropertyAdapter;
import org.uqbar.arena.layout.ColumnLayout;
import org.uqbar.arena.layout.HorizontalLayout;
import org.uqbar.arena.widgets.Button;
import org.uqbar.arena.widgets.Label;
import org.uqbar.arena.widgets.Panel;
import org.uqbar.arena.widgets.Selector;
import org.uqbar.arena.widgets.tables.Column;
import org.uqbar.arena.widgets.tables.Table;
import org.uqbar.arena.windows.Dialog;
import org.uqbar.arena.windows.SimpleWindow;
import org.uqbar.arena.windows.WindowOwner;
import org.uqbar.commons.utils.Observable;
import org.uqbar.commons.utils.Transactional;
import org.uqbar.lacar.ui.model.ListBuilder;
import org.uqbar.lacar.ui.model.bindings.Binding;

import comprarEntradas.domain.Entrada;
import comprarEntradas.domain.Noche;
import comprarEntradas.domain.Ubicacion;
import comprarEntradas.repositorio.Repositorio;


//Ventana de búsqueda, selección y venta de entradas.

@SuppressWarnings("serial")
@Observable
@Transactional
public class VentaDeEntradasWindow extends SimpleWindow<BuscadorEntrada> {

	public VentaDeEntradasWindow(WindowOwner owner) {
		super(owner, new BuscadorEntrada());
		this.getModelObject().searchDisponibles();
	}
	
	/*
	 * El default de la vista es un formulario que permite disparar la búsqueda (invocando con super). Además 
	 * le agregamos una grilla con los resultados de esa búsqueda y acciones que pueden hacerse con elementos 
	 * de esa búsqueda.
	*/
	
	@Override
	protected void createMainTemplate(Panel mainPanel) {
		this.setTitle("Festival de Rock");
		this.setTaskDescription("Ingrese los parametros de busqueda");

		super.createMainTemplate(mainPanel);

		this.createResultsGrid(mainPanel);
		this.createGridActions(mainPanel);
	}

	// *************************************************************************
	// * FORMULARIO DE BUSQUEDA
	// *************************************************************************

	//El panel principal de búsqueda permite filtrar por sector, fila o butaca.
	
	@Override
	protected void createFormPanel(Panel mainPanel) {
		Panel searchFormPanel = new Panel(mainPanel);
		searchFormPanel.setLayout(new ColumnLayout(4));
		
		new Label(searchFormPanel).setText("Noche").setForeground(Color.BLUE);
		Selector<Entrada> nroNoche = new Selector<Entrada>(searchFormPanel);
		nroNoche.allowNull(false);
		nroNoche.bindValueToProperty("nocheSeleccionada");
		Binding<ListBuilder<Entrada>> nroNocheBinding = nroNoche.bindItems(new ObservableProperty(this.getModel(), "noches"));
		nroNocheBinding.setAdapter(new PropertyAdapter(Noche.class, "nroNoche"));
		
		new Label(searchFormPanel).setText("Ubicacion").setForeground(Color.BLUE);
		Selector<Ubicacion> ubicacion = new Selector<Ubicacion>(searchFormPanel);
		ubicacion.allowNull(false);
		ubicacion.bindValueToProperty("ubicacionSeleccionada");
		Binding<ListBuilder<Ubicacion>> ubicacionBinding = ubicacion.bindItems(new ObservableProperty(Repositorio.getInstance(), "ubicaciones"));
		ubicacionBinding.setAdapter(new PropertyAdapter(Ubicacion.class, "sfb"));
		
//		new Label(searchFormPanel).setText("Fila").setForeground(Color.BLUE);
//		Selector<Ubicacion> fila = new Selector<Ubicacion>(searchFormPanel);
//		fila.allowNull(false);
//		fila.bindValueToProperty("ubicacionSeleccionada");
//		Binding<ListBuilder<Ubicacion>> filaBinding = fila.bindItems(new ObservableProperty(Repositorio.getInstance(), "ubicaciones"));
//		filaBinding.setAdapter(new PropertyAdapter(Ubicacion.class, "fila"));
//		
//		new Label(searchFormPanel).setText("Butaca").setForeground(Color.BLUE);
//		Selector<Ubicacion> butaca = new Selector<Ubicacion>(searchFormPanel);
//		butaca.allowNull(false);
//		butaca.bindValueToProperty("ubicacionSeleccionada");
//		Binding<ListBuilder<Ubicacion>> butacaBinding = butaca.bindItems(new ObservableProperty(Repositorio.getInstance(), "ubicaciones"));
//		butacaBinding.setAdapter(new PropertyAdapter(Ubicacion.class, "butaca"));
	}
	
	//Acciones asociadas a la pantalla.
	
	@Override
	protected void addActions(Panel actionsPanel) {
		new Button(actionsPanel)
			.setCaption("Buscar")
			.onClick(new MessageSend(this.getModelObject(), "searchDisponibles"))
			.setAsDefault()
			.disableOnError();

		new Button(actionsPanel) //
			.setCaption("Limpiar")
			.onClick(new MessageSend(this.getModelObject(), "clear"));
	}
	
	// *************************************************************************
	// ** RESULTADOS DE LA BUSQUEDA
	// *************************************************************************

	/*
	 * Se crea la grilla en el panel de abajo. El binding es: el contenido de la grilla en base a los
	 * resultados de la búsqueda. Cuando el usuario presiona Buscar, se actualiza el model, y éste a su vez
	 * dispara la notificación a la grilla que funciona como Observer.
	 */
	
	protected void createResultsGrid(Panel mainPanel) {
		Table<Entrada> table = new Table<Entrada>(mainPanel, Entrada.class);
		table.setHeigth(250);
		table.setWidth(400);
		table.bindItemsToProperty("resultados");
		table.bindValueToProperty("entradaSeleccionada");

		this.describeResultsGrid(table);
	}
	
	//Define las columnas de la grilla.
	
	protected void describeResultsGrid(Table<Entrada> table) {
		new Column<Entrada>(table)
		.setTitle("Noche")
		.setFixedSize(80)
		.bindContentsToProperty("nroNoche");
		
		new Column<Entrada>(table)
		.setTitle("Sector")
		.setFixedSize(80)
		.bindContentsToProperty("sector");
	
		new Column<Entrada>(table)
		.setTitle("Fila")
		.setFixedSize(80)
		.bindContentsToProperty("fila");
		
		new Column<Entrada>(table)
		.setTitle("Butaca")
		.setFixedSize(80)
		.bindContentsToProperty("butaca");
	
		new Column<Entrada>(table)
		.setTitle("Precio")
		.setFixedSize(80)
		.bindContentsToProperty("precio");
	}
	
	protected void createGridActions(Panel mainPanel) {
		Panel actionsPanel = new Panel(mainPanel);
		actionsPanel.setLayout(new HorizontalLayout());
		
		Button comprar = new Button(actionsPanel);
		comprar.setCaption("Comprar");
		comprar.onClick(new MessageSend(this, "comprarEntrada"));
	
		// Deshabilita los botones si no hay ningún elemento seleccionado en la grilla.
		
		NotNullObservable elementSelected = new NotNullObservable("entradaSeleccionada");
		comprar.bindEnabled(elementSelected);
	}
		
	// ********************************************************
	// ** ACCIONES
	// ********************************************************

	public void comprarEntrada() {
		this.openDialog(new ComprarEntradaWindow(this, this.getModelObject().getEntradaSeleccionada()));
		this.getModelObject().clear();
		this.getModelObject().searchDisponibles();
	}

	protected void openDialog(Dialog<?> dialog) {
		dialog.open();
	}	
}