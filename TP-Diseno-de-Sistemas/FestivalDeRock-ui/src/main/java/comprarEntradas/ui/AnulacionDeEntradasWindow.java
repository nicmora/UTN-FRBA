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
import org.uqbar.arena.widgets.TextBox;
import org.uqbar.arena.widgets.tables.Column;
import org.uqbar.arena.widgets.tables.Table;
import org.uqbar.arena.windows.WindowOwner;
import org.uqbar.commons.utils.Observable;
import org.uqbar.lacar.ui.model.ListBuilder;
import org.uqbar.lacar.ui.model.bindings.Binding;

import comprarEntradas.domain.Entrada;
import comprarEntradas.domain.Noche;
import comprarEntradas.domain.Ubicacion;
import comprarEntradas.repositorio.Repositorio;


//Ventana de búsqueda, selección y anulación de entradas.

@SuppressWarnings("serial")
@Observable
public class AnulacionDeEntradasWindow extends VentaDeEntradasWindow{

	public AnulacionDeEntradasWindow(WindowOwner owner) {
		super(owner);
		this.getModelObject().searchOcupadas();
	}
	
	//El panel principal de búsqueda permite filtrar por sector, fila o butaca.
	
	@Override
	protected void createFormPanel(Panel mainPanel) {
		Panel searchFormPanel = new Panel(mainPanel);
		searchFormPanel.setLayout(new ColumnLayout(8));
		
		new Label(searchFormPanel).setText("Noche").setForeground(Color.BLUE);
		Selector<Entrada> nroNoche = new Selector<Entrada>(searchFormPanel);
		nroNoche.allowNull(false);
		nroNoche.bindValueToProperty("nocheSeleccionada");
		Binding<ListBuilder<Entrada>> nroNocheBinding = nroNoche.bindItems(new ObservableProperty(this.getModel(), "noches"));
		nroNocheBinding.setAdapter(new PropertyAdapter(Noche.class, "nroNoche"));
		
		new Label(searchFormPanel).setText("Sector").setForeground(Color.BLUE);
		Selector<Entrada> sector = new Selector<Entrada>(searchFormPanel);
		sector.allowNull(false);
		sector.bindValueToProperty("ubicacionSeleccionada");
		Binding<ListBuilder<Entrada>> sectorBinding = sector.bindItems(new ObservableProperty(Repositorio.getInstance(), "ubicaciones"));
		sectorBinding.setAdapter(new PropertyAdapter(Ubicacion.class, "sector"));
		
		new Label(searchFormPanel).setText("Fila").setForeground(Color.BLUE);
		Selector<Ubicacion> fila = new Selector<Ubicacion>(searchFormPanel);
		fila.allowNull(false);
		fila.bindValueToProperty("ubicacionSeleccionada");
		Binding<ListBuilder<Ubicacion>> filaBinding = fila.bindItems(new ObservableProperty(Repositorio.getInstance(), "ubicaciones"));
		filaBinding.setAdapter(new PropertyAdapter(Ubicacion.class, "fila"));
		
		new Label(searchFormPanel).setText("Butaca").setForeground(Color.BLUE);
		Selector<Ubicacion> butaca = new Selector<Ubicacion>(searchFormPanel);
		butaca.allowNull(false);
		butaca.bindValueToProperty("ubicacionSeleccionada");
		Binding<ListBuilder<Ubicacion>> butacaBinding = butaca.bindItems(new ObservableProperty(Repositorio.getInstance(), "ubicaciones"));
		butacaBinding.setAdapter(new PropertyAdapter(Ubicacion.class, "butaca"));
		
		new Label(searchFormPanel).setText("Nombre").setForeground(Color.BLUE);
		new TextBox(searchFormPanel).setWidth(50).bindValueToProperty("nombreCliente");
		
		new Label(searchFormPanel).setText("Apellido").setForeground(Color.BLUE);
		new TextBox(searchFormPanel).setWidth(50).bindValueToProperty("apellidoCliente");
	}
	
	//Acciones asociadas a la pantalla.
	
	@Override
	protected void addActions(Panel actionsPanel) {
		new Button(actionsPanel)
			.setCaption("Buscar")
			.onClick(new MessageSend(this.getModelObject(), "searchOcupadas"))
			.setAsDefault()
			.disableOnError();

		new Button(actionsPanel) //
			.setCaption("Limpiar")
			.onClick(new MessageSend(this.getModelObject(), "clear"));
	}
	
	@Override
	protected void createResultsGrid(Panel mainPanel) {
		Table<Entrada> table = new Table<Entrada>(mainPanel, Entrada.class);
		table.setHeigth(250);
		table.setWidth(600);
		table.bindItemsToProperty("resultados");
		table.bindValueToProperty("entradaSeleccionada");

		this.describeResultsGrid(table);
	}
	
	@Override
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
		
		new Column<Entrada>(table)
		.setTitle("Nombre")
		.setFixedSize(80)
		.bindContentsToProperty("nombreCliente");
		
		new Column<Entrada>(table)
		.setTitle("Apellido")
		.setFixedSize(80)
		.bindContentsToProperty("apellidoCliente");	
	}
	
	@Override
	protected void createGridActions(Panel mainPanel) {
		Panel actionsPanel = new Panel(mainPanel);
		actionsPanel.setLayout(new HorizontalLayout());
		
		Button anular = new Button(actionsPanel);
		anular.setCaption("Anular");
		anular.onClick(new MessageSend(this, "anularEntrada"));
	
		// Deshabilita los botones si no hay ningún elemento seleccionado en la grilla.
		
		NotNullObservable elementSelected = new NotNullObservable("entradaSeleccionada");
		anular.bindEnabled(elementSelected);
	}
	
	public void anularEntrada() {
		this.getModelObject().getEntradaSeleccionada().setVendida(false);
		this.getModelObject().searchOcupadas();
	}
}
